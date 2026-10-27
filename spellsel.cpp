#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Game.h"
#include "CoreArchive.h"
#include "spellsel.h"

//  Placement new, for the construction in place of SPELLSEL_ControlsDrawBoundKeyLabel's static buffer.
#include <new>

//  spellsel.cpp -- the skill-select panel of D2Client.dll 1.10: the popup a skill button opens, its skill icons and hotkey labels, tooltips, and
//  the mouse and command handlers that bind and cycle skills; then the spinner control (a two-arrow step control with its proxy, whose one instance
//  is the gold dialog's step control) and two proxy forwarders.
//  The original source file is UI\spellsel.cpp (the assert of SPELLSEL_MiniSkillOnLButtonDown names it). Address range 0x6FB185C0..0x6FB1A79A.
static_assert(UI_MINISKILL == 3,
              "the skill-select popup's panel id, as UI_SetUIToggle is given it");
static_assert(offsetof(D2SkillStrc, pSkillsTxt) == 0x00
                  && offsetof(D2SkillStrc, pNextSkill) == 0x04,
              "the skill list is walked through the SECOND field, and the "
              "first is the record it names");
static_assert(NUMBER_OF_PLAYERCLASSES == 7,
              "gClassSkillIcons has one record per class, and the original "
              "bounds it with the literal 7 -- `cmp eax, 7` in "
              "SPELLSEL_GetSkillIconImage, the end address in the sweep");
static_assert(offsetof(D2ClientClassSkillIconStrc, szFile) == 4,
              "the file name is read four bytes into the record -- "
              "0x6FB8D0CC + 34 * nClass in SPELLSEL_Init");
static_assert(HOTKEY_SKILL1 == 0x0E && HOTKEY_SKILL2 == 0x0F && HOTKEY_SKILL3 == 0x10
                  && HOTKEY_SKILL4 == 0x11 && HOTKEY_SKILL5 == 0x12
                  && HOTKEY_SKILL6 == 0x13 && HOTKEY_SKILL7 == 0x14
                  && HOTKEY_SKILL8 == 0x15 && HOTKEY_SKILL9 == 0x2E
                  && HOTKEY_SKILL10 == 0x2F && HOTKEY_SKILL11 == 0x30
                  && HOTKEY_SKILL12 == 0x31 && HOTKEY_SKILL13 == 0x32
                  && HOTKEY_SKILL14 == 0x33 && HOTKEY_SKILL15 == 0x34
                  && HOTKEY_SKILL16 == 0x35,
              "the sixteen hotkey slots' actions, as both hotkey-label "
              "functions carry them in `mov ecx, <action>`: 0x0E..0x15, "
              "then 0x2E..0x35");
static_assert(SKILL_THROW == 2 && SKILL_LEFTHANDTHROW == 4,
              "SPELLSEL_DrawSkillButtonIcon's two thrown-stack skills: "
              "`sub eax, 2` / `jz` and `sub eax, 2` / `jnz` at 0x6FB18C15");
static_assert(offsetof(D2SkillDescTxt, nListRow) == 5 && offsetof(D2SkillDescTxt, nListPool) == 6,
              "SPELLSEL_DrawPanelSkills reads the row at [ebp+5] (0x6FB198B8) and the "
              "pool at [ebp+6] (0x6FB19918)");
static_assert(SKILLSFLAGINDEX_LEFTSKILL == 30 && SKILLSFLAGINDEX_PASSIVE == 4
                  && SKILLSFLAGINDEX2_SCROLL == 4,
              "SPELLSEL_DrawPanelSkills' three flag tests, one byte each: [edx+7] "
              "against gdwBitMasks[6] (0x6FB19829), [edx+4] against gdwBitMasks[4] "
              "(0x6FB19835), [edx+8] against gdwBitMasks[4] (0x6FB19898) -- "
              "dwFlags is at +4, so byte 3 bit 6 of the first word, byte 0 bit 4, "
              "and byte 0 bit 4 of the second");
static_assert(offsetof(D2ClientSpinnerStrc, nX) == 0x08 && offsetof(D2ClientSpinnerStrc, nY) == 0x0C
                  && offsetof(D2ClientSpinnerStrc, nUpX) == 0x10 && offsetof(D2ClientSpinnerStrc, nUpY) == 0x14
                  && offsetof(D2ClientSpinnerStrc, nDownX) == 0x18
                  && offsetof(D2ClientSpinnerStrc, nDownY) == 0x1C
                  && offsetof(D2ClientSpinnerStrc, bWheelStep) == 0x20
                  && offsetof(D2ClientSpinnerStrc, nCelHeight) == 0x24
                  && offsetof(D2ClientSpinnerStrc, nCelWidth) == 0x28
                  && offsetof(D2ClientSpinnerStrc, bHorizontal) == 0x2C
                  && offsetof(D2ClientSpinnerStrc, bMouseDown) == 0x30
                  && offsetof(D2ClientSpinnerStrc, bUpPressed) == 0x34
                  && offsetof(D2ClientSpinnerStrc, bDownPressed) == 0x38
                  && offsetof(D2ClientSpinnerStrc, pfnOnChange) == 0x3C
                  && offsetof(D2ClientSpinnerStrc, nRepeatTicks) == 0x40
                  && offsetof(D2ClientSpinnerStrc, dwPressTick) == 0x44
                  && offsetof(D2ClientSpinnerStrc, dwLastStepTick) == 0x48
                  && offsetof(D2ClientSpinnerStrc, bFirstStep) == 0x4C
                  && offsetof(D2ClientSpinnerStrc, hCelFile) == 0x50,
              "the spinner as SPELLSEL_UiSpinnerConstruct stores it: [esi+8] .. [esi+50h], "
              "every member one dword, twenty-one of them -- `push 54h` at "
              "0x6FB1A728 is the allocation");


//  The assert strings, as the original pushes them.
static const char szAssertHUnit[] = "hUnit";
static const char szAssertHSkill[] = "hSkill";
static const char szAssertFile[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\UI\\spellsel.cpp";

//  The shared skill-icon sheet's name.
static const char aSpellsSkillico[] = "Spells\\Skillicon";

//  The spinner's data root and Storm's allocation tags.
static const char aDataGlobal[] = "DATA\\GLOBAL";
static const char szNew[] = "new";
static const char szDelete[] = "delete";

//1.10f: D2Client.0x6FB185C0
//  Starts the panel up: loads the shared skill-icon sheet and the player's own class sheet, then clears six blocks of state. gClassSkillIcons is
//  seven records, one per class in class order, each a handle and a file name (Spells\AmSkillicon, SoSkillicon, NeSkillicon, PaSkillicon,
//  BaSkillicon, DrSkillicon, AsSkillicon). The record index is read twice, once for the file name and again after UI_LoadImageFromUiDir returns,
//  for the store; the two statements below keep that order.
//  The panel clear is 965 bytes, not 964: `mov ecx, 0F1h` / `rep stosd` / `stosb` from gSpellSelPanel, 241 dwords and one byte -- the four
//  sixty-entry arrays nX, nY, nSkillId and OwnerGUID, the byte count nCount at +0x3C0 and nLastTextColor, the DWORD that SPELLSEL_DrawPanelSkills
//  stores one byte past the count. The owner GUIDs are then set to -1 over the zeroes.
//  Two arrays here are written and never read: gnSpellSelSlotsUnread, whose one reference is this clear, and gnSpellSelInitTailUnread, four stores
//  and no reader.
void __cdecl SPELLSEL_Init()
{
    D2CellFileStrc *hClassIcons;

    ghSkillIconImage = UI_LoadImageFromUiDir(aSpellsSkillico);
    hClassIcons = UI_LoadImageFromUiDir(gClassSkillIcons[gnPlayerClass].szFile);
    gClassSkillIcons[gnPlayerClass].hImage = hClassIcons;

    memset(gnSpellSelSlotsUnread, 0xFF, 16 * sizeof(int32_t));
    memset(gnSpellSelOwnerGUIDs, 0xFF, 16 * sizeof(D2UnitGUID));
    memset(gnSpellSelHotkeyMode, 0xFF, 16 * sizeof(int32_t));
    memset(gnSpellSelSkillIds, 0xFF, 16 * sizeof(int32_t));
    memset(&gSpellSelPanel, 0, sizeof(gSpellSelPanel));
    memset(gSpellSelPanel.OwnerGUID, 0xFF, 60 * sizeof(D2UnitGUID));
    gnSpellSelInitTailUnread[0] = 0;
    gnSpellSelInitTailUnread[1] = 0;
    gnSpellSelInitTailUnread[2] = 0;
    gnSpellSelInitTailUnread[3] = 0;
}

//1.10f: D2Client.0x6FB18680
//  Tears the panel down: unloads the shared sheet and the player's class sheet, then sweeps every record of gClassSkillIcons. The player's sheet is
//  not unloaded twice: the explicit arm stores NULL after its unload and the sweep tests every handle before unloading it, so the sweep passes that
//  record over. The explicit arm re-reads gnPlayerClass after its unload, and so does the code.
//  The original's sweep is bounded by an address (the first byte past the seventh record) and compared signed; every address in this image is below
//  0x80000000, so signed and unsigned agree. It is written as the count, as SPELLSEL_MiniSkillOnLButtonDown does with its own address bound; the
//  original tests at the bottom, and with seven records the first pass always runs, so a `for` is the same program.
void __cdecl SPELLSEL_Shutdown()
{
    int32_t i;

    if (ghSkillIconImage)
    {
        ARCHIVE_CellFileFree(ghSkillIconImage);
        ghSkillIconImage = NULL;
    }

    if (gClassSkillIcons[gnPlayerClass].hImage)
    {
        ARCHIVE_CellFileFree(gClassSkillIcons[gnPlayerClass].hImage);
        gClassSkillIcons[gnPlayerClass].hImage = NULL;
    }

    for (i = 0; i < NUMBER_OF_PLAYERCLASSES; ++i)
    {
        if (gClassSkillIcons[i].hImage)
        {
            ARCHIVE_CellFileFree(gClassSkillIcons[i].hImage);
            gClassSkillIcons[i].hImage = NULL;
        }
    }
}

//1.10f: D2Client.0x6FB18700
//  Returns the icon sheet a skill is drawn from, loaded on first use. A NULL argument asks for the player's own class sheet; otherwise the skill's
//  class picks the record, and a class outside 0..6 gets the shared sheet. The argument is a pointer to the skill id, read as a word.
//  The range test is two compares in the original (`< 0` and `>= 7`) and is kept as two. Only the second can fire: SKILLS_GetClassIdFromSkillId
//  returns the skill's charclass when it is not negative and 7 otherwise, so a skill of no class reaches the shared sheet through `>= 7`.
D2CellFileStrc *__fastcall SPELLSEL_GetSkillIconImage(int16_t *pnSkillId)
{
    int32_t nClass;

    if (!pnSkillId)
        return gClassSkillIcons[gnPlayerClass].hImage;

    nClass = D2COMMON_SKILLS_GetClassIdFromSkillId((D2C_Skills)*pnSkillId);
    if (nClass < 0 || nClass >= NUMBER_OF_PLAYERCLASSES)
        return ghSkillIconImage;

    if (!gClassSkillIcons[nClass].hImage)
        gClassSkillIcons[nClass].hImage = UI_LoadImageFromUiDir(gClassSkillIcons[nClass].szFile);

    return gClassSkillIcons[nClass].hImage;
}

//1.10f: D2Client.0x6FB18760
//  Sets gnMiniSkillMode = nMode. It is called right after UI_SetUIToggle opens or closes UI_MINISKILL.
void __fastcall SPELLSEL_SetMiniSkillMode(int32_t nMode)
{
    gnMiniSkillMode = nMode;
}

//1.10f: D2Client.0x6FB18770
//  Returns the look a skill button is drawn with, from the skill's use state: 0 when usable, 4 for an aura, 1 for anything else. The original's
//  `sub eax, 6` / `neg` / `sbb` / `and al, 0FDh` / `add eax, 4` is MSVC's branchless choice between two constants, so it is written as a
//  conditional.
int32_t __fastcall SPELLSEL_GetSkillIconState(D2UnitStrc *pUnit, D2SkillStrc *pSkill)
{
    D2C_SkillUseStates nUseState = PLRSKILLS_SkillsGetUseStateForClient(pUnit, pSkill);

    if (nUseState == SKILLUSTATE_USABLE)
        return 0;

    return nUseState == SKILLUSTATE_AURA ? 4 : 1;
}

//1.10f: D2Client.0x6FB18790
//  Draws the charges tooltip of a skill button: while the cursor is over the button's 48 x 48 box, the skill's description, "Quantity :", the
//  charge count and the skill's second description, framed above the button. The selector (SPELLSEL_DrawPanelSkills) and the control panel
//  (PANEL_SpellSelDrawSkillButton, which passes its icon state as nTextColor) call it.
//  The hit test runs twice, each run fetching the mouse afresh: the box is anchored bottom-left, x in [nX, nX + 48] and y in [nY - 48, nY], every
//  compare signed. The first run only picks the colour: 1 outside the box, 0 inside unless the caller passed 1. The second decides whether anything
//  is drawn, so the colour picked outside the box is never used.
//  The text is built in this order: description 1 (SKILLS_GetSkillDescStringIndex(nSkillId, 1), strcpy), tbl 3998 `newline`, tbl 4188 `skilldesc4`
//  ("Quantity "), tbl 3997 `colon`, the charge count ("%i"), tbl 3998 `newline`, description 0 (strcat). Both description indices are fetched
//  before the first copy, field 0 first. The two Unicode buffers are constructed where the original constructs them (256 elements right after the
//  newline string is fetched, 10 after the third strcat). The char buffer is the frame's twelve bytes: "%i" of an int needs at most twelve, and
//  toUnicode is told nine.
void __fastcall SPELLSEL_DrawSkillChargesTooltip(int16_t *pnSkillId, int32_t nTextColor, int32_t nCharges, int32_t nX, int32_t nY)
{
    int32_t nMouseX;
    int32_t nMouseY;
    int32_t nDrawColor;

    nDrawColor = nTextColor;
    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();
    if (nMouseX < nX || nMouseX > nX + 48 || nMouseY < nY - 48 || nMouseY > nY)
        nDrawColor = 1;
    else if (nTextColor != 1)
        nDrawColor = 0;

    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();
    if (nMouseX >= nX && nMouseX <= nX + 48 && nMouseY >= nY - 48 && nMouseY <= nY)
    {
        const Unicode *pwszNewline = D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline);
        Unicode wszText[256];
        int16_t nDescIndex0 = SKILLS_GetSkillDescStringIndex((D2C_Skills)*pnSkillId, 0);
        int16_t nDescIndex1 = SKILLS_GetSkillDescStringIndex((D2C_Skills)*pnSkillId, 1);

        Unicode::strcpy(wszText, D2LANG_GetStringFromTblIndex((D2C_StringIndices)nDescIndex1));
        Unicode::strcat(wszText, pwszNewline);
        Unicode::strcat(wszText, D2LANG_GetStringFromTblIndex(STR_IDX_4188_skilldesc4));
        Unicode::strcat(wszText, D2LANG_GetStringFromTblIndex(STR_IDX_3997_colon));

        Unicode wszCharges[10];
        char szCharges[12];

        wsprintfA(szCharges, "%i", nCharges);
        Unicode::toUnicode(wszCharges, szCharges, 9);
        Unicode::strcat(wszText, wszCharges);
        Unicode::strcat(wszText, pwszNewline);
        Unicode::strcat(wszText, D2LANG_GetStringFromTblIndex((D2C_StringIndices)nDescIndex0));
        D2WIN_10129_DrawFramedText(wszText, nX, nY - 50, nDrawColor, 1);
    }
}

//1.10f: D2Client.0x6FB18940
//  Draws the finishing-move tooltip: the same box and the same two hit tests as the charges tooltip, and on a hover the text comes from
//  SKILLDESC_SkillsBuildFinishingMoveText for the player. PANEL_SpellSelDrawSkillButton calls it.
//  The value handed on is a colour: 1 outside the box and, inside it, 1 only when the caller passed 1 (the charges tooltip's rule in another
//  shape); it lands in the callee's nTextColor. The skill id is read before the player is fetched.
void __fastcall SPELLSEL_DrawFinishingMoveTooltip(int16_t *pnSkillId, int32_t nTextColor, int32_t nSkillLevel, int32_t nCharges, int32_t nX, int32_t nY)
{
    int32_t nMouseX;
    int32_t nMouseY;
    int32_t nDrawColor;

    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();
    if (nMouseX < nX || nMouseX > nX + 48 || nMouseY < nY - 48 || nMouseY > nY)
        nDrawColor = 1;
    else
        nDrawColor = nTextColor == 1;

    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();
    if (nMouseX >= nX && nMouseX <= nX + 48 && nMouseY >= nY - 48 && nMouseY <= nY)
    {
        int32_t nSkillId = *pnSkillId;
        D2UnitStrc *pPlayer = CUNIT_GetPlayer();

        SKILLDESC_SkillsBuildFinishingMoveText(pPlayer, nSkillId, nSkillLevel, nCharges, nDrawColor, nX, nY);
    }
}

//1.10f: D2Client.0x6FB189E0
//  Draws the key bound to a command as a label: nothing when the command is unbound, otherwise the key's name, cut from the end until it is no
//  wider than 40 pixels and, when wider than 13, moved left so that its right edge sits at nX + 13 (a name of 13 or less is not moved). The two
//  hotkey-label functions below call it.
//  The buffer is a function-local static in the original: gwszBoundKeyLabel (Unicode[16]) is constructed on first use under bit 0 of
//  gnBoundKeyLabelGuard, and an atexit handler is registered for it. The storage is the original's, which the original's copy of this function
//  still uses, so both halves share one guard and one buffer: the guard and the construction are written out, in the original's order, over the
//  D2Vars.h globals. The construction calls D2Lang's one-argument constructor with 0 where the original passes the default-constructor closure;
//  both store one zero word and nothing else. SPELLSEL_Nullsub2 is the destructor the compiler generated for a type with none; ours registers our
//  SPELLSEL_Nullsub2 with our CRT's atexit list, which runs at this DLL's unload instead of D2Client.dll's. Both do nothing.
//  The unbound test is 16 bits wide (`cmp ax, 0FFFFh`) and CMDTBL_ControlsFindBoundKey returns an int16_t, so `== -1` compares the same value.
//  BUG: `Unicode::strncpy(gwszBoundKeyLabel, pwszKeyName, 16)` fills all sixteen elements and, like the CRT's, writes no terminator when the source
//  is sixteen characters or longer. The `Unicode::strlen` that follows then reads on past the buffer's end into the next global,
//  gnSpellSelHotkeySlot, and the walk back writes its first terminator one element before wherever strlen stopped, which can be inside that global.
//  Reproduced: the pointer arithmetic is on the original's memory, as there.
void __fastcall SPELLSEL_ControlsDrawBoundKeyLabel(D2HotkeyIndices eCmd, int32_t nX, int32_t nY, int32_t nColor)
{
    const Unicode *pwszKeyName;
    int32_t nTextWidth;
    Unicode *pTail;
    int32_t i;

    if (CMDTBL_ControlsFindBoundKey(eCmd, 1) == -1)
        return;

    if (!(gnBoundKeyLabelGuard & 1))
    {
        gnBoundKeyLabelGuard |= 1;
        for (i = 0; i < 16; ++i)
            new (&gwszBoundKeyLabel[i]) Unicode(0);
        atexit(SPELLSEL_Nullsub2);
    }

    pwszKeyName = CMDTBL_ControlsGetBoundKeyName(eCmd, 1);
    nTextWidth = D2WIN_10121_GetTextWidth(pwszKeyName);
    if (nTextWidth > 40)
    {
        Unicode::strncpy(gwszBoundKeyLabel, pwszKeyName, 16);
        pwszKeyName = gwszBoundKeyLabel;
        pTail = &gwszBoundKeyLabel[Unicode::strlen(gwszBoundKeyLabel)];
        do
        {
            --pTail;
            pTail->ch = 0;
            nTextWidth = D2WIN_10121_GetTextWidth(gwszBoundKeyLabel);
        } while (nTextWidth > 40);
    }

    if (nTextWidth > 13)
        nX += 13 - nTextWidth;

    D2WIN_10117_DrawText(pwszKeyName, nX, nY, nColor, 0);
}

//1.10f: D2Client.0x6FB18AB0
//  A one-byte `retn`: the atexit handler SPELLSEL_ControlsDrawBoundKeyLabel registers for its static buffer, which has nothing to destroy.
void __cdecl SPELLSEL_Nullsub2()
{
}

//1.10f: D2Client.0x6FB18AC0
//  Draws one skill icon on a control-panel button: the skill's cel from the sheet the caller chose, its hotkey label, and a count at its
//  bottom-left corner. PANEL_SpellSelDrawSkillButton calls it once per hand. nCharges (the fourth parameter) is the value drawn when no thrown
//  stack supplies one, never a coordinate. nOwnerGUID is a D2UnitGUID, the type of the hotkey arrays the Mode functions compare it with.
//  The cel: the skills.txt row by the skill id, then the skilldesc.txt row by its `wSkillDesc`, each bounded and each pointer tested for NULL, each
//  failure leaving without drawing anything; the cel is the skilldesc row's byte `nIconCel`.
//  The palette on hover: inside the 48 x 48 box, anchored bottom-left like every skill icon here, the shift becomes 0 for 0, 1 for 1, 4 for 4 and 1
//  for anything else (the idiom SPELLSEL_GetSkillIconState uses); outside it, the caller's value. The caller passes SPELLSEL_GetSkillIconState's
//  answer, 0, 1 or 4, which the hover leaves as it was, so for this caller the hover test changes nothing; it is kept as a test.
//  The count: for Throw (skill 2), the quantity of the stack GetLeftHandWeapon returns, when it is throwable and stackable. For Left Hand Throw
//  (4), the item in the right arm -- or in the left arm when the right arm holds that same weapon -- when it is a weapon, throwable and stackable.
//  Anything else, and every failing test, draws nCharges. Both GetItemFromBodyLoc calls are made before either is tested. Only a value above zero
//  is drawn.
//  The text is the value's `_itoa`, widened into sixteen Unicodes and cut from the end while wider than 40 pixels; the first pass writes the
//  terminator that is already there (esi is at the strlen, before `sub esi, 2`), so a cut takes one pass more than it needs. After the cut the
//  width is measured once more and the answer dropped, then the text is drawn at (nXPos + 4, nYPos - 1) in colour 3. The sixteen Unicodes are
//  constructed where the original constructs them, after the count is known.
//  The original inlines DATATBLS_GetSkillsTxtRecordById here.
void __fastcall SPELLSEL_DrawSkillButtonIcon(D2CellFileStrc *hSkillIconImage, int16_t *pnSkillId, D2UnitGUID nOwnerGUID, int32_t nCharges, int32_t nPaletteShift, int32_t nXPos, int32_t nYPos, int32_t bLeftHand)
{
    int32_t nSkillId;
    D2SkillsTxt *pSkillsTxt;
    int32_t wSkillDesc;
    D2SkillDescTxt *pSkillDescTxt;
    uint32_t nIconCel;
    int32_t nMouseX;
    int32_t nMouseY;
    int32_t nDrawShift;
    D2GfxDataStrc pData;
    D2UnitStrc *pPlayer;
    D2InventoryStrc *pInventory;
    D2UnitStrc *pWeapon;
    D2UnitStrc *pThrowItem;
    D2UnitStrc *pLArmItem;
    int32_t nValue;
    char szValue[16];
    int32_t nTextLen;
    Unicode *pTail;

    nSkillId = *pnSkillId;
    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return;
    wSkillDesc = (int16_t)pSkillsTxt->wSkillDesc;
    if (wSkillDesc < 0 || wSkillDesc >= sgptDataTables->nSkillDescTxtRecordCount)
        return;
    pSkillDescTxt = &sgptDataTables->pSkillDescTxt[wSkillDesc];
    if (!pSkillDescTxt)
        return;
    nIconCel = pSkillDescTxt->nIconCel;

    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();
    if (nMouseX >= nXPos && nMouseX <= nXPos + 48 && nMouseY >= nYPos - 48 && nMouseY <= nYPos)
    {
        if (nPaletteShift == 0)
            nDrawShift = 0;
        else if (nPaletteShift == 1)
            nDrawShift = 1;
        else
            nDrawShift = nPaletteShift == 4 ? 4 : 1;
    }
    else
    {
        nDrawShift = nPaletteShift;
    }

    memset(&pData, 0, sizeof(pData));
    pData.pCellFile = hSkillIconImage;
    pData.nDirection = 0;
    pData.nFrame = nIconCel;
    D2GFX_TEXTURE_CelDrawColor(&pData, nXPos, nYPos, 0xFFFFFFFF, 5, nDrawShift);

    if (bLeftHand)
        SPELLSEL_DrawHotkeyLabelMode1(pnSkillId, nOwnerGUID, nXPos, nYPos);
    else
        SPELLSEL_DrawHotkeyLabelMode0(pnSkillId, nOwnerGUID, nXPos, nYPos);

    nValue = nCharges;
    pPlayer = CUNIT_GetPlayer();
    if (pPlayer && (pInventory = pPlayer->pInventory) != NULL)
    {
        if (*pnSkillId == SKILL_THROW)
        {
            pWeapon = D2COMMON_INVENTORY_GetLeftHandWeapon(pInventory);
            if (pWeapon && D2COMMON_ITEMS_CheckIfThrowable(pWeapon) && D2COMMON_ITEMS_CheckIfStackable(pWeapon))
            {
                nValue = D2COMMON_STATLIST_UnitGetStatValue(pWeapon, STAT_QUANTITY, 0);
            }
        }
        else if (*pnSkillId == SKILL_LEFTHANDTHROW)
        {
            pWeapon = D2COMMON_INVENTORY_GetLeftHandWeapon(pInventory);
            pThrowItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_RARM);
            pLArmItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_LARM);
            if (pWeapon == pThrowItem)
                pThrowItem = pLArmItem;
            if (pThrowItem && D2COMMON_ITEMS_CheckItemTypeId(pThrowItem, ITEMTYPE_WEAPON) && D2COMMON_ITEMS_CheckIfThrowable(pThrowItem) && D2COMMON_ITEMS_CheckIfStackable(pThrowItem))
            {
                nValue = D2COMMON_STATLIST_UnitGetStatValue(pThrowItem, STAT_QUANTITY, 0);
            }
        }
    }

    Unicode wszText[16];

    if (nValue > 0)
    {
        Unicode::toUnicode(wszText, _itoa(nValue, szValue, 10), 16);
        nTextLen = Unicode::strlen(wszText);
        if (D2WIN_10121_GetTextWidth(wszText) > 40)
        {
            pTail = &wszText[nTextLen];
            do
            {
                pTail->ch = 0;
                --pTail;
            } while (D2WIN_10121_GetTextWidth(wszText) > 40);
        }
        D2WIN_10121_GetTextWidth(wszText);
        D2WIN_10117_DrawText(wszText, nXPos + 4, nYPos - 1, 3, 0);
    }
}

//1.10f: D2Client.0x6FB18D50
//  Draws the hotkey label on a skill icon for slots in mode 1: for each of the sixteen hotkey slots holding this skill of this owner, the key bound
//  to that slot's command, at (nX + 34, nY - 35) in colour 4. SPELLSEL_DrawSkillButtonIcon and SPELLSEL_DrawSelectorSkillIcon each choose Mode1 or
//  Mode0 by their own flag: Mode1 for the left skill button (bLeftHand 1) and for the selector when bGrowRight is set, Mode0 otherwise.
//  The sixteen slots are written out, one block each, with the same four tests in the same order: the skill id against the slot (before the -1
//  test, so an unset slot is rejected only after its id has matched), the owner, the -1 test, the mode. `*pnSkillId` is re-read for every slot. The
//  commands are immediates, 0x0E..0x15 and then 0x2E..0x35, asserted at the head of this file; no table in the image holds them. The last block
//  adds -35 to nY in place where the others use a separate sum, because nothing reads nY afterwards.
void __fastcall SPELLSEL_DrawHotkeyLabelMode1(int16_t *pnSkillId, D2UnitGUID nOwnerGUID, int32_t nX, int32_t nY)
{
    if (gnSpellSelSkillIds[0] == *pnSkillId && gnSpellSelOwnerGUIDs[0] == nOwnerGUID && gnSpellSelSkillIds[0] != -1 && gnSpellSelHotkeyMode[0] == 1)
        SPELLSEL_ControlsDrawBoundKeyLabel(HOTKEY_SKILL1, nX + 34, nY - 35, 4);
    if (gnSpellSelSkillIds[1] == *pnSkillId && gnSpellSelOwnerGUIDs[1] == nOwnerGUID && gnSpellSelSkillIds[1] != -1 && gnSpellSelHotkeyMode[1] == 1)
        SPELLSEL_ControlsDrawBoundKeyLabel(HOTKEY_SKILL2, nX + 34, nY - 35, 4);
    if (gnSpellSelSkillIds[2] == *pnSkillId && gnSpellSelOwnerGUIDs[2] == nOwnerGUID && gnSpellSelSkillIds[2] != -1 && gnSpellSelHotkeyMode[2] == 1)
        SPELLSEL_ControlsDrawBoundKeyLabel(HOTKEY_SKILL3, nX + 34, nY - 35, 4);
    if (gnSpellSelSkillIds[3] == *pnSkillId && gnSpellSelOwnerGUIDs[3] == nOwnerGUID && gnSpellSelSkillIds[3] != -1 && gnSpellSelHotkeyMode[3] == 1)
        SPELLSEL_ControlsDrawBoundKeyLabel(HOTKEY_SKILL4, nX + 34, nY - 35, 4);
    if (gnSpellSelSkillIds[4] == *pnSkillId && gnSpellSelOwnerGUIDs[4] == nOwnerGUID && gnSpellSelSkillIds[4] != -1 && gnSpellSelHotkeyMode[4] == 1)
        SPELLSEL_ControlsDrawBoundKeyLabel(HOTKEY_SKILL5, nX + 34, nY - 35, 4);
    if (gnSpellSelSkillIds[5] == *pnSkillId && gnSpellSelOwnerGUIDs[5] == nOwnerGUID && gnSpellSelSkillIds[5] != -1 && gnSpellSelHotkeyMode[5] == 1)
        SPELLSEL_ControlsDrawBoundKeyLabel(HOTKEY_SKILL6, nX + 34, nY - 35, 4);
    if (gnSpellSelSkillIds[6] == *pnSkillId && gnSpellSelOwnerGUIDs[6] == nOwnerGUID && gnSpellSelSkillIds[6] != -1 && gnSpellSelHotkeyMode[6] == 1)
        SPELLSEL_ControlsDrawBoundKeyLabel(HOTKEY_SKILL7, nX + 34, nY - 35, 4);
    if (gnSpellSelSkillIds[7] == *pnSkillId && gnSpellSelOwnerGUIDs[7] == nOwnerGUID && gnSpellSelSkillIds[7] != -1 && gnSpellSelHotkeyMode[7] == 1)
        SPELLSEL_ControlsDrawBoundKeyLabel(HOTKEY_SKILL8, nX + 34, nY - 35, 4);
    if (gnSpellSelSkillIds[8] == *pnSkillId && gnSpellSelOwnerGUIDs[8] == nOwnerGUID && gnSpellSelSkillIds[8] != -1 && gnSpellSelHotkeyMode[8] == 1)
        SPELLSEL_ControlsDrawBoundKeyLabel(HOTKEY_SKILL9, nX + 34, nY - 35, 4);
    if (gnSpellSelSkillIds[9] == *pnSkillId && gnSpellSelOwnerGUIDs[9] == nOwnerGUID && gnSpellSelSkillIds[9] != -1 && gnSpellSelHotkeyMode[9] == 1)
        SPELLSEL_ControlsDrawBoundKeyLabel(HOTKEY_SKILL10, nX + 34, nY - 35, 4);
    if (gnSpellSelSkillIds[10] == *pnSkillId && gnSpellSelOwnerGUIDs[10] == nOwnerGUID && gnSpellSelSkillIds[10] != -1 && gnSpellSelHotkeyMode[10] == 1)
        SPELLSEL_ControlsDrawBoundKeyLabel(HOTKEY_SKILL11, nX + 34, nY - 35, 4);
    if (gnSpellSelSkillIds[11] == *pnSkillId && gnSpellSelOwnerGUIDs[11] == nOwnerGUID && gnSpellSelSkillIds[11] != -1 && gnSpellSelHotkeyMode[11] == 1)
        SPELLSEL_ControlsDrawBoundKeyLabel(HOTKEY_SKILL12, nX + 34, nY - 35, 4);
    if (gnSpellSelSkillIds[12] == *pnSkillId && gnSpellSelOwnerGUIDs[12] == nOwnerGUID && gnSpellSelSkillIds[12] != -1 && gnSpellSelHotkeyMode[12] == 1)
        SPELLSEL_ControlsDrawBoundKeyLabel(HOTKEY_SKILL13, nX + 34, nY - 35, 4);
    if (gnSpellSelSkillIds[13] == *pnSkillId && gnSpellSelOwnerGUIDs[13] == nOwnerGUID && gnSpellSelSkillIds[13] != -1 && gnSpellSelHotkeyMode[13] == 1)
        SPELLSEL_ControlsDrawBoundKeyLabel(HOTKEY_SKILL14, nX + 34, nY - 35, 4);
    if (gnSpellSelSkillIds[14] == *pnSkillId && gnSpellSelOwnerGUIDs[14] == nOwnerGUID && gnSpellSelSkillIds[14] != -1 && gnSpellSelHotkeyMode[14] == 1)
        SPELLSEL_ControlsDrawBoundKeyLabel(HOTKEY_SKILL15, nX + 34, nY - 35, 4);
    if (gnSpellSelSkillIds[15] == *pnSkillId && gnSpellSelOwnerGUIDs[15] == nOwnerGUID && gnSpellSelSkillIds[15] != -1 && gnSpellSelHotkeyMode[15] == 1)
        SPELLSEL_ControlsDrawBoundKeyLabel(HOTKEY_SKILL16, nX + 34, nY - 35, 4);
}

//1.10f: D2Client.0x6FB190C0
//  The same labels for slots in mode 0: SPELLSEL_DrawHotkeyLabelMode1 block for block, the one difference the mode test (`test eax, eax` where
//  Mode1 compares with 1). A separate definition, as the original has two.
void __fastcall SPELLSEL_DrawHotkeyLabelMode0(int16_t *pnSkillId, D2UnitGUID nOwnerGUID, int32_t nX, int32_t nY)
{
    if (gnSpellSelSkillIds[0] == *pnSkillId && gnSpellSelOwnerGUIDs[0] == nOwnerGUID && gnSpellSelSkillIds[0] != -1 && gnSpellSelHotkeyMode[0] == 0)
        SPELLSEL_ControlsDrawBoundKeyLabel(HOTKEY_SKILL1, nX + 34, nY - 35, 4);
    if (gnSpellSelSkillIds[1] == *pnSkillId && gnSpellSelOwnerGUIDs[1] == nOwnerGUID && gnSpellSelSkillIds[1] != -1 && gnSpellSelHotkeyMode[1] == 0)
        SPELLSEL_ControlsDrawBoundKeyLabel(HOTKEY_SKILL2, nX + 34, nY - 35, 4);
    if (gnSpellSelSkillIds[2] == *pnSkillId && gnSpellSelOwnerGUIDs[2] == nOwnerGUID && gnSpellSelSkillIds[2] != -1 && gnSpellSelHotkeyMode[2] == 0)
        SPELLSEL_ControlsDrawBoundKeyLabel(HOTKEY_SKILL3, nX + 34, nY - 35, 4);
    if (gnSpellSelSkillIds[3] == *pnSkillId && gnSpellSelOwnerGUIDs[3] == nOwnerGUID && gnSpellSelSkillIds[3] != -1 && gnSpellSelHotkeyMode[3] == 0)
        SPELLSEL_ControlsDrawBoundKeyLabel(HOTKEY_SKILL4, nX + 34, nY - 35, 4);
    if (gnSpellSelSkillIds[4] == *pnSkillId && gnSpellSelOwnerGUIDs[4] == nOwnerGUID && gnSpellSelSkillIds[4] != -1 && gnSpellSelHotkeyMode[4] == 0)
        SPELLSEL_ControlsDrawBoundKeyLabel(HOTKEY_SKILL5, nX + 34, nY - 35, 4);
    if (gnSpellSelSkillIds[5] == *pnSkillId && gnSpellSelOwnerGUIDs[5] == nOwnerGUID && gnSpellSelSkillIds[5] != -1 && gnSpellSelHotkeyMode[5] == 0)
        SPELLSEL_ControlsDrawBoundKeyLabel(HOTKEY_SKILL6, nX + 34, nY - 35, 4);
    if (gnSpellSelSkillIds[6] == *pnSkillId && gnSpellSelOwnerGUIDs[6] == nOwnerGUID && gnSpellSelSkillIds[6] != -1 && gnSpellSelHotkeyMode[6] == 0)
        SPELLSEL_ControlsDrawBoundKeyLabel(HOTKEY_SKILL7, nX + 34, nY - 35, 4);
    if (gnSpellSelSkillIds[7] == *pnSkillId && gnSpellSelOwnerGUIDs[7] == nOwnerGUID && gnSpellSelSkillIds[7] != -1 && gnSpellSelHotkeyMode[7] == 0)
        SPELLSEL_ControlsDrawBoundKeyLabel(HOTKEY_SKILL8, nX + 34, nY - 35, 4);
    if (gnSpellSelSkillIds[8] == *pnSkillId && gnSpellSelOwnerGUIDs[8] == nOwnerGUID && gnSpellSelSkillIds[8] != -1 && gnSpellSelHotkeyMode[8] == 0)
        SPELLSEL_ControlsDrawBoundKeyLabel(HOTKEY_SKILL9, nX + 34, nY - 35, 4);
    if (gnSpellSelSkillIds[9] == *pnSkillId && gnSpellSelOwnerGUIDs[9] == nOwnerGUID && gnSpellSelSkillIds[9] != -1 && gnSpellSelHotkeyMode[9] == 0)
        SPELLSEL_ControlsDrawBoundKeyLabel(HOTKEY_SKILL10, nX + 34, nY - 35, 4);
    if (gnSpellSelSkillIds[10] == *pnSkillId && gnSpellSelOwnerGUIDs[10] == nOwnerGUID && gnSpellSelSkillIds[10] != -1 && gnSpellSelHotkeyMode[10] == 0)
        SPELLSEL_ControlsDrawBoundKeyLabel(HOTKEY_SKILL11, nX + 34, nY - 35, 4);
    if (gnSpellSelSkillIds[11] == *pnSkillId && gnSpellSelOwnerGUIDs[11] == nOwnerGUID && gnSpellSelSkillIds[11] != -1 && gnSpellSelHotkeyMode[11] == 0)
        SPELLSEL_ControlsDrawBoundKeyLabel(HOTKEY_SKILL12, nX + 34, nY - 35, 4);
    if (gnSpellSelSkillIds[12] == *pnSkillId && gnSpellSelOwnerGUIDs[12] == nOwnerGUID && gnSpellSelSkillIds[12] != -1 && gnSpellSelHotkeyMode[12] == 0)
        SPELLSEL_ControlsDrawBoundKeyLabel(HOTKEY_SKILL13, nX + 34, nY - 35, 4);
    if (gnSpellSelSkillIds[13] == *pnSkillId && gnSpellSelOwnerGUIDs[13] == nOwnerGUID && gnSpellSelSkillIds[13] != -1 && gnSpellSelHotkeyMode[13] == 0)
        SPELLSEL_ControlsDrawBoundKeyLabel(HOTKEY_SKILL14, nX + 34, nY - 35, 4);
    if (gnSpellSelSkillIds[14] == *pnSkillId && gnSpellSelOwnerGUIDs[14] == nOwnerGUID && gnSpellSelSkillIds[14] != -1 && gnSpellSelHotkeyMode[14] == 0)
        SPELLSEL_ControlsDrawBoundKeyLabel(HOTKEY_SKILL15, nX + 34, nY - 35, 4);
    if (gnSpellSelSkillIds[15] == *pnSkillId && gnSpellSelOwnerGUIDs[15] == nOwnerGUID && gnSpellSelSkillIds[15] != -1 && gnSpellSelHotkeyMode[15] == 0)
        SPELLSEL_ControlsDrawBoundKeyLabel(HOTKEY_SKILL16, nX + 34, nY - 35, 4);
}

//1.10f: D2Client.0x6FB19430
//  Draws one skill icon in the selector panel: SPELLSEL_DrawSkillButtonIcon's draw, label and count with two differences -- the icon sheet is
//  looked up here rather than passed in, and the count is the caller's nValue with no inventory test. SPELLSEL_DrawPanelSkills calls it, with
//  bGrowRight choosing Mode1 or Mode0 as bLeftHand does there.
//  The sheet lookup is SPELLSEL_GetSkillIconImage's body for a non-NULL argument, written out without that function's `if (!pnSkillId)` test: the
//  class from SKILLS_GetClassIdFromSkillId, `< 0` and `>= 7` as two tests to the shared sheet, and otherwise the class record's handle, loaded on
//  first use. It runs before the table hops, so a skill with no skilldesc row still loads its class sheet.
//  The palette, the draw, the Mode dispatch and the count's text are SPELLSEL_DrawSkillButtonIcon's, block for block, including the wasted first
//  cut pass and the dropped width. nPaletteShift is SPELLSEL_GetSkillIconState's 0, 1 or 4, so here too the hover palette maps every value it is
//  given to itself.
//  The original also inlines DATATBLS_GetSkillsTxtRecordById here.
void __fastcall SPELLSEL_DrawSelectorSkillIcon(int16_t *pnSkillId, D2UnitGUID nOwnerGUID, int32_t nValue, int32_t nPaletteShift, int32_t nXPos, int32_t nYPos, int32_t bGrowRight)
{
    int32_t nClass;
    D2CellFileStrc *hSkillIconImage;
    int32_t nSkillId;
    D2SkillsTxt *pSkillsTxt;
    int32_t wSkillDesc;
    D2SkillDescTxt *pSkillDescTxt;
    uint32_t nIconCel;
    int32_t nMouseX;
    int32_t nMouseY;
    int32_t nDrawShift;
    D2GfxDataStrc pData;
    char szValue[16];
    int32_t nTextLen;
    Unicode *pTail;

    nClass = D2COMMON_SKILLS_GetClassIdFromSkillId((D2C_Skills)*pnSkillId);
    if (nClass < 0 || nClass >= NUMBER_OF_PLAYERCLASSES)
    {
        hSkillIconImage = ghSkillIconImage;
    }
    else
    {
        if (!gClassSkillIcons[nClass].hImage)
            gClassSkillIcons[nClass].hImage = UI_LoadImageFromUiDir(gClassSkillIcons[nClass].szFile);
        hSkillIconImage = gClassSkillIcons[nClass].hImage;
    }

    nSkillId = *pnSkillId;
    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return;
    wSkillDesc = (int16_t)pSkillsTxt->wSkillDesc;
    if (wSkillDesc < 0 || wSkillDesc >= sgptDataTables->nSkillDescTxtRecordCount)
        return;
    pSkillDescTxt = &sgptDataTables->pSkillDescTxt[wSkillDesc];
    if (!pSkillDescTxt)
        return;
    nIconCel = pSkillDescTxt->nIconCel;

    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();
    if (nMouseX >= nXPos && nMouseX <= nXPos + 48 && nMouseY >= nYPos - 48 && nMouseY <= nYPos)
    {
        if (nPaletteShift == 0)
            nDrawShift = 0;
        else if (nPaletteShift == 1)
            nDrawShift = 1;
        else
            nDrawShift = nPaletteShift == 4 ? 4 : 1;
    }
    else
    {
        nDrawShift = nPaletteShift;
    }

    memset(&pData, 0, sizeof(pData));
    pData.pCellFile = hSkillIconImage;
    pData.nDirection = 0;
    pData.nFrame = nIconCel;
    D2GFX_TEXTURE_CelDrawColor(&pData, nXPos, nYPos, 0xFFFFFFFF, 5, nDrawShift);

    if (bGrowRight)
        SPELLSEL_DrawHotkeyLabelMode1(pnSkillId, nOwnerGUID, nXPos, nYPos);
    else
        SPELLSEL_DrawHotkeyLabelMode0(pnSkillId, nOwnerGUID, nXPos, nYPos);

    Unicode wszText[16];

    if (nValue > 0)
    {
        Unicode::toUnicode(wszText, _itoa(nValue, szValue, 10), 16);
        nTextLen = Unicode::strlen(wszText);
        if (D2WIN_10121_GetTextWidth(wszText) > 40)
        {
            pTail = &wszText[nTextLen];
            do
            {
                pTail->ch = 0;
                --pTail;
            } while (D2WIN_10121_GetTextWidth(wszText) > 40);
        }
        D2WIN_10121_GetTextWidth(wszText);
        D2WIN_10117_DrawText(wszText, nXPos + 4, nYPos - 1, 3, 0);
    }
}

//1.10f: D2Client.0x6FB19660
//  Draws the skill-select popup for one mouse button: the panel state is cleared, then SPELLSEL_DrawPanelSkills runs twice -- the registering pass
//  and the hover pass -- on the side gnMiniSkillMode names: the left panel at x 80 growing right when it is set, the right one at gnScreenWidth -
//  128 growing left when it is not.
//  The original is four independent tests, `if (m) L0; if (!m) R0; if (m) L1; if (!m) R1;`, with gnMiniSkillMode loaded afresh before each. Nothing
//  SPELLSEL_DrawPanelSkills calls writes the mode, so in practice one side is drawn, twice. Only the registering pass resets the panel count; the
//  hover pass registers nothing. The clear is SPELLSEL_Init's: the 965 bytes of gSpellSelPanel, then the owner GUIDs to -1.
//  void: the eax the original leaves is the mode just tested on one path and SPELLSEL_DrawPanelSkills' leftover on the other; the caller does not
//  read it.
void __cdecl SPELLSEL_DrawBothPanels()
{
    memset(&gSpellSelPanel, 0, sizeof(gSpellSelPanel));
    memset(gSpellSelPanel.OwnerGUID, 0xFF, 60 * sizeof(D2UnitGUID));

    if (gnMiniSkillMode)
    {
        gSpellSelPanel.nCount = 0;
        SPELLSEL_DrawPanelSkills(CUNIT_GetPlayer(), 80, 1, 0, 0);
    }

    if (!gnMiniSkillMode)
    {
        int32_t nStartX = gnScreenWidth - 128;

        gSpellSelPanel.nCount = 0;
        SPELLSEL_DrawPanelSkills(CUNIT_GetPlayer(), nStartX, 0, 0, 0);
    }

    if (gnMiniSkillMode)
        SPELLSEL_DrawPanelSkills(CUNIT_GetPlayer(), 80, 1, 1, 0);

    if (!gnMiniSkillMode)
    {
        int32_t nStartX = gnScreenWidth - 128;

        SPELLSEL_DrawPanelSkills(CUNIT_GetPlayer(), nStartX, 0, 1, 0);
    }
}

//1.10f: D2Client.0x6FB19720
//  Draws one panel of the skill-select popup: the player's skills laid out in five rows, each drawn or hover-tested. SPELLSEL_DrawBothPanels calls
//  it four times, two sides by two passes, bSkipIconDraw 0 every time.
//  Layout: the rows are walked 0..4 and the whole skill list is walked again for each. A skill is placed in row iRow when (rows 0..3) it is not an
//  item skill and skilldesc.txt's ListRow is iRow, or (row 4) it IS an item skill. "Item skill" is SKILLS_GetSkillInfo's answer, TRUE only when the
//  skill has an owner GUID, which it then writes out with the charges (untouched otherwise, hence the -1 and 0 stored first); or, on the right
//  panel only, a skill with a quantity above zero whose second flag word has `scroll` set: the tomes. The row's Y starts at gnScreenHeight - 86 and
//  moves up 48 the first time a skill is placed in a row above the last one used, so an empty row takes no space. The X starts at nStartX for every
//  row and steps 48, right when bGrowRight and left otherwise.
//  Four filters come first: the record must survive the skills.txt and skilldesc.txt lookups; on the left panel skills.txt's `leftskill` must be
//  set; `passive` must not be; and bit 8 of the skill's own flags (SKILLS_GetFlags) must be clear (the code keeps the bare 8). The three skills.txt
//  flags are tested one byte at a time, so the code casts both sides to a byte. A skilldesc ListPool admits one skill per panel pass: a 256-byte
//  array cleared at the start marks each non-zero pool on first use, and a later skill of the same pool is skipped; the `< 256` test on a value
//  just masked to a byte is in the original. nListRow is read with `movsx`, so the code compares it signed.
//  bHoverTextPass 0 draws the icon unless bSkipIconDraw, and registers the slot for SPELLSEL_MiniSkillOnLButtonDown: X, Y, the skill id and the
//  owner GUID into gSpellSelPanel's arrays at gSpellSelPanel.nCount, the text colour into nLastTextColor, and the count incremented. bHoverTextPass
//  1 draws the charges tooltip when the skill has a quantity (only the right panel can see it; the left never asks) and otherwise the
//  finishing-move tooltip, whose hit test is written out. Both passes then store the row's Y into gnSpellSelRowY for rows 1..4, through a four-case
//  jump table; nothing reads those four dwords.
//  The original inlines SPELLSEL_GetSkillIconState and DATATBLS_GetSkillsTxtRecordById here. The finishing-move tooltip's hit test is written out
//  by hand rather than called: its box top is the row's local nRowTop, set once per row and decremented beside nYPos (always equal to nY - 48), not
//  nY - 48.
void __fastcall SPELLSEL_DrawPanelSkills(D2UnitStrc *pPlayer, int32_t nStartX, int32_t bGrowRight, int32_t bHoverTextPass, int32_t bSkipIconDraw)
{
    int32_t nQuantity;
    int32_t nYPos;
    int32_t nColumnStep;
    int32_t nRowStartX;
    int32_t nMaxRowSeen;
    int32_t iRow;
    uint8_t abPoolUsed[256];
    int32_t nXPos;
    int32_t nRowTop;
    D2SkillStrc *pSkill;
    D2SkillStrc *pNextSkill;
    D2SkillsTxt *pSkillsTxt;
    int16_t *pnSkillId;
    int32_t nSkillId;
    D2SkillsTxt *pSkillsTxtById;
    int32_t wSkillDesc;
    D2SkillDescTxt *pSkillDescTxt;
    D2UnitGUID nOwnerGUID;
    int32_t nCharges;
    BOOL bItemSkill;
    int32_t nTextColor;
    int32_t nListPool;

    nQuantity = 0;
    nYPos = gnScreenHeight - 86;
    nColumnStep = bGrowRight ? 48 : -48;
    nRowStartX = nStartX;
    nMaxRowSeen = 0;
    iRow = 0;
    memset(abPoolUsed, 0, sizeof(abPoolUsed));

    do
    {
        nXPos = nRowStartX;
        pSkill = D2COMMON_UNITS_GetStartSkill(pPlayer);
        if (pSkill)
        {
            nRowTop = nYPos - 48;
            do
            {
                pNextSkill = D2COMMON_SKILLS_GetNextSkill(pSkill);
                pSkillsTxt = D2COMMON_SKILLS_GetSkillsTxtRecordFromSkill(pSkill);
                if (!pSkillsTxt)
                    goto NextSkill;
                pnSkillId = &pSkillsTxt->nSkillId;

                nSkillId = pSkillsTxt->nSkillId;
                pSkillsTxtById = DATATBLS_GetSkillsTxtRecordById(nSkillId);
                if (!pSkillsTxtById)
                    goto NextSkill;
                wSkillDesc = (int16_t)pSkillsTxtById->wSkillDesc;
                if (wSkillDesc < 0 || wSkillDesc >= sgptDataTables->nSkillDescTxtRecordCount)
                    goto NextSkill;
                pSkillDescTxt = &sgptDataTables->pSkillDescTxt[wSkillDesc];
                if (!pSkillDescTxt)
                    goto NextSkill;

                //  The flags are read from the record the skill carries, not from the one looked up again by its id.
                if (bGrowRight && !(((uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_LEFTSKILL >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_LEFTSKILL & 7]))
                    goto NextSkill;
                if (((uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_PASSIVE >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_PASSIVE & 7])
                    goto NextSkill;
                if (D2COMMON_SKILLS_GetFlags(pSkill) & 8)
                    goto NextSkill;

                nOwnerGUID = (D2UnitGUID)-1;
                nCharges = 0;
                bItemSkill = D2COMMON_SKILLS_GetSkillInfo(pSkill, &nOwnerGUID, NULL, NULL, &nCharges);
                if (!bGrowRight && !bItemSkill)
                {
                    nQuantity = D2COMMON_SKILLS_GetQuantity(pSkill);
                    if (nQuantity > 0 && (((uint8_t *)pSkillsTxt->dwFlags)[4 + (SKILLSFLAGINDEX2_SCROLL >> 3)] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX2_SCROLL & 7]))
                        bItemSkill = TRUE;
                }

                if (iRow >= 4)
                {
                    if (!bItemSkill)
                        goto NextSkill;
                }
                else if (bItemSkill || iRow != (int8_t)pSkillDescTxt->nListRow)
                {
                    goto NextSkill;
                }

                nTextColor = SPELLSEL_GetSkillIconState(pPlayer, pSkill);

                if (nMaxRowSeen < iRow)
                {
                    nMaxRowSeen = iRow;
                    nYPos -= 48;
                    nRowTop -= 48;
                }

                if (pSkillDescTxt->nListPool)
                {
                    nListPool = pSkillDescTxt->nListPool;
                    if (nListPool < 256)
                    {
                        if (abPoolUsed[nListPool])
                            goto NextSkill;
                        abPoolUsed[nListPool] = 1;
                    }
                }

                if (bHoverTextPass)
                {
                    if (nQuantity)
                    {
                        SPELLSEL_DrawSkillChargesTooltip(pnSkillId, nTextColor, nQuantity, nXPos, nYPos);
                    }
                    else
                    {
                        int32_t nHoverCharges = nCharges;
                        int32_t nSkillLevel = D2COMMON_SKILLS_GetSkillLevel(pPlayer, pSkill, 1);
                        int32_t nMouseX;
                        int32_t nMouseY;
                        int32_t nDrawColor;

                        nMouseX = CURSOR_GetMouseX();
                        nMouseY = CURSOR_GetMouseY();
                        if (nMouseX < nXPos || nMouseX > nXPos + 48 || nMouseY < nRowTop || nMouseY > nYPos)
                            nDrawColor = 1;
                        else
                            nDrawColor = nTextColor == 1;

                        nMouseX = CURSOR_GetMouseX();
                        nMouseY = CURSOR_GetMouseY();
                        if (nMouseX >= nXPos && nMouseX <= nXPos + 48 && nMouseY >= nRowTop && nMouseY <= nYPos)
                        {
                            int32_t nHoverSkillId = *pnSkillId;
                            D2UnitStrc *pHoverPlayer = CUNIT_GetPlayer();

                            SKILLDESC_SkillsBuildFinishingMoveText(pHoverPlayer, nHoverSkillId, nSkillLevel, nHoverCharges, nDrawColor, nXPos, nYPos);
                        }
                    }
                    nXPos += nColumnStep;
                }
                else
                {
                    uint8_t nCount;

                    if (!bSkipIconDraw)
                        SPELLSEL_DrawSelectorSkillIcon(pnSkillId, nOwnerGUID, nCharges, nTextColor, nXPos, nYPos, bGrowRight);

                    //  BUG: the count is a byte and the four arrays have sixty entries, and nothing here compares the two. Reproduced.
                    gSpellSelPanel.nX[gSpellSelPanel.nCount] = nXPos;
                    gSpellSelPanel.nY[gSpellSelPanel.nCount] = nYPos;
                    gSpellSelPanel.nSkillId[gSpellSelPanel.nCount] = D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szAssertFile, 573);
                    D2COMMON_SKILLS_GetSkillInfo(pSkill, &gSpellSelPanel.OwnerGUID[gSpellSelPanel.nCount], NULL, NULL, NULL);

                    //  The original loads the count's low byte, stores the colour as a DWORD one byte past it (nLastTextColor), then writes the
                    //  byte back incremented.
                    nCount = gSpellSelPanel.nCount;
                    gSpellSelPanel.nLastTextColor = nTextColor;
                    gSpellSelPanel.nCount = (uint8_t)(nCount + 1);

                    nXPos = bGrowRight ? nXPos + 48 : nXPos - 48;
                }

                switch (iRow)
                {
                case 1:
                    gnSpellSelRowY[0] = nYPos;
                    break;
                case 2:
                    gnSpellSelRowY[1] = nYPos;
                    break;
                case 3:
                    gnSpellSelRowY[2] = nYPos;
                    break;
                case 4:
                    gnSpellSelRowY[3] = nYPos;
                    break;
                }

            NextSkill:
                pSkill = pNextSkill;
                nQuantity = 0;
            } while (pSkill);
        }
        ++iRow;
    } while (iRow < 5);
}

//1.10f: D2Client.0x6FB19B60
//  Looks for a skill with this id and a quantity above zero on the unit's skill list and leaves the walk when it finds one. The name says it: the
//  result is always 0. SKILLDESC_BuildDescLine is the one caller: descline 0x35 formats the value with tbl 4184 `scrollbooktext`, and 0x36..0x38
//  hand it to SKILLDESC_DrawQuantityHint. The intent reads like `return SKILLS_GetQuantity(pSkill);`.
int32_t __fastcall SPELLSEL_SkillDescGetSkillQuantity_Always0(D2UnitStrc *pUnit, int32_t nSkillId)
{
    D2SkillStrc *pSkill;
    D2SkillsTxt *pSkillsTxt;

    if (pUnit)
    {
        for (pSkill = D2COMMON_UNITS_GetStartSkill(pUnit); pSkill; pSkill = D2COMMON_SKILLS_GetNextSkill(pSkill))
        {
            pSkillsTxt = D2COMMON_SKILLS_GetSkillsTxtRecordFromSkill(pSkill);
            if (pSkillsTxt && D2COMMON_SKILLS_GetQuantity(pSkill) > 0 && pSkillsTxt->nSkillId == nSkillId)
            {
                break;
            }
        }
    }

    //  BUG: the skill the walk found is dropped and 0 comes back on every path. Reproduced.
    return 0;
}

//1.10f: D2Client.0x6FB19BB0
//  Handles a click on a button of the skill-select mini panel (UI_MINISKILL, window 3). The panel is four parallel arrays of sixty entries
//  (gSpellSelPanel's nX, nY, nSkillId and OwnerGUID); the original walks the owner GUIDs by one pointer, reaches the other three by negative
//  offsets, and bounds the loop by the address of the next global, so it visits all sixty entries. Written as `i < 60`.
//  The hit box is 48 x 48, anchored bottom-left: x >= nButtonX && x <= nButtonX + 48, y >= nButtonY - 48 && y <= nButtonY. The mouse is fetched
//  inside the loop (GetMouseX and GetMouseY once per iteration, sixty times), while a third reading taken once before the loop is what the final
//  test uses.
//  The command is MSG_SendCmdUnit with command 60 and the skill id with bit 31 set when gnMiniSkillMode is not 0 (`neg` / `sbb` / `and 80000000h`);
//  the mode is then cleared to 0 on the same path. The compiler loads the skill id into ecx and then writes only cl, so the register carries the
//  skill id's upper 24 bits into the call; MSG_SendCmdUnit is `int __fastcall(char nCmd, ...)` and reads cl alone, so they are unobservable.
//  Written as `60`.
//  A button whose skill has an owner and reports zero charges left jumps straight out of the loop to the panel-closing tail: no command is sent, no
//  further button is examined. Every other outcome sends the command.
void __stdcall SPELLSEL_MiniSkillOnLButtonDown(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nMouseYOnEntry;
    int32_t bHit;
    int32_t i;
    int32_t nCharges;

    if (!CUNIT_GetPlayer())
    {
        FOG_DisplayAssert(szAssertHUnit, szAssertFile, 954);
        exit(-1);
    }

    nMouseYOnEntry = CURSOR_GetMouseY();
    bHit = 0;

    for (i = 0; i < 60; ++i)
    {
        int32_t nButtonY = gSpellSelPanel.nY[i];
        int32_t nButtonX = gSpellSelPanel.nX[i];
        int32_t nMouseX = CURSOR_GetMouseX();
        int32_t nMouseY = CURSOR_GetMouseY();

        if (nMouseX >= nButtonX && nMouseX <= nButtonX + 48 && nMouseY >= nButtonY - 48 && nMouseY <= nButtonY)
        {
            D2UnitGUID nOwnerGUID = gSpellSelPanel.OwnerGUID[i];
            D2UnitStrc *pPlayer = CUNIT_GetPlayer();
            D2SkillStrc *pSkill = D2COMMON_SKILLS_GetSkillById(pPlayer, (D2C_Skills)gSpellSelPanel.nSkillId[i], nOwnerGUID);

            if (D2COMMON_SKILLS_GetOwnerGUIDFromSkill(pSkill) != -1 && D2COMMON_SKILLS_GetSkillInfo(pSkill, NULL, NULL, NULL, &nCharges) && nCharges <= 0)
            {
                goto CLOSE_PANEL;
            }

            MSG_SendCmdUnit(60, gSpellSelPanel.nSkillId[i] + (gnMiniSkillMode != 0 ? (int32_t)0x80000000 : 0), gSpellSelPanel.OwnerGUID[i]);
            UI_SetUIToggle(UI_MINISKILL, UI_TURNOFF, 0);
            gnMiniSkillMode = 0;
            bHit = 1;
            GAME_ResetClickState();
        }
    }

    //  A click in the bottom 49 pixels counts as handled even when it hit no button, which stops a click on the panel background falling through to
    //  the game world.
    if (nMouseYOnEntry > gnScreenHeight - 49 || bHit)
        goto BREAK_CHAIN;

CLOSE_PANEL:
    if (UI_GetUiWindowState(UI_MINISKILL))
        UI_SetUIToggle(UI_MINISKILL, UI_TURNOFF, 0);

BREAK_CHAIN:
    pMsg->lResult = 0;
    pMsg->bReturn = 1;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FB19D30
//  Selects the previous or next skill over the hotkey ring: from gnSpellSelHotkeySlot, steps through the sixteen hotkey slots in the direction of
//  nDirection's sign, counting |nDirection| usable slots, and selects the last one counted. CMDTBL_ActionSelectPreviousSkill (-1) and
//  CMDTBL_ActionSelectNextSkill (1) call it, commands 39 and 40.
//  The direction is `nDirection > 0 ? 1 : -1` (branchless in the original); the magnitude is kept apart and the function does nothing for 0. The
//  scan wraps explicitly (below 0 to 15, from 16 to 0, before each test) and gives up after sixteen tries. A slot counts when it holds a skill, its
//  mode is 0 (so a slot bound in the other mode, or cleared to -1, is passed over), the player has that skill, and it is usable: no owner, or
//  SKILLS_GetSkillInfo failing, or charges above zero.
//  The original inlines SPELLSEL_SkillsSelectHotkeySlot as the tail on nFoundSlot; ours calls it. The tail repeats that function's body: the
//  empty-slot test the scan has already passed, CUNIT_GetPlayer, the skill looked up again, the usability test again, the player fetched a second
//  time, the slot stored, command 60 with bit 31 from the mode, and SetLeftActiveSkill or SetRightActiveSkill on the re-read mode. Its left-hand
//  branch cannot be reached from here, since the scan only ever finds a mode-0 slot.
void __fastcall SPELLSEL_CycleSkill(int32_t nDirection)
{
    int32_t nRemainingSteps;
    int32_t nStep;
    int32_t nFoundSlot;
    int32_t nTries;
    int32_t nSlot;
    int32_t nCharges;

    nRemainingSteps = nDirection;
    if (CUNIT_IsPlayerDeadOrMissing())
        return;

    nStep = nDirection > 0 ? 1 : -1;
    if (nDirection < 0)
        nRemainingSteps = -nDirection;
    if (!nRemainingSteps)
        return;

    nFoundSlot = -1;
    nTries = 0;
    nSlot = gnSpellSelHotkeySlot + nStep;
    do
    {
        if (nSlot < 0)
            nSlot = 15;
        else if (nSlot >= 16)
            nSlot = 0;

        if (gnSpellSelSkillIds[nSlot] != -1 && !gnSpellSelHotkeyMode[nSlot])
        {
            D2UnitStrc *pPlayer = CUNIT_GetPlayer();

            if (pPlayer)
            {
                D2SkillStrc *pSkill = D2COMMON_SKILLS_GetSkillById(pPlayer, (D2C_Skills)gnSpellSelSkillIds[nSlot], gnSpellSelOwnerGUIDs[nSlot]);

                if (pSkill && (D2COMMON_SKILLS_GetOwnerGUIDFromSkill(pSkill) == -1 || !D2COMMON_SKILLS_GetSkillInfo(pSkill, NULL, NULL, NULL, &nCharges) || nCharges > 0))
                {
                    nFoundSlot = nSlot;
                    if (!--nRemainingSteps)
                        break;
                }
            }
        }

        nSlot += nStep;
        ++nTries;
    } while (nTries < 16);

    if (nFoundSlot != -1)
        SPELLSEL_SkillsSelectHotkeySlot(nFoundSlot);
}

//1.10f: D2Client.0x6FB19EF0
//  Unbinds the hotkeys of a scroll or tome that has run out. INV_OnGridLeftClick calls it on the branch that puts a scroll into a tome, just before
//  command 41. void: the one caller discards the result.
//  The skills it considers are those whose skills.txt second flag word has bit 4 (SKILLSFLAGINDEX2_SCROLL, asserted above); in the stock skills.txt
//  that column is 1 for exactly four skills: Scroll of Identify (217), Book of Identify (218), Scroll of Townportal (219) and Book of Townportal
//  (220). So the quantity it tests is the scroll or tome count, not an item's charges.
//  For each such skill, every entry of gSpellSelPanel.nSkillId holding its id (sixty; the bound is an address in the original, as in
//  SPELLSEL_MiniSkillOnLButtonDown) and then every hotkey slot holding the same id is cleared to -1 in all three arrays, and the server told with
//  command 81, `(nSlot << 16) + 0xFFFF`, when the quantity is at or below zero. The quantity test sits inside the inner loop. Only the skill id is
//  compared, not the owner.
//  The panel is consulted, not the hotkeys alone: a slot is cleared only if its skill is among the ids the skill-select popup registered the last
//  time it was drawn (SPELLSEL_DrawBothPanels clears them to 0 every frame it draws, and SPELLSEL_Init at startup).
//  There is no NULL test on the skills.txt record; the assert on an empty skill list is at source line 1143.
void __cdecl SPELLSEL_UnbindEmptyScrollHotkeys()
{
    D2SkillStrc *pSkill;
    D2SkillsTxt *pSkillsTxt;
    int32_t nQuantity;
    int32_t i;
    int32_t nSlot;

    pSkill = D2COMMON_UNITS_GetStartSkill(CUNIT_GetPlayer());
    if (!pSkill)
    {
        FOG_DisplayAssert(szAssertHSkill, szAssertFile, 1143);
        exit(-1);
    }

    do
    {
        pSkillsTxt = D2COMMON_SKILLS_GetSkillsTxtRecordFromSkill(pSkill);
        if (((uint8_t *)pSkillsTxt->dwFlags)[4 + (SKILLSFLAGINDEX2_SCROLL >> 3)] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX2_SCROLL & 7])
        {
            nQuantity = D2COMMON_SKILLS_GetQuantity(pSkill);
            for (i = 0; i < 60; ++i)
            {
                if (gSpellSelPanel.nSkillId[i] != pSkillsTxt->nSkillId)
                    continue;

                for (nSlot = 0; nSlot < 16; ++nSlot)
                {
                    if (gnSpellSelSkillIds[nSlot] == gSpellSelPanel.nSkillId[i] && nQuantity <= 0)
                    {
                        gnSpellSelSkillIds[nSlot] = -1;
                        gnSpellSelHotkeyMode[nSlot] = -1;
                        gnSpellSelOwnerGUIDs[nSlot] = -1;
                        MSG_SendCmdUnit(81, (nSlot << 16) + 0xFFFF, -1);
                    }
                }
            }
        }
        pSkill = D2COMMON_SKILLS_GetNextSkill(pSkill);
    } while (pSkill);
}

//1.10f: D2Client.0x6FB19FE0
//  Binds the skill under the cursor to slot nSlot when a skill hotkey is pressed while the skill-select popup is open.
//  CMDTBL_ActionSelectSkillSlot0..15 take this arm when UI_MINISKILL is open and SPELLSEL_SkillsSelectHotkeySlot otherwise.
//  Every one of the sixty registered records is hit-tested, with a fresh GetMouseX/GetMouseY pair each, in the 48 x 48 box anchored bottom-left;
//  the walk is bounded by an address in the original and written as the count, as in SPELLSEL_MiniSkillOnLButtonDown. A hit does not end the walk.
//  On a hit:
//      the slot held a skill   command 81, `(nSlot << 16) + 0xFFFF`: clear it
//      the slot takes the record's skill id and owner, and the mode 1 when gnMiniSkillMode is set and 0 when it is not (two stores on two arms)
//      command 81              `(nSlot << 16) + nSkillId + (mode ? 0x8000 : 0)`, the mode re-read from the array
//      every other slot holding the same skill and owner is cleared to -1 in all three arrays, and the server told
//  The clearing loop's counter is the packet value itself in the original (`mov edi, 0FFFFh`, `add edi, 10000h`, `cmp edi, 10FFFFh`); it is written
//  as the slot index.
void __fastcall SPELLSEL_BindHoveredSkillToSlot(int32_t nSlot)
{
    int32_t i;
    int32_t iSlot;

    for (i = 0; i < 60; ++i)
    {
        int32_t nIconY = gSpellSelPanel.nY[i];
        int32_t nIconX = gSpellSelPanel.nX[i];
        int32_t nMouseX = CURSOR_GetMouseX();
        int32_t nMouseY = CURSOR_GetMouseY();
        int32_t nSkillId;
        D2UnitGUID nOwnerGUID;

        if (nMouseX < nIconX || nMouseX > nIconX + 48 || nMouseY < nIconY - 48 || nMouseY > nIconY)
            continue;

        //  BUG: nothing tells a filled record from an empty one. SPELLSEL_DrawBothPanels clears X, Y and the skill ids to 0 and the owners to -1
        //  every frame the popup is drawn, and only the first gSpellSelPanel.nCount records are filled again, so an empty record's box is x in [0,
        //  48], y in [-48, 0]: with the cursor on the screen's top row at x <= 48 every empty record hits and binds skill 0 with owner -1 (Attack)
        //  to the slot. Reproduced.
        if (gnSpellSelSkillIds[nSlot] != -1)
            MSG_SendCmdUnit(81, (nSlot << 16) + 0xFFFF, -1);

        nSkillId = gSpellSelPanel.nSkillId[i];
        gnSpellSelSkillIds[nSlot] = nSkillId;
        if (gnMiniSkillMode)
            gnSpellSelHotkeyMode[nSlot] = 1;
        else
            gnSpellSelHotkeyMode[nSlot] = 0;
        nOwnerGUID = gSpellSelPanel.OwnerGUID[i];
        gnSpellSelOwnerGUIDs[nSlot] = nOwnerGUID;
        MSG_SendCmdUnit(81, (nSlot << 16) + nSkillId + (gnSpellSelHotkeyMode[nSlot] != 0 ? 0x8000 : 0), nOwnerGUID);

        for (iSlot = 0; iSlot < 16; ++iSlot)
        {
            if (gnSpellSelSkillIds[nSlot] == gnSpellSelSkillIds[iSlot] && gnSpellSelOwnerGUIDs[nSlot] == gnSpellSelOwnerGUIDs[iSlot] && nSlot != iSlot)
            {
                gnSpellSelSkillIds[iSlot] = -1;
                gnSpellSelHotkeyMode[iSlot] = -1;
                gnSpellSelOwnerGUIDs[iSlot] = -1;
                MSG_SendCmdUnit(81, (iSlot << 16) + 0xFFFF, -1);
            }
        }
    }
}

//1.10f: D2Client.0x6FB1A130
//  Puts the slot's skill on the mouse button its mode names, when a skill hotkey is pressed. CMDTBL_ActionSelectSkillSlot0..15 call it when the
//  popup is closed; SPELLSEL_CycleSkill's tail is a copy of it.
//  Nothing for an empty slot, a missing player or a skill the player does not have, and nothing for a skill with an owner whose charges are at or
//  below zero. Otherwise the player is fetched again, gnSpellSelHotkeySlot set, command 60 sent with bit 31 set for a mode-1 slot (`neg` / `sbb` /
//  `and edx, 80000000h`), and the skill put on the left button for a mode-1 slot, the right one otherwise. The mode is read twice, before the store
//  for the command and again for the branch.
void __fastcall SPELLSEL_SkillsSelectHotkeySlot(int32_t nSlot)
{
    int32_t *pnSlotSkillId;
    D2UnitGUID *pnSlotOwnerGUID;
    D2UnitStrc *pPlayer;
    D2SkillStrc *pSkill;
    int32_t nCharges;
    int32_t bLeftButton;

    pnSlotSkillId = &gnSpellSelSkillIds[nSlot];
    if (*pnSlotSkillId == -1)
        return;

    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
        return;

    pnSlotOwnerGUID = &gnSpellSelOwnerGUIDs[nSlot];
    pSkill = D2COMMON_SKILLS_GetSkillById(pPlayer, (D2C_Skills)*pnSlotSkillId, *pnSlotOwnerGUID);
    if (!pSkill)
        return;

    if (D2COMMON_SKILLS_GetOwnerGUIDFromSkill(pSkill) != -1 && D2COMMON_SKILLS_GetSkillInfo(pSkill, NULL, NULL, NULL, &nCharges) && nCharges <= 0)
        return;

    pPlayer = CUNIT_GetPlayer();
    bLeftButton = gnSpellSelHotkeyMode[nSlot];
    gnSpellSelHotkeySlot = nSlot;
    MSG_SendCmdUnit(60, *pnSlotSkillId + (bLeftButton != 0 ? (int32_t)0x80000000 : 0), *pnSlotOwnerGUID);
    if (gnSpellSelHotkeyMode[nSlot])
        D2COMMON_SKILLS_SetLeftActiveSkill(pPlayer, (D2C_Skills)*pnSlotSkillId, *pnSlotOwnerGUID);
    else
        D2COMMON_SKILLS_SetRightActiveSkill(pPlayer, (D2C_Skills)*pnSlotSkillId, *pnSlotOwnerGUID);
}

//1.10f: D2Client.0x6FB1A200
//  Stores one hotkey slot sent by the server: skill id, mode and owner into the three sixteen-entry arrays. SCMD_HandleSrv7B_AssignSkillHotkey
//  (packet 0x7B) is the one caller; SPELLSEL_BindHoveredSkillToSlot sends the other direction. There is no bound on nSlot. void: eax at the `retn 8`
//  holds nMode only because it was loaded to be stored, and the caller tail-returns it.
void __fastcall SPELLSEL_SetHotkeySlot(int32_t nSlot, int32_t nSkillId, int32_t nMode, D2UnitGUID nOwnerGUID)
{
    gnSpellSelSkillIds[nSlot] = nSkillId;
    gnSpellSelHotkeyMode[nSlot] = nMode;
    gnSpellSelOwnerGUIDs[nSlot] = nOwnerGUID;
}


//1.10f: D2Client.0x6FB1A220
//  The spinner control, here and in the thirteen functions after it: a two-arrow step control and the proxy that holds it. Its one instance is the
//  gold dialog's step control, which ui.cpp builds with SPELLSEL_UiSpinnerProxyConstruct(..., 223, 219, 0, UI_GoldDialogStepAmount) and keeps in
//  gpGoldDialogStepButton. The vtables, in spellsel.h, are gVtblUiSpinnerImpl (13 slots: 0 VecDelDtor, 1 Draw, 2 OnMouseWheel, 3 OnMouseDown, 4
//  OnMouseUp, 5..10 EDITBOX_DialogNullCallback, 11 GetPressedArrow, 12 TickAutoRepeat) and gVtblUiSpinnerProxy (13 slots: 0 ProxyVecDelDtor, then
//  forwarders 1..12 into the same slot of the wrapped control).
//  The gold dialog's step callback asks the spinner two things through the proxy: slot 11 (byte 44), GetPressedArrow, 1 for up and 2 for down, and
//  slot 12 (byte 48, through FwdSlot12Arg), TickAutoRepeat, how far to step.
//  How a step happens: press -- OnMouseDown arms bMouseDown, and inside an arrow stamps dwPressTick, sets the arrow's flag and calls the callback
//  with bFirstStep set, where TickAutoRepeat answers 1. Held -- Draw, every frame an arrow is set, does ++nRepeatTicks and calls the callback;
//  TickAutoRepeat answers 0 until 70 ms since the last step, then a step that grows with the hold: 1 up to 1 s, nRepeatTicks >> 5, >> 4, >> 2, and
//  past 4096 ms (hold >> 11) * nRepeatTicks. Wheel -- OnMouseWheel sets one arrow and bWheelStep; the next Draw calls the callback, and
//  TickAutoRepeat clears both arrows and the flag and answers 1, one step per notch. Release -- OnMouseUp clears the counter and the arrow.
//  The members are free functions: `__fastcall`, with nEdxUnused where the member takes a stack argument. The `fs:0` frames of Construct, Destruct,
//  ProxyDestruct and ProxyConstruct are the compiler's unwinding bookkeeping and are not reproduced, which changes no value the program computes.
//  The spinner's constructor: sets the vtable, loads the image from `DATA\GLOBAL\ui\menu\spinner` (sprintf, then ARCHIVE_CellFileLoad mode 0),
//  clears the state, measures cel 0, and places the two arrows: side by side a cel-width apart when bHorizontal, else stacked a cel-height plus one
//  apart. Both arrows start at nX + 1 in the stacked layout; horizontally the down arrow is at nX + nCelWidth, without the one. The up arrow's y is
//  nY either way. The stores follow the source shape, not the bytes' schedule: distinct members, no reader between them. bHorizontal is read back
//  from the object for the branch, and so here.
D2ClientSpinnerStrc *__fastcall SPELLSEL_UiSpinnerConstruct(D2ClientSpinnerStrc *pSpinner, int32_t nEdxUnused, int32_t nX, int32_t nY, int32_t bHorizontal, void *pfnOnChange)
{
    char szPath[260];
    D2CellFileStrc *hImage;
    D2GfxDataStrc tCelData;
    D2GfxCellStrc *pCel;

    pSpinner->pVtbl = &gVtblUiSpinnerImpl;
    sprintf(szPath, "%s\\ui\\menu\\spinner", aDataGlobal);
    hImage = ARCHIVE_CellFileLoad(szPath, 0);
    pSpinner->nX = nX;
    pSpinner->pfnOnChange = (void (__cdecl *)())pfnOnChange;
    pSpinner->bHorizontal = bHorizontal;
    pSpinner->hCelFile = hImage;
    memset(&tCelData, 0, sizeof(tCelData));
    tCelData.pCellFile = hImage;
    pSpinner->nY = nY;
    pSpinner->bUpPressed = 0;
    pSpinner->bDownPressed = 0;
    pSpinner->bMouseDown = 0;
    pSpinner->bFirstStep = 0;
    pSpinner->nRepeatTicks = 0;
    tCelData.nDirection = 0;
    tCelData.nFrame = 0;
    pCel = D2CMP_CelGetHandle(&tCelData);
    pSpinner->nCelHeight = D2CMP_CelGetHeight(pCel);
    pSpinner->nCelWidth = D2CMP_CelGetWidth(pCel);
    pSpinner->nUpY = nY;
    if (pSpinner->bHorizontal)
    {
        pSpinner->nDownY = nY;
        pSpinner->nUpX = nX + 1;
        pSpinner->nDownX = nX + pSpinner->nCelWidth;
    }
    else
    {
        pSpinner->nUpX = nX + 1;
        pSpinner->nDownY = pSpinner->nCelHeight + nY + 1;
        pSpinner->nDownX = nX + 1;
    }
    return pSpinner;
}

//1.10f: D2Client.0x6FB1A340
//  Slot 0 of gVtblUiSpinnerImpl, the scalar-deleting destructor, the shape every control in this image has (CHAR_UiButtonVecDelDtor).
D2ClientSpinnerStrc *__fastcall SPELLSEL_UiSpinnerVecDelDtor(D2ClientSpinnerStrc *pSpinner, int32_t nEdxUnused, uint8_t bFreeObject)
{
    SPELLSEL_UiSpinnerDestruct(pSpinner);
    if ((bFreeObject & 1) != 0 && pSpinner)
    {
        STORM_SMemFree(pSpinner, szDelete, -1, 0);
    }
    return pSpinner;
}

//1.10f: D2Client.0x6FB1A370
//  The destructor: puts the class's own vtable back, unloads the arrows' image, then calls the shared base destructor (a call here, inside the
//  unwinding frame, where the button's is a tail jump).
void __fastcall SPELLSEL_UiSpinnerDestruct(D2ClientSpinnerStrc *pSpinner)
{
    pSpinner->pVtbl = &gVtblUiSpinnerImpl;
    ARCHIVE_CellFileFree(pSpinner->hCelFile);
    LISTBOX_UiCtrlBaseDestruct(pSpinner);
}

//1.10f: D2Client.0x6FB1A3D0
//  Slot 1, draw. While either arrow is set -- the down flag tested first -- it counts a repeat tick and calls the callback; then both arrows are
//  drawn from the one image, frame `2 * bUpPressed` and `2 * bDownPressed + 1`: up, down, up pressed, down pressed. The second draw reuses the
//  first one's context with only the frame changed.
void __fastcall SPELLSEL_UiSpinnerDraw(D2ClientSpinnerStrc *pSpinner)
{
    D2GfxDataStrc tCelData;

    if (pSpinner->bDownPressed || pSpinner->bUpPressed)
    {
        ++pSpinner->nRepeatTicks;
        pSpinner->pfnOnChange();
    }

    memset(&tCelData, 0, sizeof(tCelData));
    tCelData.pCellFile = pSpinner->hCelFile;
    tCelData.nFrame = 2 * pSpinner->bUpPressed;
    tCelData.nDirection = 0;
    D2GFX_TEXTURE_CelDraw(&tCelData, pSpinner->nUpX, pSpinner->nUpY, 0xFFFFFFFF, 5, NULL);
    tCelData.nFrame = 2 * pSpinner->bDownPressed + 1;
    D2GFX_TEXTURE_CelDraw(&tCelData, pSpinner->nDownX, pSpinner->nDownY, 0xFFFFFFFF, 5, NULL);
}

//1.10f: D2Client.0x6FB1A460
//  Slot 11, which arrow: 1 for up, 2 for down, 0 for neither; up wins when both are set.
int32_t __fastcall SPELLSEL_UiSpinnerGetPressedArrow(D2ClientSpinnerStrc *pSpinner)
{
    if (pSpinner->bUpPressed)
        return 1;
    return pSpinner->bDownPressed != 0 ? 2 : 0;
}

//1.10f: D2Client.0x6FB1A480
//  Slot 12, how far to step now. 0 with no arrow set. A wheel notch clears both arrows and its flag and answers 1; the press's own callback
//  (bFirstStep) stamps the step time and answers 1. Otherwise nothing until 70 ms (0x46) since the last step, and then, by how long the arrow has
//  been held, 1, or the repeat count shifted right 5, 4 or 2, or past 4096 ms (hold >> 11) times the count. Every compare is unsigned and every
//  shift logical, as DWORD tick arithmetic makes them; the step time is stored before the ladder.
int32_t __fastcall SPELLSEL_UiSpinnerTickAutoRepeat(D2ClientSpinnerStrc *pSpinner)
{
    uint32_t dwNow;
    uint32_t dwHoldTime;

    if (!pSpinner->bUpPressed && !pSpinner->bDownPressed)
        return 0;

    if (pSpinner->bWheelStep)
    {
        pSpinner->bUpPressed = 0;
        pSpinner->bDownPressed = 0;
        pSpinner->bWheelStep = 0;
        return 1;
    }

    dwNow = GetTickCount();
    if (pSpinner->bFirstStep)
    {
        pSpinner->dwLastStepTick = (int32_t)dwNow;
        return 1;
    }

    dwHoldTime = dwNow - (uint32_t)pSpinner->dwPressTick;
    if (dwNow - (uint32_t)pSpinner->dwLastStepTick < 0x46)
        return 0;

    pSpinner->dwLastStepTick = (int32_t)dwNow;
    if (dwHoldTime > 0x1000)
        return (int32_t)((dwHoldTime >> 11) * (uint32_t)pSpinner->nRepeatTicks);
    if (dwHoldTime > 0xBB8)
        return (int32_t)((uint32_t)pSpinner->nRepeatTicks >> 2);
    if (dwHoldTime > 0x7D0)
        return (int32_t)((uint32_t)pSpinner->nRepeatTicks >> 4);
    if (dwHoldTime > 0x3E8)
        return (int32_t)((uint32_t)pSpinner->nRepeatTicks >> 5);
    return 1;
}

//1.10f: D2Client.0x6FB1A530
//  Slot 2, a wheel notch: presses the up arrow for a positive delta, the down arrow for a negative one, and sets bWheelStep; a zero delta sets
//  nothing. The delta is the high word of wParam, signed. The message is consumed and the chain broken either way (the 1 in eax is the constant the
//  stores used). It does not call the callback: the next Draw does, and TickAutoRepeat turns the notch into one step.
int32_t __fastcall SPELLSEL_UiSpinnerOnMouseWheel(D2ClientSpinnerStrc *pSpinner, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg)
{
    int16_t nDelta = (int16_t)HIWORD(pMsg->wParam);

    if (nDelta > 0)
    {
        pSpinner->bUpPressed = 1;
        pSpinner->bDownPressed = 0;
        pSpinner->bWheelStep = 1;
    }
    else if (nDelta < 0)
    {
        pSpinner->bUpPressed = 0;
        pSpinner->bDownPressed = 1;
        pSpinner->bWheelStep = 1;
    }

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
    return 1;
}

//1.10f: D2Client.0x6FB1A580
//  Slot 3, a press. bMouseDown is armed first, hit or miss, and only then is the cursor, the message's two words zero-extended, tested against the
//  arrows, every compare signed:
//      up    nUpX < x < nUpX + nCelWidth,  nUpY - nCelHeight < y <= nUpY
//      down  nDownX < x < nDownX + nCelWidth,  nDownY - nCelHeight < y < nDownY
//  so the up box keeps its bottom row and the down box does not (`jg` against `jge`). Reproduced as the original has it. A hit stamps dwPressTick,
//  sets the arrow and calls the callback between bFirstStep = 1 and 0, and answers 1; a miss answers 0.
int32_t __fastcall SPELLSEL_UiSpinnerOnMouseDown(D2ClientSpinnerStrc *pSpinner, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nMouseX;
    int32_t nMouseY;

    pSpinner->bMouseDown = 1;
    nMouseX = pMsg->nXpos;
    nMouseY = pMsg->nYpos;

    if (nMouseX > pSpinner->nUpX && nMouseX < pSpinner->nUpX + pSpinner->nCelWidth && nMouseY > pSpinner->nUpY - pSpinner->nCelHeight && nMouseY <= pSpinner->nUpY)
    {
        pSpinner->dwPressTick = (int32_t)GetTickCount();
        pSpinner->bUpPressed = 1;
        pSpinner->bFirstStep = 1;
        pSpinner->pfnOnChange();
        pSpinner->bFirstStep = 0;
        return 1;
    }

    if (nMouseX > pSpinner->nDownX && nMouseX < pSpinner->nDownX + pSpinner->nCelWidth && nMouseY > pSpinner->nDownY - pSpinner->nCelHeight && nMouseY < pSpinner->nDownY)
    {
        pSpinner->dwPressTick = (int32_t)GetTickCount();
        pSpinner->bDownPressed = 1;
        pSpinner->bFirstStep = 1;
        pSpinner->pfnOnChange();
        pSpinner->bFirstStep = 0;
        return 1;
    }

    return 0;
}

//1.10f: D2Client.0x6FB1A630
//  Slot 4, a release. Nothing unless bMouseDown; then the repeat count and bMouseDown are cleared, and the up arrow if it is exactly 1, else the
//  down arrow if it is exactly 1, answering 1 for a cleared arrow and 0 when neither was set. The message is not read.
int32_t __fastcall SPELLSEL_UiSpinnerOnMouseUp(D2ClientSpinnerStrc *pSpinner, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg)
{
    if (!pSpinner->bMouseDown)
        return 0;

    pSpinner->nRepeatTicks = 0;
    pSpinner->bMouseDown = 0;
    if (pSpinner->bUpPressed == 1)
    {
        pSpinner->bUpPressed = 0;
        return 1;
    }
    if (pSpinner->bDownPressed == 1)
    {
        pSpinner->bDownPressed = 0;
        return 1;
    }
    return 0;
}

//1.10f: D2Client.0x6FB1A670
//  The proxy's destructor, as CHAR_UiButtonProxyDestruct with this class's vtable: the wrapped spinner is sent its own slot 0 with the free flag,
//  then the base destructor runs.
void __fastcall SPELLSEL_UiSpinnerProxyDestruct(D2UiCtrlProxyStrc *pProxy)
{
    D2ClientSpinnerStrc *pSpinner;

    pProxy->pSpinnerVtbl = &gVtblUiSpinnerProxy;
    pSpinner = pProxy->pSpinner;
    if (pSpinner)
    {
        ((D2ClientSpinnerStrc *(__thiscall *)(D2ClientSpinnerStrc *, uint8_t))pSpinner->pVtbl->pfnDeletingDtor)(pSpinner, 1);
    }
    LISTBOX_UiCtrlBaseDestruct(pProxy);
}

//1.10f: D2Client.0x6FB1A6D0
//  Slot 0 of gVtblUiSpinnerProxy, the proxy's scalar-deleting destructor.
D2UiCtrlProxyStrc *__fastcall SPELLSEL_UiSpinnerProxyVecDelDtor(D2UiCtrlProxyStrc *pProxy, int32_t nEdxUnused, uint8_t bFreeObject)
{
    SPELLSEL_UiSpinnerProxyDestruct(pProxy);
    if ((bFreeObject & 1) != 0 && pProxy)
    {
        STORM_SMemFree(pProxy, szDelete, -1, 0);
    }
    return pProxy;
}

//1.10f: D2Client.0x6FB1A700
//  Builds a proxy around a new spinner: 0x54 bytes from Storm, the proxy's vtable stored before the allocation returns, a null control stored when
//  it fails. UI_OpenGoldEntryDialog (ui.cpp) is the one caller.
D2UiCtrlProxyStrc *__fastcall SPELLSEL_UiSpinnerProxyConstruct(D2UiCtrlProxyStrc *pProxy, int32_t nEdxUnused, int32_t nX, int32_t nY, int32_t bHorizontal, void *pfnOnChange)
{
    D2ClientSpinnerStrc *pSpinner;
    D2ClientSpinnerStrc *pConstructed;

    pProxy->pSpinnerVtbl = &gVtblUiSpinnerProxy;
    pSpinner = (D2ClientSpinnerStrc *)STORM_SMemAlloc(0x54, szNew, -1, 0);
    if (pSpinner)
    {
        pConstructed = SPELLSEL_UiSpinnerConstruct(pSpinner, 0, nX, nY, bHorizontal, pfnOnChange);
    }
    else
    {
        pConstructed = NULL;
    }
    pProxy->pSpinner = pConstructed;
    return pProxy;
}

//1.10f: D2Client.0x6FB1A780
//  Forwards to the wrapped control's slot 3, OnMouseDown, by a tail jump (`jmp dword ptr [eax+0Ch]`): the message pointer on the stack goes through
//  untouched and the target's `retn 4` pops it. It sits at +0x0C of four proxy vtables (0x6FB6EECC the button's, 0x6FB6EF80, 0x6FB6F9F0 and
//  0x6FB6FBE0 the spinner's) and every one of their slot-3 targets is an OnMouseDown taking the message, so pMsg is passed on: a call that did not
//  would leave four bytes on the stack for the target to pop.
int32_t __fastcall SPELLSEL_UiCtrlProxyFwdSlot3(D2UiCtrlProxyStrc *pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg)
{
    return (*(int32_t (__thiscall *const *const *)(void *, SMSGHANDLER_PARAMS *))pProxy->pImpl)[3](pProxy->pImpl, pMsg);
}

//1.10f: D2Client.0x6FB1A790
//  Forwards to the wrapped control's slot 12, the spinner's TickAutoRepeat, and is the one forwarder here that is not a tail jump: `call dword ptr
//  [eax+30h]`, `retn 4`. It takes one stack argument, does not pass it, and pops it itself, because the target takes nothing (editbox.cpp's
//  FwdSlot13Arg is the same shape). The answer is the step, returned. The gold dialog calls it with 0.
int32_t __fastcall SPELLSEL_UiCtrlProxyFwdSlot12Arg(D2UiCtrlProxyStrc *pProxy, int32_t nEdxUnused, int32_t nUnused)
{
    return (*(int32_t (__thiscall *const *const *)(void *))pProxy->pImpl)[12](pProxy->pImpl);
}

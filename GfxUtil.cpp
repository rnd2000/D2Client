#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "CoreArchive.h"
#include "Gfx.h"
#include "GfxUtil.h"

//  GfxUtil.cpp -- the helpers the unit drawing in Gfx.cpp leans on, from D2Client.dll 1.10: which class a shapeshifted unit is drawn as, whose
//  inventory it is drawn wearing, the cached mode tokens, a unit's cel resolved and measured, the screen cull, which components a unit has,
//  the gfx request filled and its file-name tokens, a COF's layer bytes and layer order, a facing turned into a cel direction, a component's
//  draw mode, palette and visibility, and an overlay's frame count.
//  The original source file is Engine\GfxUtil.cpp (its asserts name the path). Address range 0x6FB5DD70..0x6FB5FB08. Three of its functions,
//  GFXUTIL_GetEffectiveClassId, GFXUTIL_GetUnitDisplayInventory and GFXUTIL_GetOverlayTxtRecord, are written in Shared.cpp.
//  Five of the unit's functions also push "..\D2Client/Engine.h": an inline of Engine.h, emitted where it is used.

//  The two path literals this unit passes: its own and the Engine.h inline's.
static const char szGfxUtilCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\Engine\\GfxUtil.cpp";
static const char szEngineH[] = "..\\D2Client/Engine.h";

//  Layout checks for the numbers this unit reads.
static_assert(offsetof(D2GfxDataStrc, pCurrentCell) == 0 && offsetof(D2GfxDataStrc, nDirection) == 0x0C
              && offsetof(D2GfxDataStrc, nComponent) == 0x1D,
              "GFXUTIL_GetUnitCelSize: `mov eax, [esp+48h+pCelFile]` 0x6FB5E003, `mov [esp+54h+var_3C], edx` 0x6FB5DFD2, "
              "`mov [esp+58h+var_2B], 1` 0x6FB5DFE1");
static_assert(offsetof(D2CompositTxt, szToken) == 0x20 && offsetof(D2PlrModeTypeTxt, szToken) == 0x20
              && offsetof(D2MonModeTxt, dwToken) == 0x20 && offsetof(D2ObjModeTypeTxt, szToken) == 0x20
              && offsetof(D2ArmTypeTxt, szToken) == 0x20,
              "GFXUTIL_CacheTokens: `mov eax, [eax+20h]` 0x6FB5DE5C, and the same +20h in the other four loops");
static_assert(NUMBER_OF_PLRMODES == 20, "GFXUTIL_CacheTokens: `cmp esi, 14h` 0x6FB5DE93");
static_assert(STATEMASK_BOSSINV == 37 && UNITFLAGEX_ISSHAPESHIFTED == 1 << 3,
              "GFXUTIL_GetUnitDisplayInventory: `push 25h` 0x6FB5DDFD, `shr eax, 3` / `and eax, 1` 0x6FB5DDF1");
static_assert(offsetof(D2MonStats2Txt, dwComponentFlags) == 0xE8 && offsetof(D2ObjectsTxt, nHD) == 0x151,
              "GFXUTIL_UnitHasComponent: `mov al, [edi+eax+0E8h]` 0x6FB5E2EF, `cmp byte ptr [eax+edi+151h], 1` 0x6FB5E314");
static_assert(COMPOSIT_HEAD == 0 && COMPOSIT_TORSO == 1 && COMPOSIT_LEGS == 2 && COMPOSIT_RIGHTARM == 3 && COMPOSIT_LEFTARM == 4
              && COMPOSIT_SPECIAL1 == 8 && COMPOSIT_SPECIAL2 == 9,
              "GFXUTIL_UnitHasComponent: the player's set, cases 0-4, 8 and 9 of jpt_6FB5E27F; the item's `cmp edi, 1` 0x6FB5E329");

//  Layout checks: the request block's identity and token fields, the file-name fields of five excel records, the COF cache node's data pointer,
//  the two getters' table fields and three enum members.
static_assert(offsetof(D2GfxDataStrc, fState) == 0x1C && offsetof(D2GfxDataStrc, nUnitType) == 0x20
              && offsetof(D2GfxDataStrc, nUnitIndex) == 0x24 && offsetof(D2GfxDataStrc, nMode) == 0x28
              && offsetof(D2GfxDataStrc, nOverlay) == 0x2C && offsetof(D2GfxDataStrc, dwName) == 0x30
              && offsetof(D2GfxDataStrc, pName) == 0x44,
              "GFXUTIL_ResolveUnitAnimIdentity: `or byte ptr [esi+1Ch], 8` 0x6FB5E66D, `mov [esi+20h], eax` 0x6FB5E3D0, "
              "`mov [esi+24h], ecx` 0x6FB5E3C6, `mov [esi+28h], ecx` 0x6FB5E3DC, `mov eax, [esi+2Ch]` 0x6FB5EC01, "
              "`mov [esi+30h], eax` 0x6FB5E511, `mov [esi+44h], eax` 0x6FB5EC35");
static_assert(offsetof(D2ItemsTxt, szFlippyFile) == 0 && offsetof(D2ItemsTxt, szInvFile) == 0x20
              && offsetof(D2ItemsTxt, szUniqueInvFile) == 0x40 && offsetof(D2ItemsTxt, szSetInvFile) == 0x60
              && offsetof(D2ItemsTxt, dwAlternateGfx) == 0x90 && offsetof(D2ItemsTxt, nArmorComp) == 0x116,
              "GFXUTIL_ResolveUnitAnimIdentity: `mov [esi+44h], ebx` 0x6FB5EB4F, `add ebx, 20h` 0x6FB5EB34, "
              "`lea eax, [ebx+40h]` 0x6FB5EAE5, `lea eax, [ebx+60h]` 0x6FB5EA65, `mov ecx, [eax+90h]` 0x6FB5E7D3, "
              "`lea ebp, [eax+116h]` 0x6FB5E6CF");
static_assert(offsetof(D2SetItemsTxt, szFlippyFile) == 0x42 && offsetof(D2SetItemsTxt, szInvFile) == 0x62
              && offsetof(D2UniqueItemsTxt, szFlippyFile) == 0x3A && offsetof(D2UniqueItemsTxt, szInvFile) == 0x5A
              && offsetof(D2MissilesTxt, szCelFile) == 0x138 && offsetof(D2OverlayTxt, szFilename) == 2,
              "GFXUTIL_ResolveUnitAnimIdentity: `add eax, 42h` 0x6FB5EB97, `mov cl, [eax+62h]` 0x6FB5EA3D, "
              "`add eax, 3Ah` 0x6FB5EB77, `mov cl, [eax+5Ah]` 0x6FB5EABD, `add eax, 138h` 0x6FB5EBE7, `add eax, 2` 0x6FB5EC32");
static_assert(offsetof(D2ClientCofCacheNodeStrc, pData) == 0x14 && offsetof(D2DataTablesStrc, pOverlayTxt) == 0xBBC
              && offsetof(D2DataTablesStrc, pUniqueItemsTxt) == 0xC24
              && offsetof(D2DataTablesStrc, nUniqueItemsTxtRecordCount) == 0xC28,
              "GFXUTIL_FillCelContextTokens: `mov eax, [ecx+14h]` 0x6FB5ED5F; GFXUTIL_GetOverlayTxtRecord +0BBCh "
              "0x6FB5FAC3; GFXUTIL_GetUniqueItemsTxtRecord +0C24h 0x6FB5FAFC, +0C28h 0x6FB5FAEB");
static_assert(MONSTATS2FLAGINDEX_COMPOSITEDEATH == 16 && COMPOSIT_RIGHTHAND == 5 && COMPOSIT_SHIELD == 7,
              "GFXUTIL_ResolveUnitAnimIdentity: byte +6 against gdwBitMasks[0] 0x6FB5E646, `cmp ebx, 5` 0x6FB5E7A9, "
              "`cmp ebx, 7` 0x6FB5E7FC");

//  A four-character token as this unit stores and compares it: one dword whose bytes, in memory order, spell it. "lit " is 0x2074696C.
static constexpr uint32_t GFXUTIL_Token(char a, char b, char c, char d)
{
    return (uint32_t)(uint8_t)a | (uint32_t)(uint8_t)b << 8 | (uint32_t)(uint8_t)c << 16 | (uint32_t)(uint8_t)d << 24;
}

static_assert(GFXUTIL_Token('l', 'i', 't', ' ') == 0x2074696C && GFXUTIL_Token('x', 'x', 'x', ' ') == 0x20787878
              && GFXUTIL_Token('x', 'b', 'w', ' ') == 0x20776278 && GFXUTIL_Token('b', 'o', 'w', ' ') == 0x20776F62
              && GFXUTIL_Token('h', 's', 'h', ' ') == 0x20687368,
              "GFXUTIL_ResolveUnitAnimIdentity: `mov dword ptr [esi+38h], 2074696Ch` 0x6FB5E4AF, `mov dword ptr "
              "[esi+3Ch], 20787878h` 0x6FB5E8ED, `cmp eax, 20776278h` 0x6FB5E7A2, `cmp eax, 20776F62h` 0x6FB5E7E9, "
              "`mov eax, 20687368h` 0x6FB5E819");

//  Layout checks: the three direction counts, the word array of the unit gfx block the loaded-mode flags live in, the default draw mode and the
//  path's 64 directions.
static_assert(offsetof(D2MonStats2Txt, nModeDirections) == 0xF4 && offsetof(D2MissilesTxt, nNumDirections) == 0x1A0
              && offsetof(D2OverlayTxt, nNumDirections) == 0x80,
              "GFXUTIL_ResolveAnimDirection: `mov dl, [eax+edi+0F4h]` 0x6FB5F16A, `mov cl, [eax+1A0h]` 0x6FB5F1C3, "
              "`mov dl, [eax+80h]` 0x6FB5F1E3");
static_assert(offsetof(D2ClientUnitGfxInfoStrc, wModeLoaded) == 0 && sizeof(((D2ClientUnitGfxInfoStrc *)0)->wModeLoaded[0]) == 2,
              "GFXUTIL_IsUnitModeLoaded: `mov al, [eax+esi*2]` 0x6FB5F060; GFXUTIL_SetUnitModeLoaded: `mov [eax+esi*2], cl` 0x6FB5F080");
static_assert(DRAWMODE_NORMAL == 5, "GFXUTIL_GetCofLayerTransLvl: `mov eax, 5` 0x6FB5F01F");
static_assert(PATH_NB_DIRECTIONS == 0x40, "GFXUTIL_ResolveAnimDirection: `cmp dword ptr [ebp+0], 40h` 0x6FB5F09A");

//  Layout checks: the items.txt transparency pair, the overlay entry's two fields and the fade stat.
static_assert(offsetof(D2ItemsTxt, nTransparent) == 0x12C && offsetof(D2ItemsTxt, nTransTbl) == 0x12D,
              "GFXUTIL_ResolveUnitTransLevel: `mov cl, [eax+12Ch]` 0x6FB5F55A, `mov cl, [eax+12Dh]` 0x6FB5F56A");
static_assert(offsetof(D2ClientGfxEntryStrc, nOverlayId) == 4 && offsetof(D2ClientGfxEntryStrc, nFrame) == 0x18,
              "GFXUTIL_GetOverlayFrameCount: `mov eax, [edx+4]` 0x6FB5F9E4, `mov edi, [edx+18h]` 0x6FB5F9E7");
static_assert(STAT_FADE == 0xB5, "GFXUTIL_ResolveUnitTransLevel: `push 0B5h` 0x6FB5F49D, 0x6FB5F520");

//  The assert text GFXUTIL_MeasureLayersBottomY pushes.
static const char szComponentAssert[] = "eComponent < NUM_COMPONENTS";


//1.10f: D2Client.0x6FB5DE40
//  Caches the mode tokens. Five tables, five loops: each zeroes its array, then copies the first four characters of the token of each of its
//  first N rows (the dword at +20h, right after the 32-byte name) into it; a NULL row is fatal, "ptToken" asserted at the line below, exit(-1):
//      gaCompositTokens  16  composit.txt   DATATBLS_GetCompositTxtRecord(i)        line 54
//      gaPlrModeTokens   20  plrmode.txt    DATATBLS_GetPlrModeTypeTxtRecord(i, 1)  line 63
//      gaMonModeTokens   16  monmode.txt    DATATBLS_GetMonModeTxtRecord(i, 1)      line 72
//      gaObjModeTokens    8  objmode.txt    DATATBLS_GetObjModeTypeTxtRecord(i, 1)  line 81
//      gaArmTypeTokens    3  armtype.txt    DATATBLS_GetArmTypeTxtRecord(i)         line 90
//  Every counter is compared unsigned. The first four arrays are cleared with rep stosd and the three-dword one with three stores, which is what
//  the compiler makes of a 12-byte clear; written as a memset like the others. 16 is NUM_COMPONENTS and 20 NUMBER_OF_PLRMODES; 16 for the monster
//  modes is NUMBER_OF_MONMODES; the 8 object modes and the 3 armor types have no named constant.
//  Its one entry is a tail jmp from GFX_InitCelCache.
void __cdecl GFXUTIL_CacheTokens(void)
{
    uint32_t nIndex;
    D2CompositTxt *pCompositTxt;
    D2PlrModeTypeTxt *pPlrModeTxt;
    D2MonModeTxt *pMonModeTxt;
    D2ObjModeTypeTxt *pObjModeTxt;
    D2ArmTypeTxt *pArmTypeTxt;

    memset(gaCompositTokens, 0, sizeof(gaCompositTokens));
    nIndex = 0;
    do
    {
        pCompositTxt = D2COMMON_DATATBLS_GetCompositTxtRecord((int32_t)nIndex);
        if (!pCompositTxt)
        {
            FOG_DisplayAssert("ptToken", szGfxUtilCpp, 54);
            exit(-1);
        }

        gaCompositTokens[nIndex++] = *(int32_t *)pCompositTxt->szToken;
    }
    while (nIndex < NUM_COMPONENTS);

    memset(gaPlrModeTokens, 0, sizeof(gaPlrModeTokens));
    nIndex = 0;
    do
    {
        pPlrModeTxt = D2COMMON_DATATBLS_GetPlrModeTypeTxtRecord((int32_t)nIndex, TRUE);
        if (!pPlrModeTxt)
        {
            FOG_DisplayAssert("ptToken", szGfxUtilCpp, 63);
            exit(-1);
        }

        gaPlrModeTokens[nIndex++] = *(int32_t *)pPlrModeTxt->szToken;
    }
    while (nIndex < NUMBER_OF_PLRMODES);

    memset(gaMonModeTokens, 0, sizeof(gaMonModeTokens));
    nIndex = 0;
    do
    {
        pMonModeTxt = D2COMMON_DATATBLS_GetMonModeTxtRecord((int32_t)nIndex, TRUE);
        if (!pMonModeTxt)
        {
            FOG_DisplayAssert("ptToken", szGfxUtilCpp, 72);
            exit(-1);
        }

        gaMonModeTokens[nIndex++] = (int32_t)pMonModeTxt->dwToken;
    }
    while (nIndex < NUMBER_OF_MONMODES);

    memset(gaObjModeTokens, 0, sizeof(gaObjModeTokens));
    nIndex = 0;
    do
    {
        pObjModeTxt = D2COMMON_DATATBLS_GetObjModeTypeTxtRecord((int32_t)nIndex, TRUE);
        if (!pObjModeTxt)
        {
            FOG_DisplayAssert("ptToken", szGfxUtilCpp, 81);
            exit(-1);
        }

        gaObjModeTokens[nIndex++] = *(int32_t *)pObjModeTxt->szToken;
    }
    while (nIndex < 8);

    memset(gaArmTypeTokens, 0, sizeof(gaArmTypeTokens));
    nIndex = 0;
    do
    {
        pArmTypeTxt = D2COMMON_DATATBLS_GetArmTypeTxtRecord((int32_t)nIndex);
        if (!pArmTypeTxt)
        {
            FOG_DisplayAssert("ptToken", szGfxUtilCpp, 90);
            exit(-1);
        }

        gaArmTypeTokens[nIndex++] = *(int32_t *)pArmTypeTxt->szToken;
    }
    while (nIndex < 3);
}


//1.10f: D2Client.0x6FB5DFB0
//  Measures a unit's cel: a request block for component 1 at nDirection and nFrame, filled and resolved as GFXUTIL_FillGfxData does it, with the
//  unit's identity from GFXUTIL_ResolveUnitAnimIdentity (mode override -1, none), D2CMP's SpriteValidate (no async wait, async load), then the
//  current cel's width and height through the two out-parameters. Any refusal on the way leaves both untouched. retn 0Ch: nFrame, pnWidth and
//  pnHeight on the stack. void: no caller reads eax. The resolver's pnClass is the address of the nFrame argument; it only writes *pnClass and
//  nothing reads it after, so a local is the same program. The original inlines GFXUTIL_FillGfxData here.
void __fastcall GFXUTIL_GetUnitCelSize(D2UnitStrc *pUnit, int32_t nDirection, int32_t nFrame, int32_t *pnWidth, int32_t *pnHeight)
{
    D2GfxDataStrc tGfxData;
    int32_t nClass;

    memset(&tGfxData, 0, sizeof(tGfxData));
    if (!GFXUTIL_FillGfxData(&tGfxData, pUnit, NULL, nDirection, nFrame, &nClass, 1, -1))
        return;

    if (!D2CMP_SPRITECACHE_SpriteValidate(&tGfxData, FALSE, TRUE))
        return;

    if (!tGfxData.pCurrentCell)
        return;

    *pnWidth = D2CMP_CELCMP_CelGetWidth(tGfxData.pCurrentCell);
    *pnHeight = D2CMP_CELCMP_CelGetHeight(tGfxData.pCurrentCell);
}


//1.10f: D2Client.0x6FB5E030
//  Would the unit's cel be on screen at nX, nY? The point made relative to the viewport origin, GFX_IsUnitCofOnScreen's test first, then the unit's
//  own cel resolved as GFXUTIL_GetUnitCelSize resolves it (the shifted frame, D2Common's direction as a byte, component 1) and the cel's box tested
//  the way GFXUTIL_IsCelVisible tests one: left edge past the screen's width, right edge before 0, the anchor less the height past the screen's
//  height, the anchor plus the height before 0. Every refusal before the box is FALSE. All four compares are signed. retn 4: nY on the stack.
//  The vertical test uses the height on both sides of the anchor, so the band is twice the cel's height; it errs toward "on screen".
//  The original inlines GFXUTIL_FillGfxData here.
BOOL __fastcall GFXUTIL_IsUnitCelOnScreen(D2UnitStrc *pUnit, int32_t nX, int32_t nY)
{
    int32_t nViewX;
    int32_t nViewY;
    int32_t nFrameNo;
    int32_t nFrameCount;
    int32_t nClass;
    D2GfxDataStrc tGfxData;
    D2GfxCellStrc *pCel;
    int32_t nCelHeight;
    int32_t nCelBottom;
    int32_t nCelLeft;
    int32_t nWidth;

    nViewX = nX - SCROLL_GetViewportOriginX();
    nViewY = nY - SCROLL_GetViewportOriginY();
    if (!GFX_IsUnitCofOnScreen(pUnit, nViewX, nViewY, 0, FALSE))
        return FALSE;

    D2COMMON_UNITS_GetShiftedFrameMetrics(pUnit, &nFrameNo, &nFrameCount);
    memset(&tGfxData, 0, sizeof(tGfxData));
    if (!GFXUTIL_FillGfxData(&tGfxData, pUnit, NULL, D2COMMON_UNITS_GetDirection(pUnit), nFrameNo, &nClass, 1, -1))
        return FALSE;

    if (!D2CMP_SPRITECACHE_SpriteValidate(&tGfxData, FALSE, TRUE))
        return FALSE;

    pCel = tGfxData.pCurrentCell;
    if (!pCel)
        return FALSE;

    nCelHeight = D2CMP_CELCMP_CelGetHeight(pCel);
    nCelBottom = nViewY + D2CMP_CELCMP_CelGetOffsetY(pCel);
    nCelLeft = nViewX + D2CMP_CELCMP_CelGetOffsetX(pCel);
    nWidth = D2CMP_CELCMP_CelGetWidth(pCel);
    if (nCelLeft > gnScreenWidth)
        return FALSE;

    if (nCelLeft + nWidth < 0)
        return FALSE;

    if (nCelBottom - nCelHeight > gnScreenHeight)
        return FALSE;

    return nCelBottom + nCelHeight >= 0;
}


//1.10f: D2Client.0x6FB5E170
//  A cel against the screen. The cel's height, its two draw offsets added to nX and nY, its width; with bShear the left edge moves back by the
//  height and the width grows by it, a sheared bound for a sprite drawn leaning. Then the four tests of GFXUTIL_IsUnitCelOnScreen, all signed;
//  the vertical band is again twice the height. retn 8: nY and bShear on the stack.
int32_t __fastcall GFXUTIL_IsCelVisible(D2GfxCellStrc *pCel, int32_t nX, int32_t nY, int32_t bShear)
{
    int32_t nHeight;
    int32_t nAnchorY;
    int32_t nAnchorX;
    int32_t nWidth;

    nHeight = D2CMP_CELCMP_CelGetHeight(pCel);
    nAnchorY = nY + D2CMP_CELCMP_CelGetOffsetY(pCel);
    nAnchorX = nX + D2CMP_CELCMP_CelGetOffsetX(pCel);
    nWidth = D2CMP_CELCMP_CelGetWidth(pCel);
    if (bShear)
    {
        nAnchorX -= nHeight;
        nWidth += nHeight;
    }

    if (nAnchorX > gnScreenWidth)
        return FALSE;

    if (nAnchorX + nWidth < 0)
        return FALSE;

    if (nAnchorY - nHeight > gnScreenHeight)
        return FALSE;

    return nAnchorY + nHeight >= 0;
}


//1.10f: D2Client.0x6FB5E200
//  Does the unit have this component? The unit's type, class and mode go through #11013 ConvertMode (assert line 603) -- a NULL unit enters as
//  type 6, UNIT_TYPES_COUNT, and class -1, which D2Common leaves alone, so it falls to the default -- then by the converted type:
//      UNIT_PLAYER   components 0..4, 8 and 9 -- head, torso, legs, the two arms, special 1 and 2
//      UNIT_MONSTER  the component's bit in monstats2.txt's dwComponentFlags (+0E8h), byte nComponent >> 3, mask gdwBitMasks[n & 7]
//      UNIT_OBJECT   objects.txt's byte for the component, nHD + nComponent, equal to 1
//      UNIT_ITEM     component 1 alone
//  and everything else FALSE. The type switch is unsigned, and so is the player's. The component byte and the mask are tested a byte at a time.
//  The original inlines DATATBLS_GetMonStats2TxtRecord here.
//  BUG: a negative component passes the bound, which is signed (jge): the player's unsigned switch and the item's compare refuse it, but the
//  monster arm reads the byte at +0E8h + (n >> 3), shifted arithmetically, and the object arm the byte at +151h + n, both before the field they
//  mean. UNREACHABLE: the one caller, CUNIT_GetComponentScreenBox, is called with 1 and with a counter running 0..15, never negative.
BOOL __fastcall GFXUTIL_UnitHasComponent(D2UnitStrc *pUnit, int32_t nComponent)
{
    int32_t nType;
    int32_t nClass;
    int32_t nMode;
    D2MonStats2Txt *pMonStats2Txt;
    D2ObjectsTxt *pObjectsTxt;

    //  signed, as jge is: the negative component the bytes let by is not refused
    if (nComponent >= (int32_t)NUM_COMPONENTS)
        return FALSE;

    nType = pUnit ? (int32_t)pUnit->dwUnitType : (int32_t)UNIT_TYPES_COUNT;
    nClass = pUnit ? pUnit->dwClassId : -1;
    nMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
    D2COMMON_11013_ConvertMode(pUnit, &nType, &nClass, &nMode, szGfxUtilCpp, 603);

    switch (nType)
    {
    case UNIT_PLAYER:
        switch (nComponent)
        {
        case COMPOSIT_HEAD:
        case COMPOSIT_TORSO:
        case COMPOSIT_LEGS:
        case COMPOSIT_RIGHTARM:
        case COMPOSIT_LEFTARM:
        case COMPOSIT_SPECIAL1:
        case COMPOSIT_SPECIAL2:
            return TRUE;
        }
        return FALSE;

    case UNIT_MONSTER:
        pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord(nClass);
        if (!pMonStats2Txt)
            return FALSE;

        if (!(((uint8_t *)&pMonStats2Txt->dwComponentFlags)[nComponent >> 3] & (uint8_t)gdwBitMasks[nComponent & 7]))
            return FALSE;

        return TRUE;

    case UNIT_OBJECT:
        pObjectsTxt = D2COMMON_DATATBLS_GetObjectsTxtRecord(nClass);
        if (!pObjectsTxt || (&pObjectsTxt->nHD)[nComponent] != 1)
            return FALSE;

        return TRUE;

    case UNIT_ITEM:
        if (nComponent != (int32_t)COMPOSIT_TORSO)
            return FALSE;

        return TRUE;
    }

    return FALSE;
}


//1.10f: D2Client.0x6FB5E370
//  The identity of a gfx request, and the tokens of its file name. retn 8: pUnit and pGfxData in ecx and edx, pnClass and nModeOverride on the
//  stack. Every caller uses the answer.
//  The head: a unit's type, class and anim-or-sequence mode go through #11013 ConvertMode (assert line 752) into the block's nUnitType, nUnitIndex
//  and nMode, nModeOverride replacing the mode unless it is -1. A NULL unit sets nUnitType to 6 and nothing else, and jumps into the type's jump
//  table with 6, past the bound: 6, UNIT_TYPES_COUNT, is the OVERLAY request, the arm that reads nOverlay, and no unit reaches it. Type 5 and
//  anything above 6 are FALSE.
//  Players, monsters and objects (cases 0..2) fill the five tokens a COF name is spelled from, dwName[0..4]: the unit, the component, the armour,
//  the mode and the weapon class. *pnClass becomes -1; the component token comes from gaCompositTokens; D2Common's weapon class (#10888) is
//  stored in dwName[4] for the anim node GFX_GetUnitAnimNode or GFX_RebuildUnitGfx must find, and overwritten at the end by the COF layer's own.
//  Then the unit's COF data and its gfx info (either missing is FALSE), "lit " as the armour, and the unit token by the block's type: plrtype's
//  token, monstats' code (+10h) or objtype's token (the player and object getters called with bGetMode 0, which answers the type table).
//  The COF layer: the COF data is a 28-byte header (its layer count the first byte) followed by 9-byte layer records; the walk wants the layer
//  whose first byte is the block's component and gives up (FALSE) on a component byte of 16 or more or after the last layer. Then by the
//  converted type:
//      PLAYER   fState bit 8 for the local player; no inventory is FALSE with "lit " stored; the plrmode token; the torso's six armour
//               components; an armour component's armtype token -- "lit " in PLRMODE_DEATH and PLRMODE_DEAD; any other component "lit " at
//               16 or more, else a crossbow's right-hand item's alternate gfx or code for both hands, "lit " for a bow's right hand, "hsh "
//               for the shield under Holy Shield, and otherwise the composit item's own alternate gfx when it can be equipped; "lit " if
//               still nothing
//      MONSTER  the monmode token; the layer's variant byte from the unit's D2MonsterDataStrc (a real monster only: a shapeshifted player
//               converts to type 1 and reads 0); #11069's armour token, in the two death modes only for a monstats2.txt compositedeath row
//      OBJECT   the objmode token
//  and the tail: the weapon class from the layer's bytes 5..7, padded with spaces; any of the unit, component or weapon-class tokens missing is
//  FALSE; "xxx " for a missing mode and "lit " for a missing armour; and the armour token looked up as an item code, the item's id into *pnClass.
//  MISSILES (3): missiles.txt CelFile as the name. ITEMS (4): the mode collapses to IMODE_ONCURSOR (stored, equipped, belt, cursor, socketed) or
//  IMODE_ONGROUND (ground, dropping); the class (through Engine.h's inline for a shapeshifted unit) picks the items.txt row; on the cursor an
//  identified set item's setitems.txt invfile or the base item's setinvfile (fState bit 10h), an identified unique's uniqueitems.txt invfile or
//  the base item's uniqueinvfile (bit 1), else the variable inventory graphic or invfile; on the ground flippyfile, a unique's or a set item's own
//  when it has one, with no identified test. OVERLAYS (6): fState bit 2 and overlay.txt Filename. Every one of those answers TRUE.
//  The player arm tests the converted type again before the torso: in this arm it is always UNIT_PLAYER, so the test cannot take the clear by
//  itself; written as the bytes test it.
//  The original inlines GFXUTIL_GetUnitDisplayInventory (twice), DATATBLS_GetMonStatsTxtRecord, DATATBLS_GetMonStats2TxtRecord,
//  GFXUTIL_CopyTokenPadded, a missiles.txt record lookup, GFXUTIL_GetEffectiveClassId, DATATBLS_GetSetItemsTxtRecord and
//  GFXUTIL_GetOverlayTxtRecord here.
//  BUG: the armour components can be left unwritten. When the torso item can be equipped but its items.txt record is NULL, the jz at 0x6FB5E6CD
//  skips both the copy and the clear, and the six bytes reach #10891 COMPOSIT_GetArmorTypeFromComponent as the frame left them. Reproduced: the
//  buffer is declared uninitialised and only its address is passed, so nothing indeterminate is read here (D2Common reads it). LATENT: a NULL
//  record needs a class outside the table, which only a server whose items.txt the client lacks would send; whether such an item lives to be
//  worn is not established.
//  BUG: the inventory graphic skips unique id 0. The cursor arm takes a unique's own record only for a file index above 0 (jle 0x6FB5EAB0); the
//  ground arm takes it from 0 (jl 0x6FB5EB64). LATENT: unique id 0 is The Gnasher, whose invfile is also its base item's uniqueinvfile, so the arm
//  the skip falls to draws the same file; data that gave unique 0 a graphic of its own would draw the base's in the cursor.
int32_t __fastcall GFXUTIL_ResolveUnitAnimIdentity(D2UnitStrc *pUnit, D2GfxDataStrc *pGfxData, int32_t *pnClass, int32_t nModeOverride)
{
    int32_t nType;
    int32_t nClass;
    int32_t nMode;
    int32_t nCodeItemId;
    int32_t nWeaponClassIdOut;
    uint8_t aArmorComponents[6];
    D2InventoryStrc *pInventory;
    D2InventoryStrc *pWeaponInventory;
    D2UnitStrc *pOwner;
    uint8_t *pCofData;
    uint8_t *pLayer;
    int32_t nLayers;
    int32_t iLayer;
    int32_t iArmor;
    int32_t nAnimMode;
    int32_t nWeaponClass;
    uint32_t nArmorType;
    uint32_t nComponent;
    uint8_t nLayerComponent;
    uint8_t nVariant;
    uint8_t nVarGfx;
    uint32_t dwUnitToken;
    uint32_t dwArmorToken;
    uint32_t dwWClassToken;
    int32_t nConvType;
    int32_t nConvClass;
    int32_t nConvMode;
    int32_t nItemId;
    int32_t nFileIndex;
    char *pszFlippyFile;
    D2PlrModeTypeTxt *pPlrTypeTxt;
    D2ObjModeTypeTxt *pObjTypeTxt;
    D2MonStatsTxt *pMonStatsTxt;
    D2MonStats2Txt *pMonStats2Txt;
    D2UnitStrc *pTorso;
    D2UnitStrc *pCompositItem;
    D2UnitStrc *pRightHand;
    D2ItemsTxt *pItemsTxt;
    D2SetItemsTxt *pSetItemsTxt;
    D2UniqueItemsTxt *pUniqueItemsTxt;
    D2MissilesTxt *pMissilesTxt;
    D2OverlayTxt *pOverlayTxt;

    nType = UNIT_TYPES_COUNT;
    if (!pUnit)
    {
        pGfxData->nUnitType = UNIT_TYPES_COUNT;
    }
    else
    {
        nType = (int32_t)pUnit->dwUnitType;
        nClass = pUnit->dwClassId;
        nMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
        D2COMMON_11013_ConvertMode(pUnit, &nType, &nClass, &nMode, szGfxUtilCpp, 752);
        pGfxData->nUnitIndex = nClass;
        pGfxData->nUnitType = nType;
        pGfxData->nMode = nModeOverride != -1 ? nModeOverride : nMode;
    }

    switch (nType)
    {
    case UNIT_PLAYER:
    case UNIT_MONSTER:
    case UNIT_OBJECT:
        *pnClass = -1;

        pInventory = GFXUTIL_GetUnitDisplayInventory(pUnit, NULL);
        pGfxData->dwName[1] = (uint32_t)gaCompositTokens[pGfxData->nComponent];
        nWeaponClass = D2COMMON_COMPOSIT_GetWeaponClassId(pUnit, pInventory, &nWeaponClassIdOut, pGfxData->nMode, FALSE);
        pGfxData->dwName[4] = (uint32_t)nWeaponClass;
        if (!GFX_GetUnitAnimNode(pUnit, nWeaponClass, pGfxData->nMode) && !GFX_RebuildUnitGfx(pUnit, (int32_t)pGfxData->dwName[4], pGfxData->nMode))
        {
            return 0;
        }

        pCofData = (uint8_t *)GFX_GetUnitCofData(pUnit, pGfxData->nMode);
        if (!pCofData)
            return 0;

        if (!CUNIT_GfxGetUnitGfxInfo(pUnit))
            return 0;

        pGfxData->dwName[2] = GFXUTIL_Token('l', 'i', 't', ' ');
        switch (pGfxData->nUnitType)
        {
        case UNIT_PLAYER:
            pPlrTypeTxt = D2COMMON_DATATBLS_GetPlrModeTypeTxtRecord(pGfxData->nUnitIndex, FALSE);
            dwUnitToken = pPlrTypeTxt ? *(uint32_t *)pPlrTypeTxt->szToken : 0;
            break;

        case UNIT_MONSTER:
            pMonStatsTxt = DATATBLS_GetMonStatsTxtRecord(pGfxData->nUnitIndex);
            dwUnitToken = pMonStatsTxt ? pMonStatsTxt->dwCode : 0;
            break;

        case UNIT_OBJECT:
            pObjTypeTxt = D2COMMON_DATATBLS_GetObjModeTypeTxtRecord(pGfxData->nUnitIndex, FALSE);
            dwUnitToken = pObjTypeTxt ? *(uint32_t *)pObjTypeTxt->szToken : 0;
            break;

        default:
            dwUnitToken = 0;
            break;
        }

        pGfxData->dwName[0] = dwUnitToken;

        nLayers = pCofData[0];
        pLayer = pCofData + 0x1C;
        if (nLayers <= 0)
            return 0;

        iLayer = 0;
        for (;;)
        {
            if (pLayer[0] >= NUM_COMPONENTS)
                return 0;

            if (pLayer[0] == pGfxData->nComponent)
                break;

            pLayer += 9;
            if (++iLayer >= nLayers)
                return 0;
        }

        switch (nType)
        {
        case UNIT_PLAYER:
            if (pUnit == CUNIT_GetPlayer())
                pGfxData->fState |= 8;

            if (!pInventory)
            {
                pGfxData->dwName[2] = GFXUTIL_Token('l', 'i', 't', ' ');
                return 0;
            }

            pGfxData->dwName[3] = (uint32_t)gaPlrModeTokens[pGfxData->nMode];
            pTorso = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_TORSO);
            if (nType != UNIT_PLAYER || !D2COMMON_ITEMS_CanBeEquipped(pTorso))
            {
                memset(aArmorComponents, 0, sizeof(aArmorComponents));
            }
            else
            {
                pItemsTxt = D2COMMON_DATATBLS_GetItemsTxtRecord(pTorso ? pTorso->dwClassId : -1);
                if (pItemsTxt)
                {
                    for (iArmor = 0; iArmor < 6; ++iArmor)
                        aArmorComponents[iArmor] = pItemsTxt->nArmorComp[iArmor];
                }
            }

            pCompositItem = D2COMMON_INVENTORY_GetCompositItem(pInventory, pGfxData->nComponent);
            if (D2COMMON_COMPOSIT_IsArmorComponent((D2Composits)pGfxData->nComponent))
            {
                nAnimMode = pGfxData->nMode;
                if (nAnimMode != PLRMODE_DEAD && nAnimMode != PLRMODE_DEATH)
                {
                    nArmorType = D2COMMON_COMPOSIT_GetArmorTypeFromComponent((D2Composits)pGfxData->nComponent, aArmorComponents);
                    pGfxData->dwName[2] = (uint32_t)gaArmTypeTokens[nArmorType];
                }
                else
                {
                    pGfxData->dwName[2] = GFXUTIL_Token('l', 'i', 't', ' ');
                }
            }
            else
            {
                nComponent = pGfxData->nComponent;
                if ((int32_t)nComponent >= (int32_t)NUM_COMPONENTS)
                {
                    dwArmorToken = GFXUTIL_Token('l', 'i', 't', ' ');
                }
                else
                {
                    if (GFX_TestFlagEx(pUnit, UNITFLAGEX_ISSHAPESHIFTED) && D2COMMON_STATES_CheckStateMaskOnUnit(pUnit, STATEMASK_BOSSINV) && (pOwner = CUNIT_GetOwnerUnit(pUnit)) != NULL)
                    {
                        pWeaponInventory = pOwner->pInventory;
                    }
                    else
                    {
                        pWeaponInventory = pUnit->pInventory;
                    }

                    nWeaponClass = D2COMMON_COMPOSIT_GetWeaponClassId(pUnit, pWeaponInventory, &nMode, -1, TRUE);
                    if (nWeaponClass == (int32_t)GFXUTIL_Token('x', 'b', 'w', ' ') && (nComponent == COMPOSIT_RIGHTHAND || nComponent == COMPOSIT_LEFTHAND))
                    {
                        pRightHand = D2COMMON_INVENTORY_GetCompositItem(pWeaponInventory, COMPOSIT_RIGHTHAND);
                        if (!pRightHand)
                        {
                            dwArmorToken = GFXUTIL_Token('l', 'i', 't', ' ');
                        }
                        else
                        {
                            pItemsTxt = D2COMMON_DATATBLS_GetItemsTxtRecord(pRightHand->dwClassId);
                            if (!pItemsTxt)
                                dwArmorToken = GFXUTIL_Token('l', 'i', 't', ' ');
                            else if (pItemsTxt->dwAlternateGfx)
                                dwArmorToken = pItemsTxt->dwAlternateGfx;
                            else
                                dwArmorToken = pItemsTxt->dwCode;
                        }
                    }
                    else if (nWeaponClass == (int32_t)GFXUTIL_Token('b', 'o', 'w', ' ') && nComponent == COMPOSIT_RIGHTHAND)
                    {
                        dwArmorToken = GFXUTIL_Token('l', 'i', 't', ' ');
                    }
                    else if (nComponent == COMPOSIT_SHIELD && D2COMMON_STATES_CheckState(pUnit, STATE_HOLYSHIELD) && D2COMMON_INVENTORY_GetEquippedShield(pWeaponInventory, NULL))
                    {
                        dwArmorToken = GFXUTIL_Token('h', 's', 'h', ' ');
                    }
                    else
                    {
                        dwArmorToken = 0;
                    }
                }

                pGfxData->dwName[2] = dwArmorToken;
                if (pCompositItem && !dwArmorToken && D2COMMON_ITEMS_CanBeEquipped(pCompositItem))
                    pGfxData->dwName[2] = D2COMMON_ITEMS_GetAltGfx(pCompositItem);
                else if (!pGfxData->dwName[2])
                    pGfxData->dwName[2] = GFXUTIL_Token('l', 'i', 't', ' ');
            }

            if (!pGfxData->dwName[2])
                return 0;

            break;

        case UNIT_MONSTER:
            nAnimMode = pGfxData->nMode;
            pGfxData->dwName[3] = (uint32_t)gaMonModeTokens[nAnimMode];
            nLayerComponent = pLayer[0];
            if (pUnit->dwUnitType == UNIT_MONSTER && nLayerComponent < NUM_COMPONENTS && pUnit->pMonsterData)
                nVariant = pUnit->pMonsterData->nComponent[nLayerComponent];
            else
                nVariant = 0;

            if (nAnimMode != MONMODE_DEATH && nAnimMode != MONMODE_DEAD)
            {
                pGfxData->dwName[2] = D2COMMON_11069(pUnit, nLayerComponent, nVariant);
            }
            else
            {
                pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord(pGfxData->nUnitIndex);
                if (pMonStats2Txt && (pMonStats2Txt->nFlags[MONSTATS2FLAGINDEX_COMPOSITEDEATH >> 3] & gdwBitMasks[MONSTATS2FLAGINDEX_COMPOSITEDEATH & 7]))
                {
                    pGfxData->dwName[2] = D2COMMON_11069(pUnit, nLayerComponent, nVariant);
                }
            }

            break;

        case UNIT_OBJECT:
            pGfxData->dwName[3] = (uint32_t)gaObjModeTokens[pGfxData->nMode];
            break;
        }

        dwWClassToken = 0;
        GFXUTIL_CopyTokenPadded((const char *)&pLayer[5], (char *)&dwWClassToken);
        pGfxData->dwName[4] = dwWClassToken;
        if (!pGfxData->dwName[1] || !pGfxData->dwName[0] || !dwWClassToken)
            return 0;

        if (!pGfxData->dwName[3])
            pGfxData->dwName[3] = GFXUTIL_Token('x', 'x', 'x', ' ');

        if (!pGfxData->dwName[2])
            pGfxData->dwName[2] = GFXUTIL_Token('l', 'i', 't', ' ');

        if (D2COMMON_DATATBLS_GetItemRecordFromItemCode(pGfxData->dwName[2], &nCodeItemId))
            *pnClass = nCodeItemId;

        return 1;

    case UNIT_MISSILE:
        if (pUnit)
        {
            nClass = pUnit->dwClassId;

            if (nClass >= 0 && nClass < sgptDataTables->nMissilesTxtRecordCount)
            {
                pMissilesTxt = &sgptDataTables->pMissilesTxt[nClass];
                if (pMissilesTxt)
                    pGfxData->pName = pMissilesTxt->szCelFile;
            }
        }

        return 1;

    case UNIT_ITEM:
        switch (pGfxData->nMode)
        {
        case IMODE_STORED:
        case IMODE_EQUIP:
        case IMODE_INBELT:
        case IMODE_ONCURSOR:
        case IMODE_SOCKETED:
            pGfxData->nMode = IMODE_ONCURSOR;
            break;

        case IMODE_ONGROUND:
        case IMODE_DROPPING:
            pGfxData->nMode = IMODE_ONGROUND;
            break;
        }

        if (GFX_TestFlagEx(pUnit, UNITFLAGEX_ISSHAPESHIFTED))
        {
            nConvType = pUnit ? (int32_t)pUnit->dwUnitType : (int32_t)UNIT_TYPES_COUNT;
            nConvClass = pUnit ? pUnit->dwClassId : -1;
            nConvMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
            D2COMMON_11013_ConvertMode(pUnit, &nConvType, &nConvClass, &nConvMode, szEngineH, 278);
            nItemId = nConvClass;
        }
        else
        {
            nItemId = pUnit ? pUnit->dwClassId : -1;
        }

        pItemsTxt = D2COMMON_DATATBLS_GetItemsTxtRecord(nItemId);
        if (!pItemsTxt)
            return 1;

        nAnimMode = pGfxData->nMode;
        if (nAnimMode == IMODE_ONCURSOR)
        {
            nVarGfx = D2COMMON_ITEMS_GetVarGfxIndex(pUnit);
            if (D2COMMON_ITEMS_CheckItemFlag(pUnit, IFLAG_IDENTIFIED, 406, szGfxUtilCpp) && D2COMMON_ITEMS_GetItemQuality(pUnit) == ITEMQUAL_SET)
            {
                pSetItemsTxt = DATATBLS_GetSetItemsTxtRecord(D2COMMON_ITEMS_GetFileIndex(pUnit));
                if (pSetItemsTxt && pSetItemsTxt->szInvFile[0])
                {
                    pGfxData->pName = pSetItemsTxt->szInvFile;
                    pGfxData->fState |= 0x10;
                    return 1;
                }

                if (pItemsTxt->szSetInvFile[0])
                {
                    pGfxData->pName = pItemsTxt->szSetInvFile;
                    pGfxData->fState |= 0x10;
                    return 1;
                }
            }

            if (D2COMMON_ITEMS_CheckItemFlag(pUnit, IFLAG_IDENTIFIED, 430, szGfxUtilCpp) && D2COMMON_ITEMS_GetItemQuality(pUnit) == ITEMQUAL_UNIQUE)
            {
                nFileIndex = D2COMMON_ITEMS_GetFileIndex(pUnit);
                if (nFileIndex > 0)
                {
                    pUniqueItemsTxt = GFXUTIL_GetUniqueItemsTxtRecord(nFileIndex);
                    if (pUniqueItemsTxt && pUniqueItemsTxt->szInvFile[0])
                    {
                        pGfxData->pName = pUniqueItemsTxt->szInvFile;
                        pGfxData->fState |= 1;
                        return 1;
                    }
                }

                if (pItemsTxt->szUniqueInvFile[0])
                {
                    pGfxData->pName = pItemsTxt->szUniqueInvFile;
                    pGfxData->fState |= 1;
                    return 1;
                }
            }

            if (D2COMMON_ITEMS_GetVarInvGfxCount(pUnit))
                pGfxData->pName = D2COMMON_ITEMS_GetVarInvGfxString(pUnit, nVarGfx);
            else
                pGfxData->pName = pItemsTxt->szInvFile;

            return 1;
        }

        if (nAnimMode == IMODE_ONGROUND)
        {
            pGfxData->pName = pItemsTxt->szFlippyFile;
            pszFlippyFile = NULL;
            if (D2COMMON_ITEMS_GetItemQuality(pUnit) == ITEMQUAL_UNIQUE)
            {
                if (D2COMMON_ITEMS_GetFileIndex(pUnit) >= 0)
                {
                    pUniqueItemsTxt = GFXUTIL_GetUniqueItemsTxtRecord(D2COMMON_ITEMS_GetFileIndex(pUnit));
                    if (pUniqueItemsTxt)
                        pszFlippyFile = pUniqueItemsTxt->szFlippyFile;
                }
            }
            else if (D2COMMON_ITEMS_GetItemQuality(pUnit) == ITEMQUAL_SET)
            {
                pSetItemsTxt = DATATBLS_GetSetItemsTxtRecord(D2COMMON_ITEMS_GetFileIndex(pUnit));
                if (pSetItemsTxt)
                    pszFlippyFile = pSetItemsTxt->szFlippyFile;
            }

            if (pszFlippyFile && pszFlippyFile[0])
                pGfxData->pName = pszFlippyFile;
        }

        return 1;

    case UNIT_TYPES_COUNT:
        pGfxData->fState |= 2;

        pOverlayTxt = GFXUTIL_GetOverlayTxtRecord(pGfxData->nOverlay);
        if (pOverlayTxt)
            pGfxData->pName = pOverlayTxt->szFilename;

        return 1;
    }

    return 0;
}


//1.10f: D2Client.0x6FB5EC80
//  Fills and resolves a gfx request: four fields (pCellFile, nDirection, nFrame, and nComponent as a byte), then GFXUTIL_ResolveUnitAnimIdentity's
//  answer, handed on. retn 18h: six arguments on the stack. Not item-specific: CUNIT_GetComponentScreenBox passes any unit's component.
int32_t __fastcall GFXUTIL_FillGfxData(D2GfxDataStrc *pGfxData, D2UnitStrc *pUnit, D2CellFileStrc *pCellFile, int32_t nDirection, int32_t nFrame, int32_t *pnClass, uint8_t nComponent, int32_t nModeOverride)
{
    pGfxData->pCellFile = pCellFile;
    pGfxData->nDirection = (uint32_t)nDirection;
    pGfxData->nFrame = (uint32_t)nFrame;
    pGfxData->nComponent = nComponent;
    return GFXUTIL_ResolveUnitAnimIdentity(pUnit, pGfxData, pnClass, nModeOverride);
}


//1.10f: D2Client.0x6FB5ECC0
//  A monster component's cel context, from a cached COF: the monster counterpart of ResolveUnitAnimIdentity's composite arm, for a COF the caller
//  already holds. nUnitType 1, nUnitIndex the monster id (the word pMonTokens->nMonsterId, sign-extended), nMode, nComponent as a byte, the
//  component token, "lit ", the monster's code as the unit token; then the COF's layer for the component, the monmode token, #11070's armour token
//  (in the two death modes only for a compositedeath row) and the layer's weapon class (a call to GFXUTIL_CopyTokenPadded). retn 20h: eight
//  arguments on the stack.
//  Seven asserts, each followed by exit(-1): "dwWClass" (line 994) and "ptCofData" (995) on the inputs, "lpbCofData" (1008) on the node's data,
//  "ptCofActionData->bComponent < NUM_COMPONENTS" (1019) on each layer walked, and ptCelContext->tUnitInfo's dwComponentToken (1061), dwUnitToken
//  (1062) and dwWClassToken (1063) on the result. dwWClass is asserted and stored into dwName[4] and the layer's weapon class overwrites it: the
//  caller's value is only required to be non-zero.
//  pCofNode is a COF cache node (its +14h is pData). pMonTokens is a D2ClientMonCofTokensStrc: its first word is the monster id, and the byte at
//  +4 + 16 * nCofDirection + the component is the component's variant. The layer walk is unsigned, where the resolver's is signed.
//  The original inlines DATATBLS_GetMonStats2TxtRecord here.
int32_t __fastcall GFXUTIL_FillCelContextTokens(D2GfxDataStrc *pGfxData, int32_t nDirection, int32_t nFrame, uint8_t nComponent, int32_t nAnimMode, int32_t dwWClass,
                                            D2ClientCofCacheNodeStrc *pCofNode, D2ClientMonCofTokensStrc *pMonTokens, D2MonStatsTxt *pMonStatsTxt, int32_t nCofDirection)
{
    uint8_t *pCofData;
    uint8_t *pLayer;
    uint32_t nLayers;
    uint32_t iLayer;
    uint8_t nVariant;
    D2MonStats2Txt *pMonStats2Txt;
    uint32_t dwWClassToken;

    pGfxData->nFrame = (uint32_t)nFrame;
    pGfxData->nDirection = (uint32_t)nDirection;
    pGfxData->nComponent = nComponent;
    if (!dwWClass)
    {
        FOG_DisplayAssert("dwWClass", szGfxUtilCpp, 994);
        exit(-1);
    }

    if (!pCofNode)
    {
        FOG_DisplayAssert("ptCofData", szGfxUtilCpp, 995);
        exit(-1);
    }

    pGfxData->nUnitIndex = pMonTokens->nMonsterId;
    pGfxData->nMode = nAnimMode;
    pGfxData->nUnitType = UNIT_MONSTER;
    pGfxData->dwName[4] = (uint32_t)dwWClass;
    pGfxData->dwName[1] = (uint32_t)gaCompositTokens[(uint8_t)nComponent];
    pGfxData->dwName[2] = GFXUTIL_Token('l', 'i', 't', ' ');
    pGfxData->dwName[0] = pMonStatsTxt->dwCode;
    pCofData = (uint8_t *)pCofNode->pData;
    if (!pCofData)
    {
        FOG_DisplayAssert("lpbCofData", szGfxUtilCpp, 1008);
        exit(-1);
    }

    pLayer = pCofData + 0x1C;
    nLayers = pCofData[0];
    if (!nLayers)
        return 0;

    iLayer = 0;
    for (;;)
    {
        if (pLayer[0] >= NUM_COMPONENTS)
        {
            FOG_DisplayAssert("ptCofActionData->bComponent < NUM_COMPONENTS", szGfxUtilCpp, 1019);
            exit(-1);
        }

        if (pLayer[0] == (uint8_t)nComponent)
            break;

        pLayer += 9;
        if (++iLayer >= nLayers)
            return 0;
    }

    pGfxData->dwName[3] = (uint32_t)gaMonModeTokens[nAnimMode];
    nVariant = pMonTokens->nVariants[nCofDirection][(uint8_t)nComponent];
    if (nAnimMode != MONMODE_DEATH && nAnimMode != MONMODE_DEAD)
    {
        pGfxData->dwName[2] = (uint32_t)D2COMMON_11070(pGfxData->nUnitIndex, (uint8_t)nComponent, nVariant);
    }
    else
    {
        pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord(pGfxData->nUnitIndex);
        if (pMonStats2Txt && (pMonStats2Txt->nFlags[MONSTATS2FLAGINDEX_COMPOSITEDEATH >> 3] & gdwBitMasks[MONSTATS2FLAGINDEX_COMPOSITEDEATH & 7]))
            pGfxData->dwName[2] = (uint32_t)D2COMMON_11070(pGfxData->nUnitIndex, (uint8_t)nComponent, nVariant);
    }

    dwWClassToken = 0;
    GFXUTIL_CopyTokenPadded((const char *)&pLayer[5], (char *)&dwWClassToken);
    pGfxData->dwName[4] = dwWClassToken;
    if (!pGfxData->dwName[1])
    {
        FOG_DisplayAssert("ptCelContext->tUnitInfo.dwComponentToken", szGfxUtilCpp, 1061);
        exit(-1);
    }

    if (!pGfxData->dwName[0])
    {
        FOG_DisplayAssert("ptCelContext->tUnitInfo.dwUnitToken", szGfxUtilCpp, 1062);
        exit(-1);
    }

    if (!dwWClassToken)
    {
        FOG_DisplayAssert("ptCelContext->tUnitInfo.dwWClassToken", szGfxUtilCpp, 1063);
        exit(-1);
    }

    if (!pGfxData->dwName[2])
        pGfxData->dwName[2] = GFXUTIL_Token('x', 'x', 'x', ' ');

    if (!pGfxData->dwName[3])
        pGfxData->dwName[3] = GFXUTIL_Token('x', 'x', 'x', ' ');

    return 1;
}


//1.10f: D2Client.0x6FB5EF30
//  Returns its argument: mov eax, ecx / retn, the whole body.
void *__fastcall GFXUTIL_ReturnArg(void *p)
{
    return p;
}


//1.10f: D2Client.0x6FB5EF40
//  Byte +3 of the COF layer record for nComponent, or 0: no COF, a component of 16 or more (signed), no layers, or no layer for it. The walk is
//  the one ResolveUnitAnimIdentity and FillCelContextTokens make (9-byte records from +1Ch, pCofData[0] of them) but returns where they break.
//  Taken as the flag that the layer overrides the transparency level; the level is byte +4, GFXUTIL_GetCofLayerTransLvl.
int32_t __fastcall GFXUTIL_GetCofLayerOverrideTransLvl(uint8_t *pCofData, int32_t nComponent)
{
    uint8_t *pLayer;
    int32_t nLayers;
    int32_t iLayer;

    if (!pCofData || nComponent >= (int32_t)NUM_COMPONENTS)
        return 0;

    pLayer = pCofData + 0x1C;
    iLayer = 0;
    nLayers = pCofData[0];
    if (nLayers <= 0)
        return 0;

    while (pLayer[0] != nComponent)
    {
        pLayer += 9;
        if (++iLayer >= nLayers)
            return 0;
    }

    return pLayer[3];
}


//1.10f: D2Client.0x6FB5EF80
//  Byte +1 of the COF layer record for nComponent, or 0: the walk above, byte for byte, but for the byte it returns. Taken as the layer's shadow
//  flag.
int32_t __fastcall GFXUTIL_GetCofLayerCastsShadow(uint8_t *pCofData, int32_t nComponent)
{
    uint8_t *pLayer;
    int32_t nLayers;
    int32_t iLayer;

    if (!pCofData || nComponent >= (int32_t)NUM_COMPONENTS)
        return 0;

    pLayer = pCofData + 0x1C;
    iLayer = 0;
    nLayers = pCofData[0];
    if (nLayers <= 0)
        return 0;

    while (pLayer[0] != nComponent)
    {
        pLayer += 9;
        if (++iLayer >= nLayers)
            return 0;
    }

    return pLayer[1];
}


//1.10f: D2Client.0x6FB5EFC0
//  The COF's layer order for one direction and frame: pCofData[0] bytes, each a component, from
//      pCofData + 1Ch + nLayers * (nFrames * nDirection + nFrame + 9) + nFrames
//  with nLayers the header's byte 0 and nFrames its byte 1: the 28-byte header, the nLayers 9-byte layer records, nFrames bytes, then nLayers
//  bytes per (direction, frame). The nFrames bytes between the layer records and the order table are not read here. NULL for no COF; no bound on
//  either index. retn 4: nFrame on the stack. The multiplies are signed (imul).
uint8_t *__fastcall GFXUTIL_GetCofLayerOrder(uint8_t *pCofData, int32_t nDirection, int32_t nFrame)
{
    int32_t nFrames;

    if (!pCofData)
        return NULL;

    nFrames = pCofData[1];
    return pCofData + 0x1C + pCofData[0] * (nFrames * nDirection + nFrame + 9) + nFrames;
}


//1.10f: D2Client.0x6FB5EFF0
//  Byte +4 of the COF layer record for nComponent (the layer's draw mode when GFXUTIL_GetCofLayerOverrideTransLvl says it overrides), and
//  DRAWMODE_NORMAL, 5, on every refusal, where the two siblings answer 0. The same walk.
int32_t __fastcall GFXUTIL_GetCofLayerTransLvl(uint8_t *pCofData, int32_t nComponent)
{
    uint8_t *pLayer;
    int32_t nLayers;
    int32_t iLayer;

    if (!pCofData || nComponent >= (int32_t)NUM_COMPONENTS)
        return DRAWMODE_NORMAL;

    pLayer = pCofData + 0x1C;
    iLayer = 0;
    nLayers = pCofData[0];
    if (nLayers <= 0)
        return DRAWMODE_NORMAL;

    while (pLayer[0] != nComponent)
    {
        pLayer += 9;
        if (++iLayer >= nLayers)
            return DRAWMODE_NORMAL;
    }

    return pLayer[4];
}


//1.10f: D2Client.0x6FB5F030
//  Is the unit a player, a monster or an object, the three drawn by components? FALSE for NULL; the type is tested signed both ways.
//  A __thiscall taking nothing but ecx is a __fastcall of one argument.
BOOL __fastcall GFXUTIL_IsDrawableUnitType(D2UnitStrc *pUnit)
{
    if (!pUnit)
        return FALSE;

    if ((int32_t)pUnit->dwUnitType < (int32_t)UNIT_PLAYER || (int32_t)pUnit->dwUnitType > (int32_t)UNIT_OBJECT)
        return FALSE;

    return TRUE;
}


//1.10f: D2Client.0x6FB5F050
//  Is this mode's COF loaded for the unit: the low byte of its word in D2ClientUnitGfxInfoStrc's wModeLoaded[nMode], or 0 when the unit has no
//  gfx block; the index is unbounded. Only al is written, and both callers test al alone. The counterpart of GFXUTIL_SetUnitModeLoaded.
//  The index is a mode, not a component: GFX_ClearUnitModesLoaded clears 20 words for a player, 16 for a monster and 8 for an object.
uint8_t __fastcall GFXUTIL_IsUnitModeLoaded(D2UnitStrc *pUnit, int32_t nMode)
{
    D2ClientUnitGfxInfoStrc *pGfxInfo;

    pGfxInfo = CUNIT_GfxGetUnitGfxInfo(pUnit);
    if (!pGfxInfo)
        return 0;

    return (uint8_t)pGfxInfo->wModeLoaded[nMode];
}


//1.10f: D2Client.0x6FB5F070
//  Marks a mode's COF loaded, or not, for the unit: the low byte of wModeLoaded[nMode], the high byte left; nothing when the unit has no gfx
//  block. retn 4: bLoaded on the stack. void: no caller reads eax.
void __fastcall GFXUTIL_SetUnitModeLoaded(D2UnitStrc *pUnit, int32_t nMode, uint8_t bLoaded)
{
    D2ClientUnitGfxInfoStrc *pGfxInfo;

    pGfxInfo = CUNIT_GfxGetUnitGfxInfo(pUnit);
    if (pGfxInfo) *(uint8_t *)&pGfxInfo->wModeLoaded[nMode] = bLoaded;
}


//1.10f: D2Client.0x6FB5F090
//  A unit's facing turned into the cel direction to draw. A facing is one of the path's 64 directions, PATH_NB_DIRECTIONS; one at 40h or above
//  (unsigned) gives one direction and cel direction 0 at once. Otherwise, for nAnimMode 0 or more, the direction count by nUnitType:
//      PLAYER   16 for the local player, 8 for any other or under gbPreloadRequired
//      MONSTER  monstats2.txt's count for the mode, through the class's monstats row; an 8 drops to 4 when GFX_GetMonsterPreloadEntry has a
//               record for the class whose bLoaded for the mode is 0, the mode not preloaded yet
//      MISSILE  missiles.txt's count for the class
//      OVERLAY  (6) overlay.txt's count, 1 for no record
//      OBJECT, ITEM, anything else: 1
//  -- a lookup that fails leaves 1 too -- stored through pnNumDirections. Then, when bMayRemap is set, a facing in a COF of 8 directions whose
//  count came out 4, or of 16 whose count came out 8, is converted down by D2CMP #10019 and scaled back into the 64 -- logical shifts both ways:
//  (facing << 3) >> 6 in, (r << 6) >> 3 out for 8 to 4, << 4 / >> 4 for 16 to 8 -- and a player in PLRMODE_DEATH or PLRMODE_DEAD, or a monster in
//  MONMODE_DEATH or MONMODE_DEAD, whose count differs from the COF's gets the facing written back into its path (#10161 PATH_SetDirection), its
//  type, class and mode first converted by #11013 (assert line 1340) from NULL defaults 6 and -1. Last, always, D2CMP #10022 turns the facing into
//  the cel direction for the COF's count, through pnCelDirection. retn 1Ch: seven arguments on the stack.
//  D2CMP's #10019 is CelConvertDir (converts 8 to 4 or 16 to 8 by one of two tables, the fourth argument choosing which) and #10022 is
//  CelGameDirToNormalDir. nClassId is the unit's class (monstats for a monster, missiles.txt for a missile, overlay.txt for an overlay) and
//  nCofDirections byte 2 of the COF header, the COF's own direction count, which is what #10022 is asked for. #11013's three outputs go through
//  dead argument slots; locals here. A NULL unit keeps type 6 through #11013 and reaches neither arm.
//  The original inlines DATATBLS_GetMonStats2TxtRecord and a missiles.txt record lookup here.
void __fastcall GFXUTIL_ResolveAnimDirection(D2UnitStrc *pUnit, D2C_UnitTypes nUnitType, int32_t nClassId, int32_t nAnimMode, int32_t *pnUnitDirection, int32_t *pnCelDirection, int32_t *pnNumDirections, int32_t nCofDirections, int32_t bMayRemap)
{
    int32_t nNumDirections;
    D2MonStats2Txt *pMonStats2Txt;
    D2ClientModePreloadStrc *pPreloadEntry;
    D2MissilesTxt *pMissilesTxt;
    D2OverlayTxt *pOverlayTxt;
    int32_t nActualDirections;
    int32_t nDirection;
    int32_t nConvType;
    int32_t nConvClass;
    int32_t nConvMode;

    if ((uint32_t)*pnUnitDirection >= PATH_NB_DIRECTIONS)
    {
        *pnNumDirections = 1;
        *pnCelDirection = 0;
        return;
    }

    nNumDirections = 1;
    if (nAnimMode >= 0)
    {
        switch (nUnitType)
        {
        case UNIT_PLAYER:
            if (gbPreloadRequired || pUnit != CUNIT_GetPlayer())
                nNumDirections = 8;
            else
                nNumDirections = 16;
            break;

        case UNIT_MONSTER:
            pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord(nClassId);
            if (pMonStats2Txt)
            {
                nNumDirections = pMonStats2Txt->nModeDirections[nAnimMode];
                if (nNumDirections == 8)
                {
                    pPreloadEntry = GFX_GetMonsterPreloadEntry(nClassId);
                    if (pPreloadEntry && !pPreloadEntry->bLoaded[nAnimMode])
                        nNumDirections = 4;
                }
            }
            break;

        case UNIT_OBJECT:
        case UNIT_ITEM:
            nNumDirections = 1;
            break;

        case UNIT_MISSILE:
            if (nClassId >= 0 && nClassId < sgptDataTables->nMissilesTxtRecordCount)
            {
                pMissilesTxt = &sgptDataTables->pMissilesTxt[nClassId];
                if (pMissilesTxt)
                    nNumDirections = pMissilesTxt->nNumDirections;
            }
            break;

        case UNIT_TYPES_COUNT:
            pOverlayTxt = GFXUTIL_GetOverlayTxtRecord(nClassId);
            if (pOverlayTxt)
                nNumDirections = pOverlayTxt->nNumDirections;
            else
                nNumDirections = 1;
            break;
        }
    }

    *pnNumDirections = nNumDirections;
    if (bMayRemap)
    {
        if (nCofDirections == 8)
        {
            if (nNumDirections == 4)
                *pnUnitDirection = (int32_t)(D2CMP_CELCMP_CelConvertDir(((uint32_t)*pnUnitDirection << 3) >> 6, nCofDirections, nNumDirections, 0) << 6 >> 3);
        }
        else if (nCofDirections == 16 && nNumDirections == 8)
        {
            *pnUnitDirection = (int32_t)(D2CMP_CELCMP_CelConvertDir(((uint32_t)*pnUnitDirection << 4) >> 6, nCofDirections, nNumDirections, 0) << 6 >> 4);
        }

        nActualDirections = *pnNumDirections;
        nDirection = *pnUnitDirection;
        nConvType = pUnit ? (int32_t)pUnit->dwUnitType : (int32_t)UNIT_TYPES_COUNT;
        nConvClass = pUnit ? pUnit->dwClassId : -1;
        nConvMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
        D2COMMON_11013_ConvertMode(pUnit, &nConvType, &nConvClass, &nConvMode, szGfxUtilCpp, 1340);
        if (nConvType == UNIT_PLAYER)
        {
            if ((nConvMode == PLRMODE_DEATH || nConvMode == PLRMODE_DEAD) && nActualDirections != nCofDirections)
                D2COMMON_PATH_SetDirection(pUnit->pDynamicPath, (uint8_t)nDirection);
        }
        else if (nConvType == UNIT_MONSTER)
        {
            if ((nConvMode == MONMODE_DEATH || nConvMode == MONMODE_DEAD) && nActualDirections != nCofDirections)
                D2COMMON_PATH_SetDirection(pUnit->pDynamicPath, (uint8_t)nDirection);
        }
    }

    *pnCelDirection = D2CMP_CELCMP_CelGameDirToNormalDir(*pnUnitDirection, nCofDirections);
}


//1.10f: D2Client.0x6FB5F330
//  How far down the screen a COF frame reaches. For each of the COF's pCofData[0] layers, re-read every pass, the component pLayerOrder names
//  (asserted below NUM_COMPONENTS, "eComponent < NUM_COMPONENTS" at line 1437, exit -1) is resolved as a component at nDirection and nFrame; for
//  each that validates to a cel, the cel's Y offset less the viewport origin's Y plus the unit's client Y is kept in *pnBottomY when larger
//  (signed). The answer is the count of layers that resolved; *pnBottomY is never seeded here. retn 10h.
//  The original inlines GFXUTIL_FillGfxData here.
int32_t __fastcall GFXUTIL_MeasureLayersBottomY(D2UnitStrc *pUnit, uint8_t *pCofData, uint8_t *pLayerOrder, int32_t *pnBottomY, int32_t nDirection, int32_t nFrame)
{
    int32_t iLayer;
    int32_t nComponent;
    int32_t nResolved;
    int32_t nClass;
    int32_t nOffsetY;
    int32_t nScreenY;
    D2GfxDataStrc tGfxData;

    iLayer = 0;
    nResolved = 0;
    if (!pCofData[0])
        return 0;

    do
    {
        nComponent = pLayerOrder[iLayer];
        if (nComponent >= (int32_t)NUM_COMPONENTS)
        {
            FOG_DisplayAssert(szComponentAssert, szGfxUtilCpp, 1437);
            exit(-1);
        }

        memset(&tGfxData, 0, sizeof(tGfxData));
        if (GFXUTIL_FillGfxData(&tGfxData, pUnit, NULL, nDirection, nFrame, &nClass, nComponent, -1) && D2CMP_SPRITECACHE_SpriteValidate(&tGfxData, FALSE, TRUE) && tGfxData.pCurrentCell)
        {
            nOffsetY = D2CMP_CELCMP_CelGetOffsetY(tGfxData.pCurrentCell);
            nOffsetY -= SCROLL_GetViewportOriginY();
            nScreenY = nOffsetY + D2COMMON_UNITS_GetClientCoordY(pUnit);
            if (nScreenY > *pnBottomY)
                *pnBottomY = nScreenY;

            ++nResolved;
        }
    }
    while (++iLayer < pCofData[0]);

    return nResolved;
}


//1.10f: D2Client.0x6FB5F420
//  The draw mode a unit's component takes from a fade or an item, written through pnTransLevel with TRUE, or FALSE with nothing written. The
//  caller seeds the slot DRAWMODE_NORMAL first, so the 1 and 2 stored here are DRAWMODE_TRANS50 and DRAWMODE_TRANS75. By the unit's drawn type:
//      PLAYER   any STAT_FADE                                   TRANS50
//      MONSTER  STAT_FADE 1..15                                 TRANS50
//               above 15: an ethereal item anywhere in its own inventory, TRANS50; none, FALSE at once -- the items.txt test below is not reached
//      then     nItemClassId's items.txt row: nTransparent set gives its nTransTbl (bytes +12Ch, +12Dh)
//      PLAYER   the drawn inventory's ethereal right- or left-hand item TRANS50, ethereal shield TRANS75; any other component FALSE
//  retn 8: pnTransLevel and nItemClassId on the stack. Every exit answers 1 or 0, and the caller keeps it.
//  The original inlines an Engine.h helper and GFXUTIL_GetUnitDisplayInventory here.
BOOL __fastcall GFXUTIL_ResolveUnitTransLevel(D2UnitStrc *pUnit, D2Composits nComponent, int32_t *pnTransLevel, int32_t nItemClassId)
{
    D2InventoryStrc *pInventory;
    D2UnitStrc *pInvItem;
    D2UnitStrc *pItem;
    D2ItemsTxt *pItemsTxt;
    int32_t nType;
    int32_t nFade;
    int32_t nConvType;
    int32_t nConvClass;
    int32_t nConvMode;

    if (!pUnit)
    {
        nType = UNIT_TYPES_COUNT;
    }
    else if (!((pUnit->dwFlagEx >> 3) & 1))
    {
        nType = (int32_t)pUnit->dwUnitType;
    }
    else
    {
        nConvType = (int32_t)pUnit->dwUnitType;
        nConvClass = pUnit->dwClassId;
        nConvMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
        D2COMMON_11013_ConvertMode(pUnit, &nConvType, &nConvClass, &nConvMode, szEngineH, 242);
        nType = nConvType;
    }

    if (nType == UNIT_PLAYER)
    {
        if (D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_FADE, 0))
        {
            *pnTransLevel = DRAWMODE_TRANS50;
            return TRUE;
        }
    }
    else if (nType == UNIT_MONSTER)
    {
        nFade = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_FADE, 0);
        if (nFade > 0)
        {
            if (nFade <= 15)
            {
                *pnTransLevel = DRAWMODE_TRANS50;
                return TRUE;
            }

            pInventory = pUnit->pInventory;
            if (!pInventory)
                return FALSE;

            for (pInvItem = D2COMMON_INVENTORY_GetFirstItem(pInventory); pInvItem; pInvItem = D2COMMON_INVENTORY_GetNextItem(pInvItem))
            {
                pItem = D2COMMON_INVENTORY_UnitIsItem(pInvItem);
                if (pItem && D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_ETHEREAL, 1510, szGfxUtilCpp))
                {
                    *pnTransLevel = DRAWMODE_TRANS50;
                    return TRUE;
                }
            }

            return FALSE;
        }
    }

    if (nItemClassId != -1)
    {
        pItemsTxt = D2COMMON_DATATBLS_GetItemsTxtRecord(nItemClassId);
        if (pItemsTxt && pItemsTxt->nTransparent)
        {
            *pnTransLevel = pItemsTxt->nTransTbl;
            return TRUE;
        }
    }

    if (nType != UNIT_PLAYER)
        return FALSE;

    pInventory = GFXUTIL_GetUnitDisplayInventory(pUnit, NULL);
    if (!pInventory)
        return FALSE;

    switch (nComponent)
    {
    case COMPOSIT_RIGHTHAND:
        pItem = D2COMMON_INVENTORY_GetCompositItem(pInventory, COMPOSIT_RIGHTHAND);
        if (!pItem || !D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_ETHEREAL, 1553, szGfxUtilCpp))
            return FALSE;

        *pnTransLevel = DRAWMODE_TRANS50;
        return TRUE;

    case COMPOSIT_LEFTHAND:
        pItem = D2COMMON_INVENTORY_GetCompositItem(pInventory, COMPOSIT_LEFTHAND);
        if (!pItem || !D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_ETHEREAL, 1564, szGfxUtilCpp))
            return FALSE;

        *pnTransLevel = DRAWMODE_TRANS50;
        return TRUE;

    case COMPOSIT_SHIELD:
        pItem = D2COMMON_INVENTORY_GetCompositItem(pInventory, COMPOSIT_SHIELD);
        if (!pItem || !D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_ETHEREAL, 1575, szGfxUtilCpp))
            return FALSE;

        *pnTransLevel = DRAWMODE_TRANS75;
        return TRUE;
    }

    return FALSE;
}


//1.10f: D2Client.0x6FB5F6B0
//  The palette a unit's component is drawn through: pDefaultPalette unless the unit draws as a PLAYER and the item in the component's composit
//  slot (or, for an armour component with nothing there, the torso's) is neither broken (IFLAG_BROKEN) nor unusable (IFLAG_NOEQUIP); then NULL
//  for an item whose alternate graphic is "lit ", else ITEMS_GetColor's palette (#10829), or the default when it answers NULL. A component of 16
//  or more (signed) or a NULL unit: the default. retn 4: pDefaultPalette on the stack. The default is a palette, never a string.
//  The original inlines an Engine.h helper and GFXUTIL_GetUnitDisplayInventory here.
uint8_t *__fastcall GFXUTIL_GetComponentColor(D2UnitStrc *pUnit, int32_t nComponent, uint8_t *pDefaultPalette)
{
    D2InventoryStrc *pInventory;
    D2UnitStrc *pItem;
    uint8_t *pPalette;
    int32_t nType;
    int32_t nConvType;
    int32_t nConvClass;
    int32_t nConvMode;
    uint8_t nColorIndex;

    if (nComponent >= (int32_t)NUM_COMPONENTS || !pUnit)
        return pDefaultPalette;

    if (!((pUnit->dwFlagEx >> 3) & 1))
    {
        nType = (int32_t)pUnit->dwUnitType;
    }
    else
    {
        nConvType = (int32_t)pUnit->dwUnitType;
        nConvClass = pUnit->dwClassId;
        nConvMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
        D2COMMON_11013_ConvertMode(pUnit, &nConvType, &nConvClass, &nConvMode, szEngineH, 242);
        nType = nConvType;
    }

    if (nType != UNIT_PLAYER)
        return pDefaultPalette;

    pInventory = GFXUTIL_GetUnitDisplayInventory(pUnit, NULL);
    if (!pInventory)
        return pDefaultPalette;

    pItem = D2COMMON_INVENTORY_GetCompositItem(pInventory, nComponent);
    if (!pItem)
    {
        if (nComponent == COMPOSIT_TORSO)
            return pDefaultPalette;

        if (!D2COMMON_COMPOSIT_IsArmorComponent((D2Composits)nComponent))
            return pDefaultPalette;

        pItem = D2COMMON_INVENTORY_GetCompositItem(pInventory, COMPOSIT_TORSO);
        if (!pItem)
            return pDefaultPalette;
    }

    if (D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_BROKEN, 1622, szGfxUtilCpp) || D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_NOEQUIP, 1623, szGfxUtilCpp))
    {
        return pDefaultPalette;
    }

    if (D2COMMON_ITEMS_GetAltGfx(pItem) == GFXUTIL_Token('l', 'i', 't', ' '))
        return NULL;

    pPalette = D2COMMON_ITEMS_GetColor(pUnit, pItem, &nColorIndex, 0);
    if (!pPalette)
        return pDefaultPalette;

    return pPalette;
}


//1.10f: D2Client.0x6FB5F7F0
//  Is this component of the unit drawn: byte +2 of its COF layer record, zero-extended (the 9-byte walk of the three layer accessors, for a byte
//  none of them returns), once the unit's COF box is found on screen at its screen position (GFX_IsUnitCofOnScreen), its weapon class resolved
//  against its drawn inventory, and its animation node found or, failing that, rebuilt (GFX_RebuildUnitGfx). FALSE when the COF box test, or both
//  the node and the rebuild, or the COF fail, for a component of 16 or more (signed), and when no layer is the component. The walk steps a byte
//  pointer by 9 and returns the byte at +2.
//  The screen position: under perspective, D2Gfx's SCALE_AdjustPerspectivePosition of the unit's precise position, moved a quarter of the screen
//  width left when the right panel shifts the view (PANEL_RIGHT) and right for the left one (PANEL_LEFT) (a signed /4, and neg for the left
//  move); otherwise the client coordinates less the viewport origin.
//  The original inlines GFXUTIL_GetUnitDisplayInventory and an Engine.h helper here.
BOOL __fastcall GFXUTIL_IsUnitComponentVisible(D2UnitStrc *pUnit, D2Composits nComponent)
{
    D2InventoryStrc *pInventory;
    uint8_t *pCofData;
    uint8_t *pLayer;
    int32_t nX;
    int32_t nY;
    int32_t nPrecisionX;
    int32_t nPrecisionY;
    int32_t nClientX;
    int32_t nClientY;
    int32_t nWeaponClass;
    int32_t nWeaponClassIdOut;
    int32_t nMode;
    int32_t nConvType;
    int32_t nConvClass;
    int32_t nConvMode;
    int32_t nLayers;
    int32_t iLayer;

    nX = 0;
    nY = 0;
    if (D2GFX_CheckPerspective())
    {
        nPrecisionY = D2COMMON_UNITS_GetPrecisionY(pUnit);
        nPrecisionX = D2COMMON_UNITS_GetPrecisionX(pUnit);
        D2GFX_SCALE_AdjustPerspectivePosition(nPrecisionX, nPrecisionY, 0, &nX, &nY);
        switch (SCROLL_GetUiShiftScreen())
        {
        case PANEL_RIGHT:
            nX += -(gnScreenWidth / 4);
            break;

        case PANEL_LEFT:
            nX += gnScreenWidth / 4;
            break;
        }
    }
    else
    {
        nClientX = D2COMMON_UNITS_GetClientCoordX(pUnit);
        nX = nClientX - SCROLL_GetViewportOriginX();
        nClientY = D2COMMON_UNITS_GetClientCoordY(pUnit);
        nY = nClientY - SCROLL_GetViewportOriginY();
    }

    if (!GFX_IsUnitCofOnScreen(pUnit, nX, nY, 0, FALSE))
        return FALSE;

    pInventory = GFXUTIL_GetUnitDisplayInventory(pUnit, NULL);
    nWeaponClass = D2COMMON_COMPOSIT_GetWeaponClassId(pUnit, pInventory, &nWeaponClassIdOut, -1, TRUE);
    if (!GFX_GetUnitAnimNode(pUnit, nWeaponClass, -1) && !GFX_RebuildUnitGfx(pUnit, nWeaponClass, -1))
        return FALSE;

    if (!((pUnit->dwFlagEx >> 3) & 1))
    {
        nMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
    }
    else
    {
        nConvType = (int32_t)pUnit->dwUnitType;
        nConvClass = pUnit->dwClassId;
        nConvMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
        D2COMMON_11013_ConvertMode(pUnit, &nConvType, &nConvClass, &nConvMode, szEngineH, 260);
        nMode = nConvMode;
    }

    pCofData = (uint8_t *)GFX_GetUnitCofData(pUnit, nMode);
    if (!pCofData || (int32_t)nComponent >= (int32_t)NUM_COMPONENTS)
        return FALSE;

    nLayers = pCofData[0];
    pLayer = pCofData + 0x1C;
    iLayer = 0;
    if (nLayers <= 0)
        return FALSE;

    while (pLayer[0] != (int32_t)nComponent)
    {
        pLayer += 9;
        if (++iLayer >= nLayers)
            return FALSE;
    }

    return pLayer[2];
}


//1.10f: D2Client.0x6FB5F9D0
//  An overlay's frame count, shifted left 8: the cel's nMaxFrames << 8, or 1 when the request does not resolve or validate. The request is an
//  overlay's: the block cleared, the entry's nOverlayId stored at +2Ch (nOverlay), then filled as GFXUTIL_FillGfxData fills it for a NULL unit,
//  at the unit's direction and the entry's nFrame, component 1. The original inlines GFXUTIL_FillGfxData here.
int32_t __fastcall GFXUTIL_GetOverlayFrameCount(D2UnitStrc *pUnit, D2ClientGfxEntryStrc *pEntry)
{
    D2GfxDataStrc tGfxData;
    int32_t nClass;
    int32_t nFrame;

    memset(&tGfxData, 0, sizeof(tGfxData));
    nFrame = pEntry->nFrame;
    tGfxData.nOverlay = pEntry->nOverlayId;
    if (!GFXUTIL_FillGfxData(&tGfxData, NULL, NULL, D2COMMON_UNITS_GetDirection(pUnit), nFrame, &nClass, 1, -1))
        return 1;

    if (!D2CMP_SPRITECACHE_SpriteValidate(&tGfxData, FALSE, TRUE))
        return 1;

    return tGfxData.nMaxFrames << 8;
}


//1.10f: D2Client.0x6FB5FA60
//  Up to three characters of a token into a four-character field, padded with spaces from the first NUL: every exit writes all four bytes, and
//  the fourth is always a space. A bare retn with both pointers in registers. void: its caller discards al.
void __fastcall GFXUTIL_CopyTokenPadded(const char *pszToken, char *pszOut)
{
    if (!pszToken[0])
    {
        pszOut[0] = ' ';
        pszOut[1] = ' ';
        pszOut[2] = ' ';
        pszOut[3] = ' ';
        return;
    }

    pszOut[0] = pszToken[0];
    if (!pszToken[1])
    {
        pszOut[1] = ' ';
        pszOut[2] = ' ';
        pszOut[3] = ' ';
        return;
    }

    pszOut[1] = pszToken[1];
    if (!pszToken[2])
    {
        pszOut[2] = ' ';
        pszOut[3] = ' ';
        return;
    }

    pszOut[2] = pszToken[2];
    pszOut[3] = ' ';
}


//1.10f: D2Client.0x6FB5FAE0
//  A uniqueitems.txt record by id, or NULL below 0 or at the count (signed); 332-byte records.
D2UniqueItemsTxt *__fastcall GFXUTIL_GetUniqueItemsTxtRecord(int32_t nUniqueItemId)
{
    if (nUniqueItemId < 0 || nUniqueItemId >= sgptDataTables->nUniqueItemsTxtRecordCount)
        return NULL;

    return &sgptDataTables->pUniqueItemsTxt[nUniqueItemId];
}

#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "CoreArchive.h"
#include "Gfx.h"

//  Gfx.cpp -- the unit graphics of D2Client.dll 1.10: a unit's overlay list ticked, cycled, drawn and freed; its animation list -- one COF node per
//  mode and weapon class code, loaded through the COF cache -- built and freed; the unit, its shadow, an object and an inventory item drawn; the
//  preload sweeps; the two palette shifts.
//  The original source file is ENGINE\Gfx.cpp (the asserts of this unit name it). Address range 0x6FB57CD0..0x6FB5DD63.
//
//  D2ClientUnitGfxInfoStrc, a unit's gfx block (60 bytes): wModeLoaded[20] at +0, a flag per mode whose COF is loaded; pAnimList at +28h;
//  pOverlayList at +2Ch; the particle block at +30h (Particle.cpp); the two palette shifts at +34h and +38h.
//  D2ClientCofNodeStrc, an animation-list node (20 bytes): the mode and the weapon class code it is keyed on, the COF cache node it was loaded
//  from (D2ClientCofCacheNodeStrc, whose pData at +14h is the COF), and pNext at +10h.
//  D2ClientGfxEntryStrc, an overlay entry (168 bytes): its kind, overlay.txt id, frame rate, frame and frame count (8.8 fixed point: `>> 8` reads
//  the frame), draw offsets, the tick it was last stepped on, the Overlay.txt PreDraw flag, an aura's state and active flag, its light, pNext at
//  +A4h (the 96 bytes before it, field_44, are cleared with the entry and touched by nothing else), and three slots whose meaning is the kind's
//  (+8 field_08, +0Ch nX, +10h nY: GFX_AddOverlay stores its nTargetOrStateId, nX and nY there for every kind but ONCE and DELAYED):
//
//      MODECHNG   field_08 the mode the overlay lives in
//      COUNTER    field_08 a count, down to 0
//      ONCE       field_08 1 while it plays (set by GFX_AddOverlay)
//      LOOPEX     field_08 and nX the two overlays that follow it
//      STOP       field_08 set to 1 when it stops on its last frame
//      DELAYED    field_08 1 while it plays; field_30 the tick it ended
//      AURA       field_08 a count up to 50 while active, -1 inactive
//      AURAEX     field_08 and nX the two auras that follow, nY their state

//  The two path literals this unit passes: its own (0x6FB96EE8) and the Engine.h inline's (0x6FB96F24).
static const char szGfxCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\ENGINE\\Gfx.cpp";
static const char szEngineH[] = "..\\D2Client/Engine.h";

//  The gfx block's two lists, the overlay entry's and the COF node's fields, overlay.txt's transparency and three enum members.
static_assert(offsetof(D2ClientUnitGfxInfoStrc, pAnimList) == 0x28 && offsetof(D2ClientUnitGfxInfoStrc, pOverlayList) == 0x2C,
              "GFX_GetUnitCofData: `mov eax, [eax+28h]` 0x6FB5832C; GFX_HasOverlay: `mov eax, [eax+2Ch]` 0x6FB57CFA");
static_assert(offsetof(D2ClientGfxEntryStrc, field_08) == 8 && offsetof(D2ClientGfxEntryStrc, nX) == 0x0C
              && offsetof(D2ClientGfxEntryStrc, nY) == 0x10 && offsetof(D2ClientGfxEntryStrc, nFrameRate) == 0x14
              && offsetof(D2ClientGfxEntryStrc, nFrameCount) == 0x1C && offsetof(D2ClientGfxEntryStrc, nOffsetX) == 0x20
              && offsetof(D2ClientGfxEntryStrc, nOffsetY) == 0x24 && offsetof(D2ClientGfxEntryStrc, nLoopWaitTime) == 0x28
              && offsetof(D2ClientGfxEntryStrc, dwLastTick) == 0x2C && offsetof(D2ClientGfxEntryStrc, field_30) == 0x30,
              "GFX_TickUnitOverlays: [esi+8] 0x6FB57F15, [esi+0Ch] 0x6FB58029, [esi+10h] 0x6FB58078, [esi+14h] 0x6FB57F63, "
              "[esi+1Ch] 0x6FB57F69, [esi+28h] 0x6FB57FBE, [esi+2Ch] 0x6FB57ECF, [esi+30h] 0x6FB57FBB; GFX_DrawOverlayAt: "
              "[esi+20h] 0x6FB57E2D, [esi+24h] 0x6FB57E2A");
static_assert(offsetof(D2ClientGfxEntryStrc, bPreDraw) == 0x34 && offsetof(D2ClientGfxEntryStrc, nStateId) == 0x38
              && offsetof(D2ClientGfxEntryStrc, bAuraActive) == 0x3C && offsetof(D2ClientGfxEntryStrc, pLight) == 0x40
              && offsetof(D2ClientGfxEntryStrc, pNext) == 0xA4,
              "GFX_DrawOverlayAt: `mov eax, [esi+34h]` 0x6FB57D55; GFX_CycleAuraOverlay: `mov eax, [ebx+38h]` 0x6FB58177, "
              "[eax+3Ch] 0x6FB581E2; GFX_RemoveGfxEntry: `mov ecx, [esi+40h]` 0x6FB582C7; GFX_HasOverlay: [eax+0A4h] 0x6FB57D06");
static_assert(offsetof(D2ClientCofNodeStrc, nAnimMode) == 0 && offsetof(D2ClientCofNodeStrc, nWeaponClassCode) == 4
              && offsetof(D2ClientCofNodeStrc, pCofFile) == 8 && offsetof(D2ClientCofNodeStrc, pNext) == 0x10,
              "GFX_GetUnitAnimNode: `cmp [eax], ecx` 0x6FB583ED, `cmp [eax+4], ebx` 0x6FB583F1, `mov eax, [eax+10h]` "
              "0x6FB583F6; GFX_GetUnitCofData: `mov eax, [eax+8]` 0x6FB58342");
static_assert(offsetof(D2OverlayTxt, nTrans) == 0x7C, "GFX_DrawOverlayAt: `mov dl, [ebp+7Ch]` 0x6FB57E36");
static_assert(OVERLAYTYPE_MODECHNG == 0 && STATE_NONE == 0,
              "GFX_TickUnitOverlays: case 0 of jpt_6FB57EF2; GFX_CycleAuraOverlay: `xor esi, esi` / `xor edi, edi` 0x6FB581F9");
static_assert(OVERLAY_BONEARMOR_CAST == 0x8C && OVERLAY_BONEARMOR_FRONT == 0x8D && OVERLAY_BONEARMOR_BACK == 0x8E,
              "GFX_TickUnitOverlays: `mov edx, 8Ch` 0x6FB580FD, `push 8Dh` 0x6FB5810B, `push 8Eh` 0x6FB58106");

//  The COF cache node's fields before pData, the cache's three header fields and its buckets, and objects.txt's mode flags.
static_assert(offsetof(D2ClientCofCacheNodeStrc, pNext) == 0 && offsetof(D2ClientCofCacheNodeStrc, dwFlags) == 4
              && offsetof(D2ClientCofCacheNodeStrc, nSize) == 8 && offsetof(D2ClientCofCacheNodeStrc, szName) == 0x0C,
              "GFX_LoadCofFile: `mov [esi], eax` 0x6FB58D82, `mov ebp, [esi+4]` 0x6FB58CAE, `lea ebp, [esi+8]` 0x6FB58CCE, "
              "`lea edx, [esi+0Ch]` 0x6FB58CB1");
static_assert(offsetof(D2ClientCofCacheStrc, bLoaded) == 0 && offsetof(D2ClientCofCacheStrc, pBlock) == 4
              && offsetof(D2ClientCofCacheStrc, nBlockSize) == 8 && offsetof(D2ClientCofCacheStrc, pBuckets) == 0x0C,
              "GFX_LoadCofFile: `mov eax, [ebx+edx*4+0Ch]` 0x6FB58C61; GFX_FreeCelCache: gpCofCachePlayerBlock 0x6FB591B9 and "
              "gnCofCachePlayerBlockSize 0x6FB591AB, 4 and 8 past gbCofCachePlayerLoaded (0x6FBC1B08)");
static_assert(offsetof(D2ObjectsTxt, nMode) == 0x13F, "GFX_LoadObjectModeGfx: `cmp byte ptr [eax+edi+13Fh], 1` 0x6FB59010");

//  objects.txt's draw offsets and Draw flag, and the state both shadow drawers refuse.
static_assert(offsetof(D2ObjectsTxt, dwXOffset) == 0x148 && offsetof(D2ObjectsTxt, dwYOffset) == 0x14C
              && offsetof(D2ObjectsTxt, nDraw) == 0x150,
              "GFX_DrawUnitCelShadow: `mov cl, [eax+150h]` 0x6FB59550, `mov edi, [eax+148h]` 0x6FB59571, "
              "`mov ebx, [eax+14Ch]` 0x6FB59577");
static_assert(STATE_INVIS == 0x92, "GFX_DrawUnitCelShadow: `push 92h` 0x6FB594B3; GFX_DrawUnitShadow: `push 92h` 0x6FB59696");

//  overlay.txt's LocalBlood.
static_assert(offsetof(D2OverlayTxt, nLocalBlood) == 0x81, "GFX_DrawUnitOverlays: `mov al, [ebp+81h]` 0x6FB5A6A2");

//  missiles.txt's Trans and LocalBlood, the special-8 component and three draw modes.
static_assert(offsetof(D2MissilesTxt, nTrans) == 0x18D && offsetof(D2MissilesTxt, nLocalBlood) == 0x1A2,
              "GFX_DrawUnitCels: `mov al, [ecx+18Dh]` 0x6FB5B011, `mov al, [ecx+1A2h]` 0x6FB5B037");
static_assert(COMPOSIT_SPECIAL8 == 0x0F && DRAWMODE_MODULATE == 3 && DRAWMODE_BURN == 4 && DRAWMODE_HIGHLIGHT == 7,
              "GFX_DrawUnitCels: `cmp ebp, 0Fh` 0x6FB5AB85, `mov esi, 3` 0x6FB5B020, `mov esi, 4` 0x6FB5B029, `mov esi, 7` 0x6FB5B032");

//  The gfx block's particle slot and the extent of its mode flags, the preload record's second half, the request's flag word.
static_assert(offsetof(D2ClientUnitGfxInfoStrc, pParticles) == 0x30,
              "GFX_GetUnitParticles: `mov eax, [eax+30h]` 0x6FB5BA49; GFX_SetUnitParticles: `mov [eax+30h], esi` 0x6FB5BA5C");
static_assert(sizeof(((D2ClientUnitGfxInfoStrc *)0)->wModeLoaded) == 20 * sizeof(uint16_t),
              "GFX_SetAllUnitModesLoaded: `cmp ecx, 14h` 0x6FB5B200 over `mov [eax+ecx*2], bl` 0x6FB5B1FC");
static_assert(offsetof(D2ClientModePreloadStrc, bQueued) == 0x50,
              "GFX_PreloadAnimMode: `mov ebx, [edx+ecx*4+50h]` 0x6FB5B99F");
static_assert(offsetof(D2GfxDataStrc, fFlags) == 0x18,
              "GFX_AsyncLoadBlock: `test byte ptr [esp+58h], 4` 0x6FB5B8B4, the request at [esp+40h] (pCellFile `mov [esp+44h], edi` 0x6FB5B8A6)");

//  The overlay.txt columns GFX_AddOverlay reads, the Hand of God overlay and monstats2.txt's NoOvly flag.
static_assert(offsetof(D2OverlayTxt, wVersion) == 0x42 && offsetof(D2OverlayTxt, nPreDraw) == 0x48
              && offsetof(D2OverlayTxt, dwOffsetX) == 0x54 && offsetof(D2OverlayTxt, dwOffsetY) == 0x58
              && offsetof(D2OverlayTxt, dwHeight) == 0x5C && offsetof(D2OverlayTxt, dwAnimRate) == 0x6C
              && offsetof(D2OverlayTxt, dwInitRadius) == 0x70 && offsetof(D2OverlayTxt, dwRadius) == 0x74
              && offsetof(D2OverlayTxt, dwLoopWaitTime) == 0x78 && offsetof(D2OverlayTxt, nRed) == 0x7D
              && offsetof(D2OverlayTxt, nGreen) == 0x7E && offsetof(D2OverlayTxt, nBlue) == 0x7F,
              "GFX_AddOverlay: `cmp word ptr [ebp+42h], 64h` 0x6FB5BAC3, `mov dl, [ebp+48h]` 0x6FB5BBC2, [ebp+54h] 0x6FB5BCAA, "
              "[ebp+58h] 0x6FB5BCB4, [ebp+5Ch] 0x6FB5BCD5, [ebp+6Ch] 0x6FB5BE13, [ebp+70h] 0x6FB5BD0D, [ebp+74h] 0x6FB5BCFC, "
              "[ebp+78h] 0x6FB5BE6D, [ebp+7Dh] 0x6FB5BD09, [ebp+7Eh] 0x6FB5BD06, [ebp+7Fh] 0x6FB5BD03");
static_assert(OVERLAY_HANDOFGOD == 0x9E && MONSTATS2FLAGINDEX_NOOVLY == 2,
              "GFX_AddOverlay: `cmp esi, 9Eh` 0x6FB5BBB7; `and edx, [eax+8]`, gdwBitMasks[2], 0x6FB5BB50");

//  The COF header's box, whose X and Y pairs a transcription could cross -- GFX_IsUnitCofOnScreen moves an X edge by the Y pair.
static_assert(offsetof(D2CofHeaderStrc, nXMin) == 0x08 && offsetof(D2CofHeaderStrc, nXMax) == 0x0C
              && offsetof(D2CofHeaderStrc, nYMin) == 0x10 && offsetof(D2CofHeaderStrc, nYMax) == 0x14,
              "GFX_HitTestUnitCof: `mov ecx, [eax+8]` 0x6FB5C786, `mov edx, [eax+0Ch]` 0x6FB5C795, `mov edx, [eax+10h]` 0x6FB5C7A0, "
              "`mov eax, [eax+14h]` 0x6FB5C7B3; GFX_IsUnitCofOnScreen's shift `mov eax, [esi+14h]` / `mov edx, [esi+10h]` 0x6FB5C97B");

//  The monster COF token types, the act block's node and its COF's direction count, and monstats2.txt's BaseW.
static_assert(offsetof(D2ClientMonCofTokensStrc, nCofDirections) == 3 && offsetof(D2ClientMonCofTokensStrc, nVariants) == 4
              && offsetof(D2ClientMonModeTokenOverrideStrc, dwToken) == 0 && offsetof(D2ClientMonModeTokenOverrideStrc, nMonMode) == 4,
              "GFX_AddMonsterCofFiles: `mov al, [esp+304h+tMonTokens.nCofDirections]` 0x6FB5CE2C; GFXUTIL_FillCelContextTokens: "
              "`mov cl, [ecx+edi+4]` 0x6FB5EDFA; GFX_BuildMonsterCofName: `cmp edi, [eax*8+6FB96E84h]` 0x6FB5D19C, `mov ecx, "
              "[eax*8+6FB96E80h]` 0x6FB5D1A5");
static_assert(offsetof(D2ClientCofCacheNodeStrc, unk0x18) == 0x18 && offsetof(D2CofHeaderStrc, nDirections) == 2
              && offsetof(D2MonStats2Txt, nBaseW) == 0x10,
              "GFX_LoadActCofData: `add ebp, 18h` 0x6FB5CA9F, `cmp byte ptr [ebp+1Ah], 40h` 0x6FB5CA92; GFX_BuildMonsterCofName: "
              "`mov eax, [esi+10h]` 0x6FB5D163");

//  The two COF caches (gCofCachePlayer and gCofCacheOther, Gfx.h), each a D2ClientCofCacheStrc: the bytes treat each as one block -- `rep stosd`
//  over 203h dwords, the cache base in a register and the buckets at +0Ch. Two accessors of ours, not the original's.
static inline D2ClientCofCacheStrc *GFX_PlayerCofCache()
{
    return &gCofCachePlayer;
}

static inline D2ClientCofCacheStrc *GFX_OtherCofCache()
{
    return &gCofCacheOther;
}

//  The eight-character name compare the cache walks make: two dwords, the title's against the node's szName (`cmp edx, [eax+0Ch]` /
//  `cmp ecx, [eax+10h]`). A helper of ours, not the original's.
static inline BOOL GFX_CofNameEquals(const char *szTitle, const D2ClientCofCacheNodeStrc *pNode)
{
    return *(const uint32_t *)&szTitle[0] == *(const uint32_t *)&pNode->szName[0] && *(const uint32_t *)&szTitle[4] == *(const uint32_t *)&pNode->szName[4];
}


//1.10f: D2Client.0x6FB57CD0
//  Does the unit carry this overlay: the overlay list walked for an entry whose nOverlayId matches, TRUE at the first, FALSE at the end. No gfx
//  block asserts `ptGfxInfo` at line 492 and exits. BOOL: the two exits are `xor eax, eax` (0x6FB57D10) and `mov eax, 1` (0x6FB57D14).
BOOL __fastcall GFX_HasOverlay(D2UnitStrc *pUnit, int32_t nOverlayId)
{
    D2ClientUnitGfxInfoStrc *pGfxInfo;
    D2ClientGfxEntryStrc *pEntry;

    pGfxInfo = CUNIT_GfxGetUnitGfxInfo(pUnit);
    if (!pGfxInfo)
    {
        FOG_DisplayAssert((uint32_t)"ptGfxInfo", (uint32_t)szGfxCpp, 492);
        exit(-1);
    }

    for (pEntry = pGfxInfo->pOverlayList; pEntry; pEntry = pEntry->pNext)
    {
        if (pEntry->nOverlayId == nOverlayId)
            return TRUE;
    }

    return FALSE;
}


//1.10f: D2Client.0x6FB57D20
//  Draws every entry of one overlay at a screen position, not at the unit: the cel at nX and nY plus the entry's draw offsets, through D2Gfx's
//  TEXTURE_CelDraw with gamma 0xFFFFFF80 and overlay.txt's Trans as the draw mode, while the entry's frame (`>> 8`, signed -- `sar ebx, 8`
//  0x6FB57DA9) is below its frame count (`sar ecx, 8` / `jge`, 0x6FB57E23). The request is an overlay's: the block cleared, nOverlay set,
//  GFXUTIL_FillGfxData for a NULL unit (`xor edx, edx` 0x6FB57E0E) at the unit's direction, the frame, component 1, mode -1. An overlay id out
//  of overlay.txt's range, or a request that does not resolve, returns from the whole function, the entries after it not drawn. No gfx block
//  or an empty list: nothing. `retn 8`: nX and nY on the stack. The original inlines GFXUTIL_GetOverlayTxtRecord here.
//  An entry is drawn in a pass only if its PreDraw flag fits the pass -- pass 0 takes the entries without it, pass 1 those with it -- and not
//  as a DELAYED entry that is not playing (field_08 0) or an AURA entry that is not active.
void __fastcall GFX_DrawOverlayAt(D2UnitStrc *pUnit, int32_t nOverlayId, int32_t nX, int32_t nY)
{
    D2ClientUnitGfxInfoStrc *pGfxInfo;
    D2ClientGfxEntryStrc *pEntry;
    D2OverlayTxt *pOverlayTxt;
    D2GfxDataStrc tGfxData;
    int32_t nPass;
    int32_t bDraw;
    int32_t nFrame;
    int32_t nClass;

    pGfxInfo = CUNIT_GfxGetUnitGfxInfo(pUnit);
    if (!pGfxInfo)
        return;

    pEntry = pGfxInfo->pOverlayList;
    if (!pEntry)
        return;

    for (nPass = 0; nPass < 2; ++nPass)
    {
        //  BUG: pEntry is not reset between the two passes: pass 0 leaves it NULL at the end of the list (`jnz loc_6FB57D55` 0x6FB57E5E falls
        //  through) and pass 1 starts with `test esi, esi` / `jz` (0x6FB57D4D) on it, so no entry with PreDraw set is ever drawn. UNREACHABLE: the
        //  one caller, UIPANELS_DrawChargeUpIcons, is gated by a global that is zero in a stock client.
        for (; pEntry; pEntry = pEntry->pNext)
        {
            bDraw = TRUE;
            if (pEntry->bPreDraw && nPass == 0)
                bDraw = FALSE;
            if (!pEntry->bPreDraw && nPass != 0)
                bDraw = FALSE;
            if (pEntry->nKind == OVERLAYTYPE_DELAYED && !pEntry->field_08)
                bDraw = FALSE;
            if (pEntry->nKind == OVERLAYTYPE_AURA && !pEntry->bAuraActive)
                bDraw = FALSE;

            if (pEntry->nOverlayId != nOverlayId || !bDraw)
                continue;

            nFrame = pEntry->nFrame >> 8;
            pOverlayTxt = GFXUTIL_GetOverlayTxtRecord(pEntry->nOverlayId);
            if (!pOverlayTxt)
                return;

            memset(&tGfxData, 0, sizeof(tGfxData));
            tGfxData.nOverlay = nOverlayId;
            if (!GFXUTIL_FillGfxData(&tGfxData, NULL, NULL, D2COMMON_UNITS_GetDirection(pUnit), nFrame, &nClass, 1, -1))
                return;

            if (nFrame < pEntry->nFrameCount >> 8)
                D2GFX_TEXTURE_CelDraw(&tGfxData, nX + pEntry->nOffsetX, nY + pEntry->nOffsetY, 0xFFFFFF80, pOverlayTxt->nTrans, NULL);
        }
    }
}


//1.10f: D2Client.0x6FB57E80
//  Steps one unit's overlays one tick, from UNITMODE_UpdateUnit. No gfx block asserts `ptGfxInfo` at line 768 and exits. The original inlines
//  Engine.h's drawn-mode conversion here.
//  The walk restarts from the head after every entry: each pass looks for the first entry not yet stamped with this GetTickCount, stamps it
//  (dwLastTick, +2Ch) and steps it, then goes back to the head (0x6FB57EBE, the target of every exit of both switches) -- so an entry removed,
//  or added by GFX_AddOverlay, never leaves the walk holding a freed pointer. It ends when every entry carries the tick.
//
//  The step, by kind (any kind but 0, 1, 2, 6 and 8 goes straight to the frame step, `cmp eax, 8 / ja` 0x6FB57EED):
//
//      MODECHNG   removed unless the unit's drawn mode (Engine.h:260) is field_08
//      COUNTER    removed if field_08 was 0; field_08 counted down either way
//      ONCE       removed once field_08 is 0
//      DELAYED    not playing: back to the head until nLoopWaitTime ms have passed since field_30 (`jb`, UNSIGNED, 0x6FB57FC5),
//                 then playing (field_08 1)
//      AURA       removed if the unit has left nStateId; when active, field_08 counted up to 50 (`jl`, signed) and then the aura
//                 handed on (GFX_CycleAuraOverlay)
//
//  then nFrame += nFrameRate, and at the frame count (`jl`, signed, 0x6FB57F75) the end, by kind (on kind - 1, `ja` for 0):
//
//      COUNTER, LOOP, AURA  wrapped (nFrame -= nFrameCount)
//      ONCE                 field_08 0 -- removed next tick
//      LOOPEX               removed; field_08 and nX added as LOOP overlays, each unless -1
//      STOP                 held on its last frame, nFrameRate 0, field_08 1
//      DELAYED              wrapped, not playing, field_30 the tick
//      BONEARMOR            removed; OVERLAY_BONEARMOR_CAST added as LOOPEX, with BONEARMOR_FRONT and BONEARMOR_BACK to follow
//      AURAEX               removed; if the unit is still in nY, field_08 and nX added as AURA overlays of state nY, each unless -1
//
//  +2Ch is the stamp that skips an entry already stepped, and kind 3 is only LOOPEX's replacements. Nothing here reads an entry after
//  GFX_RemoveGfxEntry has freed it.
void __fastcall GFX_TickUnitOverlays(D2UnitStrc *pUnit)
{
    D2ClientUnitGfxInfoStrc *pGfxInfo;
    D2ClientGfxEntryStrc *pEntry;
    uint32_t dwTick;
    BOOL bKeep;
    int32_t nMode;
    int32_t nConvType;
    int32_t nConvClass;
    int32_t nConvMode;
    int32_t nFrame;
    int32_t nFrameCount;
    int32_t nNextOverlay;
    int32_t nNextOverlay2;
    int32_t nState;

    pGfxInfo = CUNIT_GfxGetUnitGfxInfo(pUnit);
    if (!pGfxInfo)
    {
        FOG_DisplayAssert((uint32_t)"ptGfxInfo", (uint32_t)szGfxCpp, 768);
        exit(-1);
    }

    dwTick = GetTickCount();
    for (;;)
    {
        for (pEntry = pGfxInfo->pOverlayList; pEntry && pEntry->dwLastTick == dwTick; pEntry = pEntry->pNext)
            ;

        if (!pEntry)
            return;

        pEntry->dwLastTick = dwTick;
        switch (pEntry->nKind)
        {
        case OVERLAYTYPE_MODECHNG:
            if (pUnit && ((pUnit->dwFlagEx >> 3) & 1))
            {
                nConvType = (int32_t)pUnit->dwUnitType;
                nConvClass = (int32_t)pUnit->dwClassId;
                nConvMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
                D2COMMON_11013_ConvertMode(pUnit, &nConvType, &nConvClass, &nConvMode, szEngineH, 260);
                nMode = nConvMode;
            }
            else
            {
                nMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
            }

            if (nMode != pEntry->field_08)
            {
                GFX_RemoveGfxEntry(pUnit, pEntry);
                continue;
            }
            break;

        case OVERLAYTYPE_COUNTER:
            bKeep = pEntry->field_08 != 0;
            --pEntry->field_08;
            if (!bKeep)
            {
                GFX_RemoveGfxEntry(pUnit, pEntry);
                continue;
            }
            break;

        case OVERLAYTYPE_ONCE:
            if (!pEntry->field_08)
            {
                GFX_RemoveGfxEntry(pUnit, pEntry);
                continue;
            }
            break;

        case OVERLAYTYPE_DELAYED:
            if (!pEntry->field_08)
            {
                if (GetTickCount() - (uint32_t)pEntry->field_30 < (uint32_t)pEntry->nLoopWaitTime)
                    continue;

                pEntry->field_08 = 1;
            }
            break;

        case OVERLAYTYPE_AURA:
            if (!D2COMMON_STATES_CheckState(pUnit, (D2C_States)pEntry->nStateId))
            {
                GFX_RemoveGfxEntry(pUnit, pEntry);
                continue;
            }

            if (pEntry->bAuraActive)
            {
                if (pEntry->field_08 < 50)
                    ++pEntry->field_08;
                else
                    GFX_CycleAuraOverlay(pUnit, pGfxInfo, pEntry);
            }
            break;
        }

        nFrameCount = pEntry->nFrameCount;
        nFrame = pEntry->nFrameRate + pEntry->nFrame;
        pEntry->nFrame = nFrame;
        if (nFrame < nFrameCount)
            continue;

        switch (pEntry->nKind)
        {
        case OVERLAYTYPE_COUNTER:
        case OVERLAYTYPE_LOOP:
        case OVERLAYTYPE_AURA:
            pEntry->nFrame = nFrame - nFrameCount;
            break;

        case OVERLAYTYPE_ONCE:
            pEntry->field_08 = 0;
            break;

        case OVERLAYTYPE_LOOPEX:
            nNextOverlay = pEntry->field_08;
            nNextOverlay2 = pEntry->nX;
            GFX_RemoveGfxEntry(pUnit, pEntry);
            if (nNextOverlay != -1)
                GFX_AddOverlay(pUnit, (D2_OverlayIds)nNextOverlay, OVERLAYTYPE_LOOP, 0, 0, 0, 0, 0);
            if (nNextOverlay2 != -1)
                GFX_AddOverlay(pUnit, (D2_OverlayIds)nNextOverlay2, OVERLAYTYPE_LOOP, 0, 0, 0, 0, 0);
            break;

        case OVERLAYTYPE_STOP:
            pEntry->nFrameRate = 0;
            pEntry->nFrame = nFrameCount - 1;
            pEntry->field_08 = 1;
            break;

        case OVERLAYTYPE_DELAYED:
            pEntry->field_08 = 0;
            pEntry->nFrame = nFrame - nFrameCount;
            pEntry->field_30 = (int32_t)GetTickCount();
            break;

        case OVERLAYTYPE_BONEARMOR:
            GFX_RemoveGfxEntry(pUnit, pEntry);
            GFX_AddOverlay(pUnit, OVERLAY_BONEARMOR_CAST, OVERLAYTYPE_LOOPEX, 0, OVERLAY_BONEARMOR_FRONT, OVERLAY_BONEARMOR_BACK, 0, 0);
            break;

        case OVERLAYTYPE_AURAEX:
            nNextOverlay2 = pEntry->nX;
            nNextOverlay = pEntry->field_08;
            nState = pEntry->nY;
            GFX_RemoveGfxEntry(pUnit, pEntry);
            if (D2COMMON_STATES_CheckState(pUnit, (D2C_States)nState))
            {
                if (nNextOverlay != -1)
                    GFX_AddOverlay(pUnit, (D2_OverlayIds)nNextOverlay, OVERLAYTYPE_AURA, 0, nState, 0, 0, 0);
                if (nNextOverlay2 != -1)
                    GFX_AddOverlay(pUnit, (D2_OverlayIds)nNextOverlay2, OVERLAYTYPE_AURA, 0, nState, 0, 0, 0);
            }
            break;
        }
    }
}


//1.10f: D2Client.0x6FB58170
//  The visible aura handed on. A unit standing in several auras carries an AURA entry for each and draws one at a time; this deactivates every
//  AURA entry of pEntry's state (bAuraActive 0, field_08 -1) and activates those of the next state up -- the lowest nStateId above it, or the
//  lowest of all when none is above (both compares signed, `jge`/`jle` 0x6FB58211, 0x6FB58218) -- with bAuraActive 1 and field_08 0.
//  STATE_NONE, 0, is the "none yet" of both running minimums. `retn 4`: pEntry on the stack.
//  The state is probed first: when the unit has it, it is toggled off, and STATES_CheckStateMaskAuraOnUnit asked whether any aura is left;
//  none clears pEntry's field_08 (`mov [ebx+8], eax`, eax the 0 answered, 0x6FB581A7). Then the state is toggled back on -- twice on that path:
//  `push 1` / call at 0x6FB581B2 and again at 0x6FB581C5 after `jmp short loc_6FB581C1`. D2Common's STATES_ToggleState (#10486) sets the
//  state's bit and refreshes the unit's room, so the second call repeats the refresh and changes nothing.
//  void: the last walk leaves its cursor NULL in eax at every exit (`mov eax, ebp` / `test` / `jz` 0x6FB58236, or the walk's end), and neither
//  caller reads it.
void __fastcall GFX_CycleAuraOverlay(D2UnitStrc *pUnit, D2ClientUnitGfxInfoStrc *pGfxInfo, D2ClientGfxEntryStrc *pEntry)
{
    D2ClientGfxEntryStrc *pList;
    D2ClientGfxEntryStrc *pOverlay;
    BOOL bHasState;
    int32_t nState;
    int32_t nStateId;
    int32_t nNextState;
    int32_t nLowestState;

    nState = pEntry->nStateId;
    bHasState = D2COMMON_STATES_CheckState(pUnit, (D2C_States)nState);
    if (bHasState)
        D2COMMON_STATES_ToggleState(pUnit, (D2C_States)nState, FALSE);

    if (!D2COMMON_STATES_CheckStateMaskAuraOnUnit(pUnit))
    {
        pEntry->field_08 = 0;
        if (bHasState)
            D2COMMON_STATES_ToggleState(pUnit, (D2C_States)nState, TRUE);
    }

    if (bHasState)
        D2COMMON_STATES_ToggleState(pUnit, (D2C_States)nState, TRUE);

    for (pOverlay = pGfxInfo->pOverlayList; pOverlay; pOverlay = pOverlay->pNext)
    {
        if (pOverlay->nStateId == nState && pOverlay->nKind == OVERLAYTYPE_AURA)
        {
            pOverlay->bAuraActive = FALSE;
            pOverlay->field_08 = -1;
        }
    }

    pList = pGfxInfo->pOverlayList;
    nNextState = STATE_NONE;
    nLowestState = STATE_NONE;
    for (pOverlay = pList; pOverlay; pOverlay = pOverlay->pNext)
    {
        if (pOverlay->nKind != OVERLAYTYPE_AURA)
            continue;

        nStateId = pOverlay->nStateId;
        if ((nNextState == STATE_NONE || nStateId < nNextState) && nStateId > nState)
            nNextState = nStateId;
        if (nLowestState == STATE_NONE || nStateId < nLowestState)
            nLowestState = nStateId;
    }

    if (nNextState == STATE_NONE)
        nNextState = nLowestState;

    for (pOverlay = pList; pOverlay; pOverlay = pOverlay->pNext)
    {
        if (pOverlay->nStateId == nNextState && pOverlay->nKind == OVERLAYTYPE_AURA)
        {
            pOverlay->bAuraActive = TRUE;
            pOverlay->field_08 = 0;
        }
    }
}


//1.10f: D2Client.0x6FB58270
//  Takes one overlay entry off the unit and frees it: found on the list by its address (the trailing pointer kept for the unlink), its light
//  removed (DLIGHTMAP_RemoveLight) and cleared, the entry unlinked -- from the head or from the one before it -- then, for an AURA entry, the aura
//  handed on (GFX_CycleAuraOverlay, after the unlink), and FOG_Free(entry, this file, 483, 0). An entry not on the list, or an empty list:
//  nothing. No gfx block asserts `ptGfxInfo` at line 449 and exits.
//  void: eax is the gfx block on the two refusals and FOG_Free's answer after a removal, and no caller reads it. The `test esi, esi` after the
//  walk finds the entry (0x6FB582C3) can never fail -- a NULL cursor has already returned -- and is the source's test, kept. The entry's pLight
//  is a D2ClientLightStrc *: GFX_AddOverlay stores DLIGHTMAP_AddLight's answer there (`mov [esi+40h], eax` 0x6FB5BD24).
void __fastcall GFX_RemoveGfxEntry(D2UnitStrc *pUnit, D2ClientGfxEntryStrc *pEntry)
{
    D2ClientUnitGfxInfoStrc *pGfxInfo;
    D2ClientGfxEntryStrc *pCur;
    D2ClientGfxEntryStrc *pPrev;

    pGfxInfo = CUNIT_GfxGetUnitGfxInfo(pUnit);
    if (!pGfxInfo)
    {
        FOG_DisplayAssert((uint32_t)"ptGfxInfo", (uint32_t)szGfxCpp, 449);
        exit(-1);
    }

    pPrev = NULL;
    for (pCur = pGfxInfo->pOverlayList; pCur && pCur != pEntry; pCur = pCur->pNext)
        pPrev = pCur;

    if (!pCur)
        return;

    if (pCur->pLight)
    {
        DLIGHTMAP_RemoveLight(pCur->pLight);
        pCur->pLight = NULL;
    }

    if (pPrev)
        pPrev->pNext = pCur->pNext;
    else
        pGfxInfo->pOverlayList = pCur->pNext;

    if (pCur->nKind == OVERLAYTYPE_AURA)
        GFX_CycleAuraOverlay(pUnit, pGfxInfo, pCur);

    FOG_Free((uint32_t)pCur, (uint32_t)szGfxCpp, 483, 0);
}


//1.10f: D2Client.0x6FB58320
//  The COF of the unit's node for a mode: the animation list walked for the first node whose mode is nAnimMode, whatever its weapon class code,
//  and that node's COF -- pCofFile->pData, the cache node's +14h (`mov eax, [eax+8]` / `mov eax, [eax+14h]`, 0x6FB58342) -- or NULL for no gfx
//  block or no such node. GFX_GetUnitAnimNode keys on the weapon class code too. pCofFile is read with no test.
void *__fastcall GFX_GetUnitCofData(D2UnitStrc *pUnit, int32_t nAnimMode)
{
    D2ClientUnitGfxInfoStrc *pGfxInfo;
    D2ClientCofNodeStrc *pNode;

    pGfxInfo = CUNIT_GfxGetUnitGfxInfo(pUnit);
    if (!pGfxInfo)
        return NULL;

    for (pNode = pGfxInfo->pAnimList; pNode; pNode = pNode->pNext)
    {
        if (pNode->nAnimMode == nAnimMode)
            return pNode->pCofFile->pData;
    }

    return NULL;
}


//1.10f: D2Client.0x6FB58350
//  The unit's animation node for a weapon class code and a mode: the first node keyed on both, or NULL. The code is the four-character token
//  D2Common's COMPOSIT_GetWeaponClassId (#10888) answers, as the node's nWeaponClassCode names it. nAnimMode -1 means the unit's drawn mode -- its
//  anim-or-sequence mode, converted by D2Common's #11013 for a shapeshifted unit (Engine.h:260). No gfx block asserts `ptGfxInfo` at line 942
//  and exits. `retn 4`: nAnimMode on the stack. The original inlines Engine.h's drawn-mode conversion here.
D2ClientCofNodeStrc *__fastcall GFX_GetUnitAnimNode(D2UnitStrc *pUnit, int32_t nWeaponClassCode, int32_t nAnimMode)
{
    D2ClientUnitGfxInfoStrc *pGfxInfo;
    D2ClientCofNodeStrc *pNode;
    int32_t nConvType;
    int32_t nConvClass;
    int32_t nConvMode;

    pGfxInfo = CUNIT_GfxGetUnitGfxInfo(pUnit);
    if (!pGfxInfo)
    {
        FOG_DisplayAssert((uint32_t)"ptGfxInfo", (uint32_t)szGfxCpp, 942);
        exit(-1);
    }

    if (nAnimMode == -1)
    {
        if (pUnit && ((pUnit->dwFlagEx >> 3) & 1))
        {
            nConvType = (int32_t)pUnit->dwUnitType;
            nConvClass = (int32_t)pUnit->dwClassId;
            nConvMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
            D2COMMON_11013_ConvertMode(pUnit, &nConvType, &nConvClass, &nConvMode, szEngineH, 260);
            nAnimMode = nConvMode;
        }
        else
        {
            nAnimMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
        }
    }

    for (pNode = pGfxInfo->pAnimList; pNode; pNode = pNode->pNext)
    {
        if (pNode->nAnimMode == nAnimMode && pNode->nWeaponClassCode == nWeaponClassCode)
            return pNode;
    }

    return NULL;
}


//1.10f: D2Client.0x6FB58410
//  Loads a mode's graphics and answers the unit's animation node: the unit's type, class and mode converted (#11013, line 1280 of this file);
//  nAnimMode -1 takes the converted mode. Then by the converted type:
//
//      MONSTER  the mode loaded if monstats2.txt has it (Gfx.cpp:1175, below), and the node for (nWeaponClassCode, the mode)
//      PLAYER   the mode's COF node got or built (GFX_GetUnitCofNode) and, when it is, the mode marked loaded (GFXUTIL_SetUnitModeLoaded);
//               then the node for (nWeaponClassCode, the DRAWN mode)
//      OBJECT   the mode loaded (GFX_LoadObjectModeGfx); then the node for (nWeaponClassCode, -1, the drawn mode)
//      other    NULL
//
//  `retn 4`: nAnimMode on the stack. The callers fall back on this when their lookup misses. Seven pass nAnimMode -1 (`push -1`);
//  GFX_AsyncLoadBlock passes the mode it preloads (`push ebx` 0x6FB586A6) and GFXUTIL_ResolveUnitAnimIdentity its request's mode (`push edx`,
//  [esi+28h], 0x6FB5E472). The anim list is freed only by a monster's MONMODE_DEAD. The slot of nAnimMode is reused as
//  GFX_GetUnitCofNode's out-parameter (`mov [esp+2Ch+pMode], eax` 0x6FB58615), so the converted mode lives in the local nResolvedMode
//  and the mode being loaded in nAnimMode.
//  The original inlines Engine.h's drawn-mode conversion, a monster's mode loaded (the conversion at line 1175 of this file),
//  GFX_SetAllUnitModesLoaded and GFX_GetUnitAnimNode here.
D2ClientCofNodeStrc *__fastcall GFX_RebuildUnitGfx(D2UnitStrc *pUnit, int32_t nWeaponClassCode, int32_t nAnimMode)
{
    D2MonStats2Txt *pMonStats2Txt;
    void *pCofData;
    int32_t nType;
    int32_t nClass;
    int32_t nUnitMode;
    int32_t nMode;
    int32_t nLoadMode;
    int32_t nMonType;
    int32_t nMonClass;
    int32_t nMonMode;

    nType = pUnit ? (int32_t)pUnit->dwUnitType : (int32_t)UNIT_TYPES_COUNT;
    nClass = pUnit ? (int32_t)pUnit->dwClassId : -1;
    nUnitMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
    D2COMMON_11013_ConvertMode(pUnit, &nType, &nClass, &nUnitMode, szGfxCpp, 1280);

    nMode = nAnimMode;
    if (nMode == -1)
        nMode = nUnitMode;

    if (nType == UNIT_MONSTER)
    {
        nLoadMode = nMode;
        nMonType = pUnit ? (int32_t)pUnit->dwUnitType : (int32_t)UNIT_TYPES_COUNT;
        nMonClass = pUnit ? (int32_t)pUnit->dwClassId : -1;
        nMonMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
        D2COMMON_11013_ConvertMode(pUnit, &nMonType, &nMonClass, &nMonMode, szGfxCpp, 1175);
        if (nMode == MONMODE_SEQUENCE)
            nLoadMode = nMonMode;

        pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord(nMonClass);
        if (pMonStats2Txt && (gdwBitMasks[nLoadMode & 7] & pMonStats2Txt->nModeFlags[nLoadMode >> 3]))
        {
            if (nLoadMode == MONMODE_DEAD)
            {
                GFX_FreeUnitAnimList(pUnit);
                GFX_SetAllUnitModesLoaded(pUnit, 0);
                nLoadMode = MONMODE_DEAD;
            }
            else if (nLoadMode == MONMODE_SEQUENCE)
            {
                nLoadMode = nMonMode;
            }

            GFX_LoadMonsterModeGfx(pUnit, nLoadMode);
        }

        return GFX_GetUnitAnimNode(pUnit, nWeaponClassCode, nMode);
    }

    if (nType == UNIT_PLAYER)
    {
        pCofData = NULL;
        if (GFX_GetUnitCofNode(pUnit, &pCofData, nMode))
            GFXUTIL_SetUnitModeLoaded(pUnit, nMode, 1);

        //  BUG: with a mode given, the PLAYER and OBJECT arms load that mode and return the node of the unit's current drawn mode -- the inlined
        //  lookup
        //  resolves its mode with no -1 test, a constant -1 (0x6FB58658..0x6FB586B8), and the object arm passes `push 0FFFFFFFFh` (0x6FB58705) --
        //  where the MONSTER arm returns the given mode's. NOT ESTABLISHED: a mode comes only from GFX_AsyncLoadBlock, for the preload sweeps'
        //  monsters
        //  and players (the object arm with a mode is never taken), and both consumers test the answer for NULL only; whether a player's drawn mode
        //  can
        //  lack its node then was not looked for.
        return GFX_GetUnitAnimNode(pUnit, nWeaponClassCode, -1);
    }

    if (nType == UNIT_OBJECT)
    {
        GFX_LoadObjectModeGfx(pUnit, nMode);
        return GFX_GetUnitAnimNode(pUnit, nWeaponClassCode, -1);
    }

    return NULL;
}


//1.10f: D2Client.0x6FB58720
//  A mode's COF node found or built, and the COF answered. NULL for a unit GFXUTIL_IsDrawableUnitType refuses, or when D2Common's #10885 builds
//  no COF path for the unit, its drawn inventory and the mode (the path, and the weapon class code through its out-parameter). Then:
//
//    * the first node of the mode (-1: the drawn mode), keyed on the mode alone; if its weapon class code is the path's, nothing to build;
//    * a node of the mode with another code is replaced: its COF handed back through ppReplacedCofData (GFX_GetUnitCofData), the node freed by
//      its cache node (GFX_FreeUnitAnimNodeByCof, asserted "0", line 1032);
//    * a new node -- FOG_Alloc(20, this file, 1078, 0), asserted `ptUnitCofNode` at 1079, its five fields zeroed -- gets the COF from the
//      player cache for a unit drawn as a PLAYER, from the other cache for any other, found there by the path's lower-cased title
//      (GFX_ExtractFileTitle, GFX_HashCofName, the eight-character compare) or loaded into it (GFX_LoadCofFile); then the mode, the code and
//      the cache node are stored and the node pushed on the list;
//
//  and the answer is the COF of the first node whose mode is nAnimMode -- the argument as passed, not the drawn mode -- or NULL. `retn 4`:
//  nAnimMode on the stack. The return is the COF (the node's pCofFile->pData, `mov eax, [edx+14h]` 0x6FB58AAA), not a node. ppReplacedCofData
//  is written only when a node of the mode is replaced, with the replaced node's COF (0x6FB588D0); no caller reads it: each hands the address
//  of a dead slot.
//  nAnimMode -1 is resolved for the first walk only; the node is stored with, and the last walk looks for, the -1 itself (`mov ecx, [esp+234h+
//  nAnimMode]` / `mov [ebp+0], ecx` 0x6FB58A52, `cmp [eax], ebx` 0x6FB58A8C). No caller passes -1, so the -1 arm is dead in this build.
//  The original inlines GFXUTIL_GetUnitDisplayInventory, Engine.h's drawn-mode and drawn-type conversions and GFX_HashCofName here.
void *__fastcall GFX_GetUnitCofNode(D2UnitStrc *pUnit, void **ppReplacedCofData, int32_t nAnimMode)
{
    D2ClientUnitGfxInfoStrc *pGfxInfo;
    D2ClientUnitGfxInfoStrc *pGfxInfo2;
    D2ClientCofNodeStrc *pNode;
    D2ClientCofNodeStrc *pNewNode;
    D2ClientCofCacheStrc *pCache;
    D2ClientCofCacheNodeStrc *pCofFile;
    D2InventoryStrc *pInventory;
    uint32_t nBucket;
    int32_t nWeaponClassCode;
    int32_t nMode;
    int32_t nType;
    int32_t nConvType;
    int32_t nConvClass;
    int32_t nConvMode;
    char szPath[260];
    char szTitle[260];

    memset(szPath, 0, sizeof(szPath));
    nWeaponClassCode = 0;
    if (!GFXUTIL_IsDrawableUnitType(pUnit))
        return NULL;

    pInventory = GFXUTIL_GetUnitDisplayInventory(pUnit, NULL);
    D2COMMON_10885_COMPOSIT_unk(pUnit, szPath, &nWeaponClassCode, TRUE, 1, pInventory, nAnimMode);
    if (!szPath[0])
        return NULL;

    pGfxInfo = CUNIT_GfxGetUnitGfxInfo(pUnit);
    if (!pGfxInfo)
        return NULL;

    nMode = nAnimMode;
    pGfxInfo2 = CUNIT_GfxGetUnitGfxInfo(pUnit);
    if (!pGfxInfo2)
    {
        FOG_DisplayAssert((uint32_t)"ptGfxInfo", (uint32_t)szGfxCpp, 1010);
        exit(-1);
    }

    if (nAnimMode == -1)
    {
        if (GFX_TestFlagEx(pUnit, UNITFLAGEX_ISSHAPESHIFTED))
        {
            nConvType = (int32_t)pUnit->dwUnitType;
            nConvClass = (int32_t)pUnit->dwClassId;
            nConvMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
            D2COMMON_11013_ConvertMode(pUnit, &nConvType, &nConvClass, &nConvMode, szEngineH, 260);
            nMode = nConvMode;
        }
        else
        {
            nMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
        }
    }

    for (pNode = pGfxInfo2->pAnimList; pNode; pNode = pNode->pNext)
    {
        if (pNode->nAnimMode == nMode)
            break;
    }

    if (!pNode || pNode->nWeaponClassCode != nWeaponClassCode)
    {
        if (pNode)
        {
            *ppReplacedCofData = GFX_GetUnitCofData(pUnit, nMode);
            if (!GFX_FreeUnitAnimNodeByCof(pUnit, pNode->pCofFile))
            {
                FOG_DisplayAssert((uint32_t)"0", (uint32_t)szGfxCpp, 1032);
                exit(-1);
            }
        }

        pNewNode = (D2ClientCofNodeStrc *)FOG_Alloc(sizeof(D2ClientCofNodeStrc), (uint32_t)szGfxCpp, 1078, 0);
        if (!pNewNode)
        {
            FOG_DisplayAssert((uint32_t)"ptUnitCofNode", (uint32_t)szGfxCpp, 1079);
            exit(-1);
        }

        pNewNode->nAnimMode = 0;
        pNewNode->nWeaponClassCode = 0;
        pNewNode->pCofFile = NULL;
        pNewNode->field_0C = 0;
        pNewNode->pNext = NULL;

        nType = (int32_t)pUnit->dwUnitType;
        if ((pUnit->dwFlagEx >> 3) & 1)
        {
            nConvType = nType;
            nConvClass = (int32_t)pUnit->dwClassId;
            nConvMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
            D2COMMON_11013_ConvertMode(pUnit, &nConvType, &nConvClass, &nConvMode, szEngineH, 242);
            nType = nConvType;
        }

        pCache = nType ? GFX_OtherCofCache() : GFX_PlayerCofCache();

        memset(szTitle, 0, sizeof(szTitle));
        GFX_ExtractFileTitle(szPath, szTitle);
        _strlwr(szTitle);
        nBucket = GFX_HashCofName(szTitle);
        for (pCofFile = pCache->pBuckets[nBucket]; pCofFile; pCofFile = pCofFile->pNext)
        {
            if (GFX_CofNameEquals(szTitle, pCofFile))
                break;
        }

        if (!pCofFile)
        {
            pCofFile = GFX_LoadCofFile(pCache, szPath);
            if (!pCofFile)
            {
                //  BUG: when GFX_LoadCofFile fails, pNewNode -- allocated and zeroed, not yet linked -- is lost: this path returns NULL
                //  (0x6FB58A2E..0x6FB58A4F)
                //  with no FOG_Free. NOT ESTABLISHED: it needs a COF the archives do not hold; the stock archives' list against every path #10885
                //  builds was
                //  not taken.
                FOG_Trace("Could not add cof file: %s", szPath);
                return NULL;
            }
        }

        pNewNode->nAnimMode = nAnimMode;
        pNewNode->pCofFile = pCofFile;
        pNewNode->nWeaponClassCode = nWeaponClassCode;
        pNewNode->pNext = pGfxInfo->pAnimList;
        pGfxInfo->pAnimList = pNewNode;
    }

    pGfxInfo = CUNIT_GfxGetUnitGfxInfo(pUnit);
    if (!pGfxInfo)
        return NULL;

    for (pNode = pGfxInfo->pAnimList; pNode; pNode = pNode->pNext)
    {
        if (pNode->nAnimMode == nAnimMode)
            return pNode->pCofFile->pData;
    }

    return NULL;
}


//1.10f: D2Client.0x6FB58AC0
//  A path's file title: when the character four before the end is '.' (an extension of three), the characters between the last '\' and it,
//  copied to szTitle and terminated; otherwise szTitle is left as it was -- its two callers clear it first. GFX_GetUnitCofNode's paths come from
//  D2Common's #10885 with bAddPathPrefix TRUE (`push 1` 0x6FB587A6), the data directory's backslashes before the title. The first test of the walk
//  compares the '.' itself with '\' (`cmp cl, 5Ch` 0x6FB58AE4) and cannot hold -- the source's
//  `while` test before its first step.
//  BUG: the scan back for '\' has no bound: a path with no backslash before the extension would walk below its start.
void __fastcall GFX_ExtractFileTitle(const char *szPath, char *szTitle)
{
    const char *pScan;
    int32_t nLength;

    pScan = &szPath[strlen(szPath) - 4];
    if (*pScan != '.')
        return;

    nLength = 0;
    while (*pScan != '\\')
    {
        --pScan;
        ++nLength;
    }

    strncpy(szTitle, pScan + 1, nLength - 1);
    szTitle[nLength - 1] = '\0';
}


//1.10f: D2Client.0x6FB58B10
//  A COF file loaded into a cache: its node -- FOG_Alloc(28, this file, 219, 0), zeroed, flagged 1 (allocated alone, so GFX_FreeCelCache frees
//  it), named by the file's lower-cased title -- gets the file through ARCHIVE_LoadFile(D2Win's archive, the path, &nSize, this file, 223), whose
//  COF header is asserted at 64 directions or fewer (`cmp byte ptr [eax+2], 40h / jbe`, 0x6FB58CF7, line 238), then is pushed on its bucket
//  (GFX_HashCofName of the title) and counted into gnCofMemoryUsed with its 28 bytes. NULL when its "already cached" test fires -- which it does
//  not for the file asked about, below -- or the file does not load. The cache's buckets are at +0Ch. The original inlines GFX_ExtractFileTitle
//  twice and GFX_HashCofName twice here.
//  BUG: the "already cached" test looks up the title of the title. The title is taken from the path (0x6FB58B35..0x6FB58B72), and then the cache
//  is searched by the title of that (0x6FB58B9C..0x6FB58BE1) -- a title is a stem, with no '.' four from its end, so the second title stays the
//  empty string its buffer was cleared to, and the search is for "". A node is named "" only if its file's path had no three-letter extension,
//  so the test does not find the file it is asked about. LATENT: GFX_GetUnitCofNode searches the cache itself before it calls, and so does
//  GFX_AddMonsterCofFiles (Gfx.cpp:5277-5286).
//  BUG: when ARCHIVE_LoadFile fails the node, allocated and not linked, is lost (`jz loc_6FB58DA6` 0x6FB58CF1). NOT ESTABLISHED: it needs a
//  COF the archives do not hold; GFX_AddMonsterCofFiles halts on that NULL, GFX_GetUnitCofNode goes on.
//  BUG: the title is copied into the node's eight-byte szName (0x6FB58CC4..0x6FB58CCC) before its length is asserted below 8 (0x6FB58D29): a
//  longer one runs over pData and unk0x18 first. UNREACHABLE in effect: such a title halts right after, and every title built is seven characters.
//  BUG: the FOG_Alloc answer is not tested. LATENT: allocation failure.
D2ClientCofCacheNodeStrc *__fastcall GFX_LoadCofFile(D2ClientCofCacheStrc *pCache, const char *pszFileName)
{
    D2ClientCofCacheNodeStrc *pNode;
    uint8_t *pCofData;
    uint32_t nBucket;
    char szTitle[260];
    char szName[260];

    memset(szTitle, 0, sizeof(szTitle));
    GFX_ExtractFileTitle(pszFileName, szTitle);
    _strlwr(szTitle);

    memset(szName, 0, sizeof(szName));
    GFX_ExtractFileTitle(szTitle, szName);
    _strlwr(szName);
    nBucket = GFX_HashCofName(szName);
    for (pNode = pCache->pBuckets[nBucket]; pNode; pNode = pNode->pNext)
    {
        if (GFX_CofNameEquals(szName, pNode))
            return NULL;
    }

    pNode = (D2ClientCofCacheNodeStrc *)FOG_Alloc(sizeof(D2ClientCofCacheNodeStrc), (uint32_t)szGfxCpp, 219, 0);
    memset(pNode, 0, sizeof(D2ClientCofCacheNodeStrc));
    pNode->dwFlags |= 1;
    strcpy(pNode->szName, szTitle);
    pNode->pData = ARCHIVE_LoadFile(D2WIN_GetArchive(), pszFileName, (uint32_t *)&pNode->nSize, szGfxCpp, 223);
    if (!pNode->pData)
        return NULL;

    pCofData = (uint8_t *)pNode->pData;
    if (pCofData[2] > 64)
    {
        FOG_DisplayAssert((uint32_t)"swapCofHeader->bNumDirections <= 64", (uint32_t)szGfxCpp, 238);
        exit(-1);
    }

    nBucket = GFX_HashCofName(szTitle);
    pNode->pNext = pCache->pBuckets[nBucket];
    pCache->pBuckets[nBucket] = pNode;
    gnCofMemoryUsed += pNode->nSize + sizeof(D2ClientCofCacheNodeStrc);
    return pNode;
}


//1.10f: D2Client.0x6FB58DC0
//  The unit's animation node of a COF freed: the first node whose pCofFile is the cache node given, unlinked and freed (FOG_Free(node, this file,
//  978, 0), its next read first); TRUE, or FALSE when no node holds it. No gfx block asserts `ptGfxInfo` at line 970 and exits. The walk
//  compares the node's +8, pCofFile (`cmp [ecx+8], edi` 0x6FB58DF7); the one caller, GFX_GetUnitCofNode, asserts the answer.
BOOL __fastcall GFX_FreeUnitAnimNodeByCof(D2UnitStrc *pUnit, D2ClientCofCacheNodeStrc *pCofFile)
{
    D2ClientUnitGfxInfoStrc *pGfxInfo;
    D2ClientCofNodeStrc *pNode;
    D2ClientCofNodeStrc *pPrev;
    D2ClientCofNodeStrc *pNext;

    pGfxInfo = CUNIT_GfxGetUnitGfxInfo(pUnit);
    if (!pGfxInfo)
    {
        FOG_DisplayAssert((uint32_t)"ptGfxInfo", (uint32_t)szGfxCpp, 970);
        exit(-1);
    }

    pPrev = NULL;
    for (pNode = pGfxInfo->pAnimList; pNode && pNode->pCofFile != pCofFile; pNode = pNode->pNext)
        pPrev = pNode;

    if (!pNode)
        return FALSE;

    pNext = pNode->pNext;
    FOG_Free((uint32_t)pNode, (uint32_t)szGfxCpp, 978, 0);
    if (pPrev)
        pPrev->pNext = pNext;
    else
        pGfxInfo->pAnimList = pNext;

    return TRUE;
}


//1.10f: D2Client.0x6FB58E40
//  A monster's mode loaded: the unit's class converted (#11013, line 1124), its monstats2.txt row found, and the mode's bit in nModeFlags tested;
//  a missing row or a clear bit halts -- "[GFX.CPP] Mode (%i) is not used by monster (%i)", the mode and the class, FOG_DisplayHalt at line
//  1158 -- and exits. Then, unless the mode is marked loaded, its COF node is got or built (GFX_GetUnitCofNode) and, when there is one, the mode
//  is marked loaded. void: the exits store 1 or 0 and no caller reads eax. The mode bit is stored into the slot GFX_GetUnitCofNode is then
//  handed for its replaced COF (`mov [esp+120h+pReplacedCofData], edx` 0x6FB58F17) -- one slot, two uses. The original inlines
//  DATATBLS_GetMonStats2TxtRecord here.
void __fastcall GFX_LoadMonsterModeGfx(D2UnitStrc *pUnit, int32_t nMode)
{
    D2MonStats2Txt *pMonStats2Txt;
    void *pReplacedCofData;
    int32_t nType;
    int32_t nClass;
    int32_t nUnitMode;
    char szMessage[256];

    nType = pUnit ? (int32_t)pUnit->dwUnitType : (int32_t)UNIT_TYPES_COUNT;
    nClass = pUnit ? (int32_t)pUnit->dwClassId : -1;
    nUnitMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
    D2COMMON_11013_ConvertMode(pUnit, &nType, &nClass, &nUnitMode, szGfxCpp, 1124);

    pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord(nClass);
    if (!pMonStats2Txt || !(gdwBitMasks[nMode & 7] & pMonStats2Txt->nModeFlags[nMode >> 3]))
    {
        FOG_DisplayHalt((const char *)FOG_csprintf((uint32_t)szMessage, "[GFX.CPP] Mode (%i) is not used by monster (%i)", nMode, nClass), szGfxCpp, 1158);
        exit(-1);
    }

    if (GFXUTIL_IsUnitModeLoaded(pUnit, nMode))
        return;

    if (!GFX_GetUnitCofNode(pUnit, &pReplacedCofData, nMode))
        return;

    GFXUTIL_SetUnitModeLoaded(pUnit, nMode, 1);
}


//1.10f: D2Client.0x6FB58F90
//  An object's mode loaded: the unit's class converted (#11013, line 1213), its objects.txt row got (D2Common's #10626) and asserted (`ptStats`,
//  1217), and the row's Mode flag for the mode required to be 1 -- anything else halts, "[GFX.CPP] Mode (%i) is not used by object (%i)", line
//  1243, and exits. Then, unless the mode is marked loaded, its COF node is got or built, asserted (`lpbCofData`, 1235), and the mode marked
//  loaded. void: no caller reads eax. As in GFX_LoadMonsterModeGfx, one slot holds the mode bit and then the replaced-COF out-parameter.
void __fastcall GFX_LoadObjectModeGfx(D2UnitStrc *pUnit, int32_t nMode)
{
    D2ObjectsTxt *pObjectsTxt;
    void *pReplacedCofData;
    int32_t nType;
    int32_t nClass;
    int32_t nUnitMode;
    char szMessage[256];

    nType = pUnit ? (int32_t)pUnit->dwUnitType : (int32_t)UNIT_TYPES_COUNT;
    nClass = pUnit ? (int32_t)pUnit->dwClassId : -1;
    nUnitMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
    D2COMMON_11013_ConvertMode(pUnit, &nType, &nClass, &nUnitMode, szGfxCpp, 1213);

    pObjectsTxt = D2COMMON_DATATBLS_GetObjectsTxtRecord(nClass);
    if (!pObjectsTxt)
    {
        FOG_DisplayAssert((uint32_t)"ptStats", (uint32_t)szGfxCpp, 1217);
        exit(-1);
    }

    if (pObjectsTxt->nMode[nMode] != 1)
    {
        FOG_DisplayHalt((const char *)FOG_csprintf((uint32_t)szMessage, "[GFX.CPP] Mode (%i) is not used by object (%i)", nMode, nClass), szGfxCpp, 1243);
        exit(-1);
    }

    if (GFXUTIL_IsUnitModeLoaded(pUnit, nMode))
        return;

    if (!GFX_GetUnitCofNode(pUnit, &pReplacedCofData, nMode))
    {
        FOG_DisplayAssert((uint32_t)"lpbCofData", (uint32_t)szGfxCpp, 1235);
        exit(-1);
    }

    GFXUTIL_SetUnitModeLoaded(pUnit, nMode, 1);
}


//1.10f: D2Client.0x6FB590B0
//  The graphics caches set up: the COF memory budget stored; both COF caches cleared (203h dwords each, a D2ClientCofCacheStrc); the player
//  preload table (gaPlayerPreloadTable) cleared, 460h bytes in one `rep stosd` of 118h dwords (`mov ecx, 118h` 0x6FB590D2, `rep stosd`
//  0x6FB590DC); the preload throttle tick zeroed; the monster preload table allocated from Fog's default pool, 160 bytes per monstats.txt row
//  (`lea esi, [eax+eax*4]` / `shl esi, 5`, 0x6FB59103), FOG_AllocPool(NULL, size, this file, 2009, 0), and cleared; whether the client must
//  preload (D2Win's #10185) stored; and GFXUTIL_CacheTokens, the tail (`jmp` 0x6FB59136).
//  Its one caller is MEMPART_InitMemoryPartitions, which jumps here as its own tail with 2 MB (`jmp` 0x6FAA14DB). The name says "Cel"; the
//  caches are COF caches and the preload state. The allocation is not tested before the memset.
void __fastcall GFX_InitCelCache(int32_t nCofMemoryBudget)
{
    uint32_t nSize;

    gnCofMemoryBudget = nCofMemoryBudget;
    memset(GFX_PlayerCofCache(), 0, sizeof(D2ClientCofCacheStrc));
    memset(GFX_OtherCofCache(), 0, sizeof(D2ClientCofCacheStrc));
    memset(gaPlayerPreloadTable, 0, sizeof(gaPlayerPreloadTable));
    gdwPreloadThrottleTick = 0;
    nSize = sizeof(D2ClientModePreloadStrc) * sgptDataTables->nMonStatsTxtRecordCount;
    gpMonsterPreloadTable = (D2ClientModePreloadStrc *)FOG_AllocPool(0, nSize, (uint32_t)szGfxCpp, 2009, 0);
    memset(gpMonsterPreloadTable, 0, nSize);
    gbPreloadRequired = D2WIN_10185();
    GFXUTIL_CacheTokens();
}


//1.10f: D2Client.0x6FB59140
//  The graphics caches freed: each COF cache's buckets walked and every node flagged 1 -- loaded alone by GFX_LoadCofFile -- freed with its COF
//  (FOG_Free lines 3632 and 3633) and taken off gnCofMemoryUsed with its 28 bytes; nodes of an act's block (GFX_LoadActCofData's, flag 0) are
//  left to the block, which is freed after (3652 the player cache's, 3669 the other's) and taken off the count; each cache cleared; and the
//  monster preload table returned to Fog's default pool (FOG_FreePool, line 2033) and the pointer cleared. Its one caller is
//  MEMPART_DestroyCaches, which jumps here as its tail (0x6FAA14F0). void. The original inlines GFX_FreeCofCache here.
void __cdecl GFX_FreeCelCache(void)
{
    D2ClientCofCacheStrc *pCache;
    D2ClientCofCacheNodeStrc *pNode;
    D2ClientCofCacheNodeStrc *pNext;
    uint32_t nBucket;

    pCache = GFX_PlayerCofCache();
    for (nBucket = 0; nBucket < 512; ++nBucket)
    {
        for (pNode = pCache->pBuckets[nBucket]; pNode; pNode = pNext)
        {
            pNext = pNode->pNext;
            if (pNode->dwFlags & 1)
            {
                gnCofMemoryUsed += -(int32_t)sizeof(D2ClientCofCacheNodeStrc) - pNode->nSize;
                FOG_Free((uint32_t)pNode->pData, (uint32_t)szGfxCpp, 3632, 0);
                FOG_Free((uint32_t)pNode, (uint32_t)szGfxCpp, 3633, 0);
            }
        }
    }

    gnCofMemoryUsed -= pCache->nBlockSize;
    if (pCache->pBlock)
        FOG_Free((uint32_t)pCache->pBlock, (uint32_t)szGfxCpp, 3652, 0);

    memset(pCache, 0, sizeof(D2ClientCofCacheStrc));

    GFX_FreeCofCache();

    if (gpMonsterPreloadTable)
    {
        FOG_FreePool(NULL, gpMonsterPreloadTable, szGfxCpp, 2033, 0);
        gpMonsterPreloadTable = NULL;
    }
}


//1.10f: D2Client.0x6FB592C0
//  A unit's gfx block made: FOG_Alloc(60, this file, 2044, 0), asserted `ptGfxInfo` at 2045, zeroed (`rep stosd`, 0Fh dwords), both palette
//  shifts 2 (`mov [edx+34h], eax` / `mov [edx+38h], eax`, eax 2). Its one caller, UNITGFX_UnitInitGfx, stores it.
D2ClientUnitGfxInfoStrc *__cdecl GFX_AllocUnitGfxInfo(void)
{
    D2ClientUnitGfxInfoStrc *pGfxInfo;

    pGfxInfo = (D2ClientUnitGfxInfoStrc *)FOG_Alloc(sizeof(D2ClientUnitGfxInfoStrc), (uint32_t)szGfxCpp, 2044, 0);
    if (!pGfxInfo)
    {
        FOG_DisplayAssert((uint32_t)"ptGfxInfo", (uint32_t)szGfxCpp, 2045);
        exit(-1);
    }

    memset(pGfxInfo, 0, sizeof(D2ClientUnitGfxInfoStrc));
    pGfxInfo->nPaletteShift1 = 2;
    pGfxInfo->nPaletteShift2 = 2;
    return pGfxInfo;
}


//1.10f: D2Client.0x6FB59320
//  A unit's gfx block freed: its animation nodes (FOG_Free line 2066), its overlay entries -- each one's light removed first
//  (DLIGHTMAP_RemoveLight) and freed at line 1676 -- and the block (2072). No gfx block asserts `ptGfxInfo` at 2060 and exits. The unit's pointer
//  to the block is left; the callers are the unit's teardown and MONSTER_ChangeMonsterClass. The particle block at +30h is not freed here;
//  whether the other callers free the unit's particles first was not looked for. The original inlines an overlay-list free (its line 1676) here.
void __fastcall GFX_FreeUnitGfxInfo(D2UnitStrc *pUnit)
{
    D2ClientUnitGfxInfoStrc *pGfxInfo;
    D2ClientCofNodeStrc *pNode;
    D2ClientCofNodeStrc *pNextNode;
    D2ClientGfxEntryStrc *pEntry;
    D2ClientGfxEntryStrc *pNextEntry;
    D2ClientLightStrc *pLight;

    pGfxInfo = CUNIT_GfxGetUnitGfxInfo(pUnit);
    if (!pGfxInfo)
    {
        FOG_DisplayAssert((uint32_t)"ptGfxInfo", (uint32_t)szGfxCpp, 2060);
        exit(-1);
    }

    for (pNode = pGfxInfo->pAnimList; pNode; pNode = pNextNode)
    {
        pNextNode = pNode->pNext;
        FOG_Free((uint32_t)pNode, (uint32_t)szGfxCpp, 2066, 0);
    }

    for (pEntry = pGfxInfo->pOverlayList; pEntry; pEntry = pNextEntry)
    {
        pLight = pEntry->pLight;
        pNextEntry = pEntry->pNext;
        if (pLight)
            DLIGHTMAP_RemoveLight(pLight);

        FOG_Free((uint32_t)pEntry, (uint32_t)szGfxCpp, 1676, 0);
    }

    FOG_Free((uint32_t)pGfxInfo, (uint32_t)szGfxCpp, 2072, 0);
}


//1.10f: D2Client.0x6FB593C0
//  A unit's animation list freed: every node (FOG_Free line 2088), then the list cleared. No gfx block asserts `ptGfxInfo` at 2082 and exits.
//  Its caller is GFX_RebuildUnitGfx's monster arm (MONMODE_DEAD).
void __fastcall GFX_FreeUnitAnimList(D2UnitStrc *pUnit)
{
    D2ClientUnitGfxInfoStrc *pGfxInfo;
    D2ClientCofNodeStrc *pNode;
    D2ClientCofNodeStrc *pNext;

    pGfxInfo = CUNIT_GfxGetUnitGfxInfo(pUnit);
    if (!pGfxInfo)
    {
        FOG_DisplayAssert((uint32_t)"ptGfxInfo", (uint32_t)szGfxCpp, 2082);
        exit(-1);
    }

    for (pNode = pGfxInfo->pAnimList; pNode; pNode = pNext)
    {
        pNext = pNode->pNext;
        FOG_Free((uint32_t)pNode, (uint32_t)szGfxCpp, 2088, 0);
    }

    pGfxInfo->pAnimList = NULL;
}


//1.10f: D2Client.0x6FB59420
//  A unit's animation list freed but its death nodes: every node whose mode is neither PLRMODE_DEATH (0) nor PLRMODE_DEAD (11h) (`test eax, eax` /
//  `cmp eax, 11h`, 0x6FB5945A..0x6FB5945E) unlinked and freed (FOG_Free line 2111), the two kept. No gfx block asserts `ptGfxInfo` at 2101 and
//  exits. Both callers are in PLAYER_DoClientAction, on the death actions, with GFX_SetAllUnitModesLoaded(pUnit, 0) beside them.
void __fastcall GFX_FreeUnitAnimListButDeath(D2UnitStrc *pUnit)
{
    D2ClientUnitGfxInfoStrc *pGfxInfo;
    D2ClientCofNodeStrc *pNode;
    D2ClientCofNodeStrc *pPrev;
    D2ClientCofNodeStrc *pNext;

    pGfxInfo = CUNIT_GfxGetUnitGfxInfo(pUnit);
    if (!pGfxInfo)
    {
        FOG_DisplayAssert((uint32_t)"ptGfxInfo", (uint32_t)szGfxCpp, 2101);
        exit(-1);
    }

    pPrev = NULL;
    for (pNode = pGfxInfo->pAnimList; pNode; pNode = pNext)
    {
        pNext = pNode->pNext;
        if (pNode->nAnimMode == PLRMODE_DEATH || pNode->nAnimMode == PLRMODE_DEAD)
        {
            pPrev = pNode;
            continue;
        }

        FOG_Free((uint32_t)pNode, (uint32_t)szGfxCpp, 2111, 0);
        if (pPrev)
            pPrev->pNext = pNext;
        else
            pGfxInfo->pAnimList = pNext;
    }
}


//1.10f: D2Client.0x6FB59490
//  A one-cel unit's shadow: the shadow of a unit not drawn by components -- GFX_DrawUnitShadow (0x6FB596B6) sends here the units
//  GFXUTIL_IsDrawableUnitType refuses, missiles, items and the rest past UNIT_OBJECT. Nothing for a unit whose dwFlags carries
//  UNITFLAG_DRAWSHADOW (`shr eax, 5` / `and eax, 1` 0x6FB594A3) or in STATE_INVIS. Then the unit's type, class and mode converted (#11013, line
//  2151); the position is the unit's client coordinates plus its particle block's position `>> 11` (PARTICLE_GetUnitGfxCoordX/Y), less the
//  viewport origin; in perspective a quarter screen left with the right panel open, right with the left one, and 16 down whatever the panels
//  (`add ebx, 10h` 0x6FB595E2, reached from all three arms); with both panels open nothing is shifted and the shadow is still drawn.
//  GFX_IsUnitCofOnScreen (below) must find the unit's COF box on screen, its left edge moved by half the box's height for a shadow (bShadow 1);
//  then the direction, the frame, the gfx block filled for component 1 -- the one an item carries (GFXUTIL_ResolveUnitAnimIdentity) -- and
//  D2Gfx's TEXTURE_CelDrawShadow. void: the exits store 1 or 0 and the one caller discards it. The slot holds the perspective flag, then the
//  direction's byte (`mov byte ptr [esp+74h+bPerspective], al` 0x6FB59601).
//  UNITFLAG_DRAWSHADOW means the opposite in this module: set, no shadow. Object.cpp clears it for an object whose mode blocks light and sets it
//  otherwise, and States.cpp's invis setfunc sets it beside UNITFLAGEX_NODRAW; its only readers are the two shadow drawers here.
//  The object arm is dead: it offsets the position by objects.txt's Xoffset/Yoffset and refuses a mode whose Draw or BlocksLight is 0, after
//  asserting the row (`ptStats`, 2162). No object reaches this function -- the caller sends them to the component path -- and #11013 converts
//  a shapeshifted unit to UNIT_PLAYER or UNIT_MONSTER only. Written as the bytes have it; GFX_DrawUnitCelHilight carries the same arm.
//  BUG: an attached particle's position is written in subtiles (the catapults' loaded balls, missiles.txt 406, 410, 415, 418 and 421, made
//  UNITFLAGEX_NODRAW and not DRAWSHADOW), and a ball does reach the shadow list: GFX_DrawUnitCels draws the catapult's owner and its ball, forced
//  past NODRAW, and UNITGFX_GfxDrawUnit sets dwFlagEx bit 7 on it. Here its nX and nY are added `>> 11`. UNREACHABLE in effect:
//  GFX_IsUnitCofOnScreen then answers FALSE (a missile's animation list is always empty), so the sum is never drawn -- nor is anything else:
//  every unit that comes here is a type past UNIT_OBJECT, and this function reaches TEXTURE_CelDrawShadow for none of them.
void __fastcall GFX_DrawUnitCelShadow(D2UnitStrc *pUnit)
{
    D2ObjectsTxt *pObjectsTxt;
    D2GfxDataStrc tGfxData;
    int32_t nType;
    int32_t nClass;
    int32_t nMode;
    int32_t bPerspective;
    int32_t nOffsetX;
    int32_t nOffsetY;
    int32_t nX;
    int32_t nY;
    uint8_t nDirection;
    int32_t nFrame;
    int32_t nFrameCount;
    int32_t nClassOut;

    if (pUnit && (pUnit->dwFlags & UNITFLAG_DRAWSHADOW))
        return;

    if (D2COMMON_STATES_CheckState(pUnit, STATE_INVIS))
        return;

    nType = pUnit ? (int32_t)pUnit->dwUnitType : (int32_t)UNIT_TYPES_COUNT;
    nClass = pUnit ? (int32_t)pUnit->dwClassId : -1;
    nMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
    D2COMMON_11013_ConvertMode(pUnit, &nType, &nClass, &nMode, szGfxCpp, 2151);
    bPerspective = D2GFX_CheckPerspective();

    nOffsetX = 0;
    nOffsetY = 0;
    if (nType == UNIT_OBJECT)
    {
        pObjectsTxt = D2COMMON_DATATBLS_GetObjectsTxtRecord(nClass);
        if (!pObjectsTxt)
        {
            FOG_DisplayAssert((uint32_t)"ptStats", (uint32_t)szGfxCpp, 2162);
            exit(-1);
        }

        if (!pObjectsTxt->nDraw || !pObjectsTxt->nBlocksLight[nMode])
            return;

        nOffsetX = pObjectsTxt->dwXOffset;
        nOffsetY = pObjectsTxt->dwYOffset;
    }

    nX = nOffsetX - SCROLL_GetViewportOriginX();
    nX += PARTICLE_GetUnitGfxCoordX(pUnit);
    nX += D2COMMON_UNITS_GetClientCoordX(pUnit);
    nY = nOffsetY - SCROLL_GetViewportOriginY();
    nY += PARTICLE_GetUnitGfxCoordY(pUnit);
    nY += D2COMMON_UNITS_GetClientCoordY(pUnit);

    if (bPerspective)
    {
        //  `cdq` / `and edx, 3` / `add eax, edx` / `sar eax, 2`: the signed division by 4; the right panel's arm negates it (`neg eax`).
        switch (SCROLL_GetUiShiftScreen())
        {
        case PANEL_RIGHT:
            nX += -(gnScreenWidth / 4);
            break;

        case PANEL_LEFT:
            nX += gnScreenWidth / 4;
            break;
        }

        nY += 16;
    }

    if (!GFX_IsUnitCofOnScreen(pUnit, nX, nY, 0, 1))
        return;

    nDirection = D2COMMON_UNITS_GetDirection(pUnit);
    D2COMMON_UNITS_GetShiftedFrameMetrics(pUnit, &nFrame, &nFrameCount);
    memset(&tGfxData, 0, sizeof(tGfxData));
    if (!GFXUTIL_FillGfxData(&tGfxData, pUnit, NULL, nDirection, nFrame, &nClassOut, 1, -1))
        return;

    D2GFX_TEXTURE_CelDrawShadow(&tGfxData, nX, nY);
}


//1.10f: D2Client.0x6FB59670
//  A unit's shadow, layer by layer. Its one caller is DWALL_EnvDrawCellShadowsAndUnits, for a shadow-list node of kind 2. Nothing for
//  UNITFLAG_DRAWSHADOW or STATE_INVIS, as above; a unit not drawn by components goes to GFX_DrawUnitCelShadow.
//  The unit perspective is turned off for the player: with D2Gfx's unit perspective (#10007) and its perspective (#10010) both on, the player's
//  own unit, drawn as a player (Engine.h:242), has its shadow drawn with the unit perspective off (#10008, 0) and 8 higher; the setting saved
//  first is put back at the exits. The position: in perspective, the precision position less half the particle block's raw Z
//  (PARTICLE_GetUnitGfxCoordZRaw), refused unless D2Gfx's SCALE_CheckPerspectivePosition accepts it, adjusted (#10066) and moved by the
//  viewport origin and 16 down; otherwise the client position plus half the particle block's Z offset on both axes. Then the particle block's
//  screen offsets, the viewport origin less 2 on X, an object's objects.txt offsets, and the panel shift -- no 16 here, it was added above.
//  The layers: the cel context must resolve, as in GFX_DrawUnitCelShadow; the unit's type, class and mode converted (line 2334), its frame, its
//  weapon class code from the inventory it is drawn with, and its animation node got or rebuilt; the COF of its mode -- none traces "Unable to
//  get cof file for unit type:%d  class:%d  mode:%d in file:%s  line:%d" (2359) and leaves. The direction resolved against the COF's direction
//  count, the frame's layer order (asserted, `lpbCompByte`, 2372), the layers' bottom measured -- nothing drawn unless it answers above 0 -- and
//  every layer whose COF entry casts a shadow drawn with TEXTURE_CelDrawShadow; a component past NUM_COMPONENTS asserts (2390). The layer count
//  is the COF's first byte, read through GFXUTIL_ReturnArg's copy of the pointer, and the walk is unsigned (`jb` 0x6FB59BC2).
//  void: the exits store 1 or 0, and after the one call (0x6FAA6944) the caller reloads its node pointer and does not read eax. The original
//  inlines Engine.h's drawn-type conversion, GFXUTIL_GetUnitDisplayInventory, GFX_GetUnitAnimNode and GFX_GetUnitCofData here.
//  BUG: the perspective is not put back on two exits. A failed rebuild (`jz loc_6FB59A73` 0x6FB59A1A) and the missing COF's trace (0x6FB59A6B)
//  return through 0x6FB59A73, which restores nothing; the other three failing exits test bPerspectiveForcedOff first (0x6FB5979A, 0x6FB598CB,
//  0x6FB59B1F). After either, D2Gfx's unit perspective stays off for the player until something sets it. Reproduced. NOT ESTABLISHED: it is
//  forced off only for the local player drawn as a player, so it needs the player's own COF to fail.
void __fastcall GFX_DrawUnitShadow(D2UnitStrc *pUnit)
{
    D2ObjectsTxt *pObjectsTxt;
    D2InventoryStrc *pInventory;
    uint8_t *pCofData;
    uint8_t *pCof;
    uint8_t *pLayerOrder;
    D2GfxDataStrc tGfxData;
    int32_t bPerspective;
    BOOL bPerspectiveForcedOff;
    int32_t bScreenPerspective;
    int32_t nDrawnType;
    int32_t nDrawnClass;
    int32_t nDrawnMode;
    int32_t nHalfZ;
    int32_t nPrecisionX;
    int32_t nPrecisionY;
    int32_t nXAdjust;
    int32_t nYAdjust;
    int32_t nX;
    int32_t nY;
    int32_t nUnitType;
    int32_t nClassId;
    int32_t nAnimMode;
    int32_t nFrame;
    int32_t nFrameCount;
    int32_t nWeaponClassCode;
    int32_t nWeaponClassId;
    int32_t nUnitDirection;
    int32_t nCelDirection;
    int32_t nNumDirections;
    int32_t nBottomY;
    uint32_t iLayer;
    int32_t nComponent;
    int32_t nClassOut;

    if (pUnit && (pUnit->dwFlags & UNITFLAG_DRAWSHADOW))
        return;

    if (D2COMMON_STATES_CheckState(pUnit, STATE_INVIS))
        return;

    if (!GFXUTIL_IsDrawableUnitType(pUnit))
    {
        GFX_DrawUnitCelShadow(pUnit);
        return;
    }

    bPerspectiveForcedOff = FALSE;
    bPerspective = D2GFX_CheckUnitPerspective();
    if (bPerspective && D2GFX_CheckPerspective() && pUnit == CUNIT_GetPlayer())
    {
        if (pUnit && ((pUnit->dwFlagEx >> 3) & 1))
        {
            nDrawnType = (int32_t)pUnit->dwUnitType;
            nDrawnClass = (int32_t)pUnit->dwClassId;
            nDrawnMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
            D2COMMON_11013_ConvertMode(pUnit, &nDrawnType, &nDrawnClass, &nDrawnMode, szEngineH, 242);
        }
        else
        {
            nDrawnType = pUnit ? (int32_t)pUnit->dwUnitType : (int32_t)UNIT_TYPES_COUNT;
        }

        if (nDrawnType == UNIT_PLAYER)
        {
            D2GFX_SetUnitPerspective(0);
            bPerspectiveForcedOff = TRUE;
        }
    }

    bScreenPerspective = D2GFX_CheckPerspective();
    if (bScreenPerspective)
    {
        //  `cdq` / `sub eax, edx` / `sar` 1: the signed halving.
        nHalfZ = PARTICLE_GetUnitGfxCoordZRaw(pUnit) / 2;
        nPrecisionX = D2COMMON_UNITS_GetPrecisionX(pUnit) - nHalfZ;
        nPrecisionY = D2COMMON_UNITS_GetPrecisionY(pUnit) - nHalfZ;
        if (!D2GFX_SCALE_CheckPerspectivePosition(nPrecisionX, nPrecisionY))
        {
            if (bPerspectiveForcedOff)
                D2GFX_SetUnitPerspective(bPerspective);
            return;
        }

        D2GFX_SCALE_AdjustPerspectivePosition(nPrecisionX, nPrecisionY, 0, &nXAdjust, &nYAdjust);
        nXAdjust += SCROLL_GetViewportOriginX();
        nYAdjust = nYAdjust + SCROLL_GetViewportOriginY() + 16;
    }
    else
    {
        nHalfZ = PARTICLE_GetUnitParticleOffsetZ(pUnit) / 2;
        nXAdjust = D2COMMON_UNITS_GetClientCoordX(pUnit) + nHalfZ;
        nYAdjust = D2COMMON_UNITS_GetClientCoordY(pUnit) + nHalfZ;
    }

    if (bPerspectiveForcedOff)
        nYAdjust -= 8;

    nXAdjust += PARTICLE_GetUnitParticleOffsetX(pUnit);
    nYAdjust += PARTICLE_GetUnitParticleOffsetY(pUnit);
    nX = nXAdjust - SCROLL_GetViewportOriginX() - 2;
    nY = nYAdjust - SCROLL_GetViewportOriginY();

    if (pUnit && pUnit->dwUnitType == UNIT_OBJECT)
    {
        //  BUG: the objects.txt row is used with no NULL test (0x6FB5986C), where GFX_DrawUnitCelShadow asserts the same row. Reproduced.
        //  UNREACHABLE:
        //  D2Common's getter asserts both bounds and exits.
        pObjectsTxt = D2COMMON_DATATBLS_GetObjectsTxtRecord(pUnit->dwClassId);
        nX += pObjectsTxt->dwXOffset;
        nY += pObjectsTxt->dwYOffset;
    }

    if (bScreenPerspective)
    {
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

    if (!GFX_IsUnitCofOnScreen(pUnit, nX, nY, 0, 1))
    {
        if (bPerspectiveForcedOff)
            D2GFX_SetUnitPerspective(bPerspective);
        return;
    }

    nUnitType = pUnit ? (int32_t)pUnit->dwUnitType : (int32_t)UNIT_TYPES_COUNT;
    nClassId = pUnit ? (int32_t)pUnit->dwClassId : -1;
    nAnimMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
    D2COMMON_11013_ConvertMode(pUnit, &nUnitType, &nClassId, &nAnimMode, szGfxCpp, 2334);
    D2COMMON_UNITS_GetShiftedFrameMetrics(pUnit, &nFrame, &nFrameCount);

    pInventory = GFXUTIL_GetUnitDisplayInventory(pUnit, NULL);
    nWeaponClassCode = D2COMMON_COMPOSIT_GetWeaponClassId(pUnit, pInventory, &nWeaponClassId, nAnimMode, FALSE);

    if (!GFX_GetUnitAnimNode(pUnit, nWeaponClassCode, -1) && !GFX_RebuildUnitGfx(pUnit, nWeaponClassCode, -1))
        return;

    pCofData = (uint8_t *)GFX_GetUnitCofData(pUnit, nAnimMode);
    if (!pCofData)
    {
        FOG_Trace("Unable to get cof file for unit type:%d  class:%d  mode:%d in file:%s  line:%d", nUnitType, nClassId, nAnimMode, szGfxCpp, 2359);
        return;
    }

    pCof = (uint8_t *)GFXUTIL_ReturnArg(pCofData);
    nUnitDirection = D2COMMON_UNITS_GetDirection(pUnit);
    GFXUTIL_ResolveAnimDirection(pUnit, (D2C_UnitTypes)nUnitType, nClassId, nAnimMode, &nUnitDirection, &nCelDirection, &nNumDirections, pCof[2], 1);
    pLayerOrder = GFXUTIL_GetCofLayerOrder(pCofData, nCelDirection, nFrame);
    if (!pLayerOrder)
    {
        FOG_DisplayAssert((uint32_t)"lpbCompByte", (uint32_t)szGfxCpp, 2372);
        exit(-1);
    }

    nBottomY = 0;
    if (GFXUTIL_MeasureLayersBottomY(pUnit, pCof, pLayerOrder, &nBottomY, nUnitDirection, nFrame) <= 0)
    {
        if (bPerspectiveForcedOff)
            D2GFX_SetUnitPerspective(bPerspective);
        return;
    }

    for (iLayer = 0; iLayer < pCof[0]; ++iLayer)
    {
        nComponent = pLayerOrder[iLayer];
        if (nComponent >= (int32_t)NUM_COMPONENTS)
        {
            FOG_DisplayAssert((uint32_t)"eComponent < NUM_COMPONENTS", (uint32_t)szGfxCpp, 2390);
            exit(-1);
        }

        if (GFXUTIL_GetCofLayerCastsShadow(pCofData, nComponent))
        {
            memset(&tGfxData, 0, sizeof(tGfxData));
            if (GFXUTIL_FillGfxData(&tGfxData, pUnit, NULL, nUnitDirection, nFrame, &nClassOut, nComponent, -1))
                D2GFX_TEXTURE_CelDrawShadow(&tGfxData, nX, nY);
        }
    }

    if (bPerspectiveForcedOff)
        D2GFX_SetUnitPerspective(bPerspective);
}


//1.10f: D2Client.0x6FB59C10
//  A one-cel unit's hit flash: the unit's cel drawn with D2Gfx's TEXTURE_CelDrawHilight in the colour GFX_GetUnitHilightPaletteIndex picks. The
//  one call site is GFX_DrawUnitHilight (below, 0x6FB5A309), for a unit that is not a player, monster or object; GFX_DrawUnitCels calls that
//  while CUNIT_ShouldDrawUnitComponents spends the unit's flash counter, which only CUNIT_SetUnitComponentDrawCount arms -- three frames -- from
//  MONSTER_DoClientAction's two hit arms, a monster's. So no unit reaching this function has the counter armed by that path (nothing else in
//  D2Client writes it, +58h); how a unit's block is cleared when it is made: not looked for.
//  The shape is GFX_DrawUnitCelShadow's: the type converted (line 2476), the same dead object arm (2484), the client position less the viewport
//  origin -- no particle block here -- and the panel shift with no 16; the cel context resolved not centred (`push 0` 0x6FB59D25), the colour
//  fetched, the direction, the frame, component 1. void: the bytes still leave 1, 0, or IsUnitCofOnScreen's 0 in eax, and the one caller
//  discards it. The slot holds the perspective flag, then the direction's byte (0x6FB59D54).
void __fastcall GFX_DrawUnitCelHilight(D2UnitStrc *pUnit)
{
    D2ObjectsTxt *pObjectsTxt;
    D2GfxDataStrc tGfxData;
    int32_t nType;
    int32_t nClass;
    int32_t nMode;
    int32_t bPerspective;
    int32_t nOffsetX;
    int32_t nOffsetY;
    int32_t nX;
    int32_t nY;
    uint8_t nPaletteIndex;
    uint8_t nDirection;
    int32_t nFrame;
    int32_t nFrameCount;
    int32_t nClassOut;

    nOffsetX = 0;
    nOffsetY = 0;
    nType = pUnit ? (int32_t)pUnit->dwUnitType : (int32_t)UNIT_TYPES_COUNT;
    nClass = pUnit ? (int32_t)pUnit->dwClassId : -1;
    nMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
    D2COMMON_11013_ConvertMode(pUnit, &nType, &nClass, &nMode, szGfxCpp, 2476);
    bPerspective = D2GFX_CheckPerspective();

    if (nType == UNIT_OBJECT)
    {
        pObjectsTxt = D2COMMON_DATATBLS_GetObjectsTxtRecord(nClass);
        if (!pObjectsTxt)
        {
            FOG_DisplayAssert((uint32_t)"ptStats", (uint32_t)szGfxCpp, 2484);
            exit(-1);
        }

        if (!pObjectsTxt->nDraw || !pObjectsTxt->nBlocksLight[nMode])
            return;

        nOffsetX = pObjectsTxt->dwXOffset;
        nOffsetY = pObjectsTxt->dwYOffset;
    }

    nX = nOffsetX - SCROLL_GetViewportOriginX();
    nX += D2COMMON_UNITS_GetClientCoordX(pUnit);
    nY = nOffsetY - SCROLL_GetViewportOriginY();
    nY += D2COMMON_UNITS_GetClientCoordY(pUnit);

    if (bPerspective)
    {
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

    if (!GFX_IsUnitCofOnScreen(pUnit, nX, nY, 0, 0))
        return;

    nPaletteIndex = GFX_GetUnitHilightPaletteIndex(pUnit);
    nDirection = D2COMMON_UNITS_GetDirection(pUnit);
    D2COMMON_UNITS_GetShiftedFrameMetrics(pUnit, &nFrame, &nFrameCount);
    memset(&tGfxData, 0, sizeof(tGfxData));
    if (!GFXUTIL_FillGfxData(&tGfxData, pUnit, NULL, nDirection, nFrame, &nClassOut, 1, -1))
        return;

    D2GFX_TEXTURE_CelDrawHilight(&tGfxData, nX, nY, nPaletteIndex);
}


//1.10f: D2Client.0x6FB59DD0
//  The hit flash's colour. The four palette indices are made once (gbPaletteIndicesInit): D2Win's MixRGB of green (0, C0h, 0), yellow
//  (E0h, E0h, 0), red (E0h, 0, 0) and blue (0, 0, C0h), each stored as a byte. Then the unit's type and class converted (line 2439): anything
//  but a monster is green; a monster whose monstats.txt row carries MONSTATSFLAGINDEX_INTERACT (an NPC one can talk to) is blue; otherwise by
//  its life, 100 * STAT_HITPOINTS / its maximum life, signed (`idiv` 0x6FB59EF4): below 33 red, below 66 yellow, the rest green -- green also
//  for a maximum of 0. It is the colour of the flash a monster is drawn with for three frames after it is hit (GFX_DrawUnitCelHilight above).
//  The original inlines DATATBLS_GetMonStatsTxtRecord here.
uint8_t __fastcall GFX_GetUnitHilightPaletteIndex(D2UnitStrc *pUnit)
{
    D2MonStatsTxt *pMonStatsTxt;
    int32_t nType;
    int32_t nClass;
    int32_t nMode;
    int32_t nMaxLife;
    int32_t nLifePercent;

    if (!gbPaletteIndicesInit)
    {
        gnPaletteIndexGreen = D2WIN_10034_MixRGB(0, 0xC0, 0);
        gnPaletteIndexYellow = D2WIN_10034_MixRGB(0xE0, 0xE0, 0);
        gnPaletteIndexRed = D2WIN_10034_MixRGB(0xE0, 0, 0);
        gnPaletteIndexBlue = D2WIN_10034_MixRGB(0, 0, 0xC0);
        gbPaletteIndicesInit = 1;
    }

    nType = pUnit ? (int32_t)pUnit->dwUnitType : (int32_t)UNIT_TYPES_COUNT;
    nClass = pUnit ? (int32_t)pUnit->dwClassId : -1;
    nMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
    D2COMMON_11013_ConvertMode(pUnit, &nType, &nClass, &nMode, szGfxCpp, 2439);
    if (nType != UNIT_MONSTER)
        return gnPaletteIndexGreen;

    //  BUG: the monstats.txt row is read whether or not the lookup found one: a class outside [0, nMonStatsTxtRecordCount) makes the copy's NULL
    //  (`xor eax, eax` 0x6FB59EB9), and `mov al, [eax+0Dh]` (0x6FB59EC1) reads it with nothing between: a fault at 0Dh. Reproduced: the routed
    //  lookup answers NULL and the line below reads it. LATENT: a monster's class is bounded where it is made and changed, and the four transform
    //  classes of states.txt (430, 431, 135, 212) are in the table.
    pMonStatsTxt = DATATBLS_GetMonStatsTxtRecord(nClass);
    if (pMonStatsTxt->nMonStatsFlags[MONSTATSFLAGINDEX_INTERACT >> 3] & gdwBitMasks[MONSTATSFLAGINDEX_INTERACT & 7])
        return gnPaletteIndexBlue;

    nMaxLife = D2COMMON_STATLIST_GetMaxLifeFromUnit(pUnit);
    if (!nMaxLife)
        return gnPaletteIndexGreen;

    //  BUG: the percentage can overflow: `lea` / `lea` / `shl 2` (0x6FB59EEA..0x6FB59EF0) is the hit points times 100 in 32 bits; the stat is 8.8
    //  fixed point, so a monster with more than 83886 hit points wraps. Written unsigned, which wraps the same way and is defined, then divided
    //  signed as the bytes divide. LATENT: the client keeps a monster's life on a scale of 128 (STAT_MAXHP 0x8000); two unmasked packet values and
    //  a
    //  hireling's 32-bit stat packets were not traced to a bound.
    nLifePercent = (int32_t)(100u * (uint32_t)D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_HITPOINTS, 0)) / nMaxLife;
    if (nLifePercent < 33)
        return gnPaletteIndexRed;

    if (nLifePercent < 66)
        return gnPaletteIndexYellow;

    return gnPaletteIndexGreen;
}


//1.10f: D2Client.0x6FB59F20
//  A unit's hit flash, layer by layer: GFX_DrawUnitShadow's shape with TEXTURE_CelDrawHilight in place of the shadow, and the colour
//  GFX_GetUnitHilightPaletteIndex picks. One call site, GFX_DrawUnitCels (0x6FB5A760), when CUNIT_ShouldDrawUnitComponents has spent a tick of
//  the flash counter -- three draws after MONSTER_DoClientAction's hit arms arm it (GFX_DrawUnitCelHilight above). A unit not drawn by
//  components goes to GFX_DrawUnitCelHilight, by a tail.
//  The position: in perspective, the precision position adjusted with the particle block's Z offset as D2Gfx's bias (#10066), the panel shift,
//  16 down, then the block's X and Y offsets; flat, the client position less the viewport origin plus the block's X offset, and its Y and Z
//  offsets on Y. The cel context resolved not centred; then the colour, the converted type, class and mode (line 2597), the direction, the
//  frame, the weapon class code, the animation node got or rebuilt, the mode's COF -- none traces "Unable to get cof file ..." (2622) and
//  leaves -- the direction resolved, the layer order (asserted, 2634), the bottom measured, and every layer whose component
//  GFXUTIL_IsUnitComponentVisible passes drawn. No component bound here, where GFX_DrawUnitShadow asserts one.
//  void: the bytes leave 1 or 0 and the caller compares two other registers next. The original inlines GFXUTIL_IsDrawableUnitType,
//  GFXUTIL_GetUnitDisplayInventory, GFX_GetUnitAnimNode and GFX_GetUnitCofData here.
void __fastcall GFX_DrawUnitHilight(D2UnitStrc *pUnit)
{
    D2InventoryStrc *pInventory;
    uint8_t *pCofData;
    uint8_t *pCof;
    uint8_t *pLayerOrder;
    D2GfxDataStrc tGfxData;
    int32_t bPerspective;
    int32_t nBias;
    int32_t nPrecisionX;
    int32_t nPrecisionY;
    int32_t nX;
    int32_t nY;
    uint8_t nPaletteIndex;
    int32_t nType;
    int32_t nClass;
    int32_t nMode;
    int32_t nFrame;
    int32_t nFrameCount;
    int32_t nWeaponClassCode;
    int32_t nWeaponClassId;
    int32_t nUnitDirection;
    int32_t nCelDirection;
    int32_t nNumDirections;
    int32_t nBottomY;
    uint32_t iLayer;
    int32_t nComponent;
    int32_t nClassOut;

    bPerspective = D2GFX_CheckPerspective();
    if (!GFXUTIL_IsDrawableUnitType(pUnit))
    {
        GFX_DrawUnitCelHilight(pUnit);
        return;
    }

    if (bPerspective)
    {
        nBias = PARTICLE_GetUnitParticleOffsetZ(pUnit);
        nPrecisionY = D2COMMON_UNITS_GetPrecisionY(pUnit);
        nPrecisionX = D2COMMON_UNITS_GetPrecisionX(pUnit);
        D2GFX_SCALE_AdjustPerspectivePosition(nPrecisionX, nPrecisionY, nBias, &nX, &nY);
        switch (SCROLL_GetUiShiftScreen())
        {
        case PANEL_RIGHT:
            nX += -(gnScreenWidth / 4);
            break;

        case PANEL_LEFT:
            nX += gnScreenWidth / 4;
            break;
        }

        nY += 16;
        nX += PARTICLE_GetUnitParticleOffsetX(pUnit);
        nY += PARTICLE_GetUnitParticleOffsetY(pUnit);
    }
    else
    {
        nX = D2COMMON_UNITS_GetClientCoordX(pUnit);
        nX -= SCROLL_GetViewportOriginX();
        nX += PARTICLE_GetUnitParticleOffsetX(pUnit);
        nY = D2COMMON_UNITS_GetClientCoordY(pUnit);
        nY -= SCROLL_GetViewportOriginY();
        nY += PARTICLE_GetUnitParticleOffsetY(pUnit);
        nY += PARTICLE_GetUnitParticleOffsetZ(pUnit);
    }

    if (!GFX_IsUnitCofOnScreen(pUnit, nX, nY, 0, 0))
        return;

    nPaletteIndex = GFX_GetUnitHilightPaletteIndex(pUnit);
    nType = (int32_t)pUnit->dwUnitType;
    nClass = (int32_t)pUnit->dwClassId;
    nMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
    D2COMMON_11013_ConvertMode(pUnit, &nType, &nClass, &nMode, szGfxCpp, 2597);
    nUnitDirection = D2COMMON_UNITS_GetDirection(pUnit);
    D2COMMON_UNITS_GetShiftedFrameMetrics(pUnit, &nFrame, &nFrameCount);

    pInventory = GFXUTIL_GetUnitDisplayInventory(pUnit, NULL);
    nWeaponClassCode = D2COMMON_COMPOSIT_GetWeaponClassId(pUnit, pInventory, &nWeaponClassId, nMode, FALSE);

    if (!GFX_GetUnitAnimNode(pUnit, nWeaponClassCode, -1) && !GFX_RebuildUnitGfx(pUnit, nWeaponClassCode, -1))
        return;

    pCofData = (uint8_t *)GFX_GetUnitCofData(pUnit, nMode);
    if (!pCofData)
    {
        FOG_Trace("Unable to get cof file for unit type:%d  class:%d  mode:%d in file:%s  line:%d", nType, nClass, nMode, szGfxCpp, 2622);
        return;
    }

    pCof = (uint8_t *)GFXUTIL_ReturnArg(pCofData);
    GFXUTIL_ResolveAnimDirection(pUnit, (D2C_UnitTypes)nType, nClass, nMode, &nUnitDirection, &nCelDirection, &nNumDirections, pCof[2], 1);
    pLayerOrder = GFXUTIL_GetCofLayerOrder(pCofData, nCelDirection, nFrame);
    if (!pLayerOrder)
    {
        FOG_DisplayAssert((uint32_t)"lpbCompByte", (uint32_t)szGfxCpp, 2634);
        exit(-1);
    }

    nBottomY = 0;
    if (GFXUTIL_MeasureLayersBottomY(pUnit, pCof, pLayerOrder, &nBottomY, nUnitDirection, nFrame) <= 0)
        return;

    for (iLayer = 0; iLayer < pCof[0]; ++iLayer)
    {
        nComponent = pLayerOrder[iLayer];
        if (GFXUTIL_IsUnitComponentVisible(pUnit, (D2Composits)nComponent))
        {
            memset(&tGfxData, 0, sizeof(tGfxData));
            if (GFXUTIL_FillGfxData(&tGfxData, pUnit, NULL, nUnitDirection, nFrame, &nClassOut, nComponent, -1))
                D2GFX_TEXTURE_CelDrawHilight(&tGfxData, nX, nY, nPaletteIndex);
        }
    }
}


//1.10f: D2Client.0x6FB5A320
//  An item drawn at a screen position. Nothing for a unit that is not an item. Gold picks its pile by the amount, compared UNSIGNED (`jb`
//  0x6FB5A35F, `jnb` 0x6FB5A366, `sbb` 0x6FB5A374): below 100 the first, below 500 the second, below 5000 the third, the fourth above -- handed
//  to GFXUTIL_FillGfxData as the direction. Then the sprite validated (D2CMP's SpriteValidate, loaded asynchronously), the cel tested on screen
//  with its height added to nY (GFXUTIL_IsCelVisible), the item's colour palette (D2Common's ITEMS_GetColor), DRAWMODE_NORMAL or DRAWMODE_TRANS50
//  for an ethereal item (line 2742), TEXTURE_CelDraw at nX and nY plus the cel's height, and the item's overlays (GFX_DrawUnitOverlays, pass 0,
//  flat at the cel's centre, `shr` 1 of the height and width, unsigned). Called by the inventory panels, grid, sockets and the mercenary's, the
//  belt, the anvil and the cursor's item. `retn 4`: nY on the stack. void: the bytes leave 1 or 0. The colour and the flag test are called
//  before the heights.
void __fastcall GFX_DrawInventoryItem(D2UnitStrc *pItem, int32_t nX, int32_t nY)
{
    D2GfxDataStrc tGfxData;
    uint8_t *pPalette;
    uint8_t nColor;
    uint32_t nGold;
    int32_t nGoldPile;
    int32_t eDrawMode;
    int32_t nCelHeight;
    int32_t nCenterY;
    int32_t nClassOut;

    if (!pItem || pItem->dwUnitType != UNIT_ITEM)
        return;

    nGoldPile = 0;
    if (D2COMMON_ITEMS_GetItemType(pItem) == ITEMTYPE_GOLD)
    {
        nGold = (uint32_t)D2COMMON_STATLIST_UnitGetStatValue(pItem, STAT_GOLD, 0);
        if (nGold >= 100)
        {
            if (nGold < 500)
                nGoldPile = 1;
            else
                nGoldPile = 3 - (nGold < 5000);
        }
    }

    memset(&tGfxData, 0, sizeof(tGfxData));
    if (!GFXUTIL_FillGfxData(&tGfxData, pItem, NULL, nGoldPile, 0, &nClassOut, 1, -1))
        return;

    if (!D2CMP_SpriteValidate(&tGfxData, FALSE, TRUE))
        return;

    if (!GFXUTIL_IsCelVisible(tGfxData.pCurrentCell, nX, nY + D2CMP_CelGetHeight(tGfxData.pCurrentCell), 0))
        return;

    pPalette = D2COMMON_ITEMS_GetColor(NULL, pItem, &nColor, 1);
    eDrawMode = DRAWMODE_NORMAL;
    if (D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_ETHEREAL, 2742, szGfxCpp))
        eDrawMode = DRAWMODE_TRANS50;

    nCelHeight = D2CMP_CelGetHeight(tGfxData.pCurrentCell);
    D2GFX_TEXTURE_CelDraw(&tGfxData, nX, nY + nCelHeight, 0xFFFFFFFF, eDrawMode, pPalette);
    nCenterY = nY + ((uint32_t)D2CMP_CelGetHeight(tGfxData.pCurrentCell) >> 1);
    GFX_DrawUnitOverlays(pItem, 0xFF, 0, nX + ((uint32_t)D2CMP_CelGetWidth(tGfxData.pCurrentCell) >> 1), nCenterY, 0);
}


//1.10f: D2Client.0x6FB5A490
//  A unit's overlays drawn, one pass. No gfx block asserts `ptGfxInfo` (line 627) and exits.
//  The position: in perspective -- except for an item not on the ground (IMODE_ONGROUND), drawn flat wherever it is held -- the precision
//  position through D2Gfx's SCALE_AdjustPerspectivePosition into nX and nY themselves, the panel shift, 16 down. Flat, the client position less
//  the viewport origin, or, for an item, nX and nY when neither is -1 (the inventory's screen position). Then the particle block's X, Y and Z
//  offsets, Z on Y, and 8 up with bShiftUp8 -- set by GFX_DrawUnitCels exactly when it turned the unit perspective off for the player, the
//  shift GFX_DrawUnitShadow makes in the same case.
//  The pass: the overlay list walked as GFX_DrawOverlayAt walks it: an entry fits pass 1 if its PreDraw flag is set and pass 0 if not; a DELAYED
//  entry not playing and an AURA entry not active are skipped. Each entry's overlay.txt row (asserted, `ptStats`, 723), the request filled for a
//  NULL unit with the overlay's id, the unit's direction and the entry's frame `>> 8`, and drawn while the frame is below the frame count `>> 8`
//  (both `sar`, signed), with nColor as the tint and overlay.txt's Trans as the draw mode, at the position plus the entry's offsets. A request
//  that does not resolve returns from the whole function (`jz loc_6FB5A6F3` 0x6FB5A696), the entries after it not drawn -- as in
//  GFX_DrawOverlayAt. The callers: GFX_DrawInventoryItem (pass 0, the cel's centre) and GFX_DrawUnitCels (pass 1 before a drawable unit's
//  layers, pass 0 after them, pass 0 after a one-cel unit with no shift; -1, -1 for the position).
//  The request's class goes to nX's own slot (`lea ecx, [esp+6Ch+nX]` 0x6FB5A66B), dead by then; a local of ours here.
//  The original inlines GFXUTIL_GetOverlayTxtRecord here.
void __fastcall GFX_DrawUnitOverlays(D2UnitStrc *pUnit, uint32_t nColor, int32_t bPreDrawLayer, int32_t nX, int32_t nY, int32_t bShiftUp8)
{
    D2ClientUnitGfxInfoStrc *pGfxInfo;
    D2ClientGfxEntryStrc *pEntry;
    D2OverlayTxt *pOverlayTxt;
    D2GfxDataStrc tGfxData;
    int32_t bPerspective;
    int32_t nPrecisionX;
    int32_t nPrecisionY;
    int32_t nPosX;
    int32_t nPosY;
    int32_t bDraw;
    int32_t nFrame;
    int32_t nClassOut;

    pGfxInfo = CUNIT_GfxGetUnitGfxInfo(pUnit);
    if (!pGfxInfo)
    {
        FOG_DisplayAssert((uint32_t)"ptGfxInfo", (uint32_t)szGfxCpp, 627);
        exit(-1);
    }

    bPerspective = D2GFX_CheckPerspective();
    pEntry = pGfxInfo->pOverlayList;
    if (!pEntry)
        return;

    if (bPerspective && (!pUnit || pUnit->dwUnitType != UNIT_ITEM || D2COMMON_UNITS_GetAnimOrSeqMode(pUnit) == IMODE_ONGROUND))
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

        nPosX = nX;
        nPosY = nY + 16;
    }
    else
    {
        nPosX = D2COMMON_UNITS_GetClientCoordX(pUnit);
        nPosX -= SCROLL_GetViewportOriginX();
        nPosY = D2COMMON_UNITS_GetClientCoordY(pUnit);
        nPosY -= SCROLL_GetViewportOriginY();
        if (pUnit && pUnit->dwUnitType == UNIT_ITEM && nX != -1 && nY != -1)
        {
            nPosX = nX;
            nPosY = nY;
        }
    }

    nPosX += PARTICLE_GetUnitParticleOffsetX(pUnit);
    nPosY += PARTICLE_GetUnitParticleOffsetY(pUnit);
    nPosY += PARTICLE_GetUnitParticleOffsetZ(pUnit);
    if (bShiftUp8)
        nPosY -= 8;

    for (; pEntry; pEntry = pEntry->pNext)
    {
        bDraw = TRUE;
        if (pEntry->bPreDraw && !bPreDrawLayer)
            bDraw = FALSE;
        if (!pEntry->bPreDraw && bPreDrawLayer)
            bDraw = FALSE;
        if (pEntry->nKind == OVERLAYTYPE_DELAYED && !pEntry->field_08)
            bDraw = FALSE;
        if (pEntry->nKind == OVERLAYTYPE_AURA && !pEntry->bAuraActive)
            bDraw = FALSE;

        if (!bDraw)
            continue;

        nFrame = pEntry->nFrame >> 8;
        pOverlayTxt = GFXUTIL_GetOverlayTxtRecord(pEntry->nOverlayId);
        if (!pOverlayTxt)
        {
            FOG_DisplayAssert((uint32_t)"ptStats", (uint32_t)szGfxCpp, 723);
            exit(-1);
        }

        memset(&tGfxData, 0, sizeof(tGfxData));
        tGfxData.nOverlay = pEntry->nOverlayId;
        if (!GFXUTIL_FillGfxData(&tGfxData, NULL, NULL, D2COMMON_UNITS_GetDirection(pUnit), nFrame, &nClassOut, 1, -1))
            return;

        if (nFrame < pEntry->nFrameCount >> 8)
        {
            //  BUG: the green-blood palette is fetched and thrown away. For an overlay whose overlay.txt LocalBlood is set (`mov al, [ebp+81h]`
            //  0x6FB5A6A2),
            //  with green blood on (GAME_IsGreenBloodEnabled), PALSHIFT_GetGreenBloodRemap is called (0x6FB5A6B5) and its answer is overwritten by
            //  the next
            //  load (`mov eax, [ebx+24h]` 0x6FB5A6BA): TEXTURE_CelDraw gets a literal NULL palette (`push 0` 0x6FB5A6CE), so a blood overlay is
            //  never drawn
            //  green. Reproduced. LATENT: green blood is on only for a Color.txt whose first byte is '1', and the stock one holds 0.
            if (pOverlayTxt->nLocalBlood && GAME_IsGreenBloodEnabled())
                PALSHIFT_GetGreenBloodRemap();

            D2GFX_TEXTURE_CelDraw(&tGfxData, nPosX + pEntry->nOffsetX, nPosY + pEntry->nOffsetY, nColor, pOverlayTxt->nTrans, NULL);
        }
    }
}


//  The highlight's brightening, written twice in GFX_DrawUnitCels's bytes: the light's low byte doubled and held to 64..255 (`cmp eax, 40h` /
//  `jg`, `cmp eax, 0FFh` / `jl`, signed), the other three bytes kept (`xor cl, cl` / `or ecx, eax`). A helper of ours, not the original's.
static inline uint32_t GFX_BrightenLight(uint32_t dwLight)
{
    int32_t nLevel;

    nLevel = 2 * (int32_t)(dwLight & 0xFF);
    if (nLevel <= 64)
        nLevel = 64;
    else if (nLevel >= 255)
        nLevel = 255;

    return (dwLight & 0xFFFFFF00u) | (uint32_t)nLevel;
}


//1.10f: D2Client.0x6FB5A720
//  A unit's cels drawn -- every unit UNITGFX_GfxDrawUnit draws, and the ghost copies SKILLS_DrawMotionTrail and SKILLS_DrawSpinCopy make;
//  UNITGFX_GfxDrawUnit reads the answer, 1 for drawn. Nothing for a tile. First the hit flash, when CUNIT_ShouldDrawUnitComponents spends a tick
//  of its counter (GFX_DrawUnitHilight). Then the type, class and mode converted (line 2788), the particle block's offsets added to nX and to nY
//  (Z too, on Y), and for an object its objects.txt row asserted (`ptStats`, 2801): none drawn if its Draw is 0, else its Xoffset and Yoffset
//  added.
//  A player, monster or object is drawn layer by layer -- the block below, which the original inlines here (Gfx.cpp:1321..1418). Anything else is
//  one cel: the direction, the frame, the palette shift of a unit with a palette index that is not the player (D2Win's #10175), gold's pile in
//  place of the direction (as GFX_DrawInventoryItem picks it, unsigned), the request filled and the sprite validated -- either failing returns
//  0 -- then the position (the object offsets added a second time: a no-op, no object comes this way), the panel shift in perspective, and the
//  light brightened for the unit SELECT_GetSelectedUnit names. A missile is drawn in missiles.txt's Trans (1 DRAWMODE_MODULATE, 2 DRAWMODE_BURN,
//  else DRAWMODE_NORMAL; DRAWMODE_HIGHLIGHT when selected) with the green-blood remap for a LocalBlood missile when green blood is on, else the
//  palette shift, and nothing more; anything else in DRAWMODE_NORMAL (HIGHLIGHT when selected) with an item's own colour palette (D2Common's
//  ITEMS_GetColor) or the palette shift, then, with bDrawCel, its overlays (pass 0). 1 for drawn.
//  bGhostly: set, every layer is drawn DRAWMODE_TRANS50 and neither highlighted nor flat -- UNITGFX_GfxDrawUnit passes MONSTER_IsGhostly's answer
//  for a monster, the motion trail 1 and the spin copy 1 for a player. The result is `int`: UNITGFX_GfxDrawUnit tests it.
int32_t __fastcall GFX_DrawUnitCels(D2UnitStrc *pUnit, int32_t nColor, int32_t nX, int32_t nY, int32_t bGhostly, int32_t bDrawCel)
{
    D2ObjectsTxt *pObjectsTxt;
    D2MonStats2Txt *pMonStats2Txt;
    D2MissilesTxt *pMissilesTxt;
    D2InventoryStrc *pInventory;
    D2UnitStrc *pOwner;
    uint8_t *pCofData;
    uint8_t *pCof;
    uint8_t *pLayerOrder;
    uint8_t *pShiftTable;
    uint8_t *pLayerPalette;
    uint8_t *pCelPalette;
    uint8_t *pPalette;
    D2GfxDataStrc tGfxData;
    D2GfxDataStrc tCelData;
    int32_t bScreenPerspective;
    int32_t bLayerPerspective;
    int32_t bUnitPerspective;
    BOOL bForcedOff;
    BOOL bHighlight;
    BOOL bTransLevel;
    BOOL bFlatSprite;
    BOOL bComponentColor;
    BOOL bSelected;
    uint32_t dwLight;
    int32_t nType;
    int32_t nClass;
    int32_t nMode;
    int32_t nUnitType;
    int32_t nClassId;
    int32_t nAnimMode;
    int32_t nOffsetX;
    int32_t nOffsetY;
    int32_t nPosY;
    int32_t nXPos;
    int32_t nYPos;
    int32_t nPanels;
    int32_t nPrecisionX;
    int32_t nPrecisionY;
    int32_t eDrawMode;
    int32_t nFrame;
    int32_t nFrameCount;
    int32_t nWeaponClassCode;
    int32_t nWeaponClassId;
    int32_t nUnitDirection;
    int32_t nCelDirection;
    int32_t nNumDirections;
    int32_t nItemClassId;
    int32_t iLayer;
    int32_t nComponent;
    int32_t nCelFrame;
    int32_t nCelFrameCount;
    int32_t nClassOut;
    int32_t nCelX;
    int32_t nCelY;
    uint32_t nGold;
    uint8_t nItemColor;

    bScreenPerspective = D2GFX_CheckPerspective();
    if (pUnit && pUnit->dwUnitType == UNIT_TILE)
        return 0;

    if (CUNIT_ShouldDrawUnitComponents(pUnit))
        GFX_DrawUnitHilight(pUnit);

    nType = pUnit ? (int32_t)pUnit->dwUnitType : (int32_t)UNIT_TYPES_COUNT;
    nClass = pUnit ? (int32_t)pUnit->dwClassId : -1;
    nMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
    D2COMMON_11013_ConvertMode(pUnit, &nType, &nClass, &nMode, szGfxCpp, 2788);

    nOffsetX = 0;
    nOffsetY = 0;
    nX += PARTICLE_GetUnitParticleOffsetX(pUnit);
    nPosY = nY + PARTICLE_GetUnitParticleOffsetY(pUnit);
    nPosY += PARTICLE_GetUnitParticleOffsetZ(pUnit);
    if (nType == UNIT_OBJECT)
    {
        pObjectsTxt = D2COMMON_DATATBLS_GetObjectsTxtRecord(nClass);
        if (!pObjectsTxt)
        {
            FOG_DisplayAssert((uint32_t)"ptStats", (uint32_t)szGfxCpp, 2801);
            exit(-1);
        }

        if (!pObjectsTxt->nDraw)
            return 0;

        nOffsetX = pObjectsTxt->dwXOffset;
        nOffsetY = pObjectsTxt->dwYOffset;
        nX += nOffsetX;
        nPosY += nOffsetY;
    }

    if (GFXUTIL_IsDrawableUnitType(pUnit))
    {
        //  The layer draw of a player, monster or object, written out in place (Gfx.cpp:1321..1418 in the original).
        //  The type, class and mode converted again (1321); the position less the viewport origin, a quarter screen by the panels in perspective;
        //  for
        //  the player's own unit drawn as a player with D2Gfx's unit and screen perspective on, the unit perspective turned off, the layers drawn
        //  not
        //  flat, and the flag handed to the overlays (their 8 up). The cel context resolved; with bDrawCel the overlays of pass 1; the frame;
        //  D2Win's
        //  palette shift for a palette index (#10175), the weapon class code from the inventory the unit is drawn with, the animation node got or
        //  rebuilt, the mode's COF ("Unable to get cof file ...", 1398), PalShift.cpp's unit shift table when there is no palette shift, the
        //  highlight
        //  (CUNIT_ShouldHighlightUnit), the direction resolved, the layer order (asserted, `lpbCompByte`, 1418); an object whose objects.txt
        //  OrderFlag
        //  for the mode is 1 is drawn flat in perspective (D2Gfx's CelFlatSpriteDraw at the precision position); a unit under the green state mask
        //  (D2Common's #10561) takes no component colour, unless it is the player in 3D.
        //  Then each layer: COMPOSIT_SPECIAL8 has no palette but, for a monster whose monstats2.txt LocalBlood is set with green blood on, the
        //  green-blood remap; COMPOSIT_SPECIAL7 is not drawn but draws the owner, through UNITGFX_GfxDrawUnit, when the owner is a missile or in
        //  STATE_ATTACHED; any other the shift table. The request filled (its item class seeded -1); the draw mode GFXUTIL_ResolveUnitTransLevel
        //  sets
        //  (5 by default); the component colour; and then by the COF entry's override flag, bGhostly, the trans level and the highlight: TRANS50
        //  for a
        //  ghost, the COF layer's own level, or DRAWMODE_HIGHLIGHT with the light brightened. The loop's index is signed here (`jl` 0x6FB5AE05),
        //  where
        //  GFX_DrawUnitShadow's is not. Last, with bDrawCel, the overlays of pass 0 with the flag, and the unit perspective put back.
        dwLight = (uint32_t)nColor;
        nUnitType = pUnit ? (int32_t)pUnit->dwUnitType : (int32_t)UNIT_TYPES_COUNT;
        nClassId = pUnit ? (int32_t)pUnit->dwClassId : -1;
        nAnimMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
        D2COMMON_11013_ConvertMode(pUnit, &nUnitType, &nClassId, &nAnimMode, szGfxCpp, 1321);
        nXPos = nX - SCROLL_GetViewportOriginX();
        nYPos = nPosY - SCROLL_GetViewportOriginY();
        bLayerPerspective = D2GFX_CheckPerspective();
        nPanels = SCROLL_GetUiShiftScreen();
        if (bLayerPerspective)
        {
            switch (nPanels)
            {
            case PANEL_RIGHT:
                nXPos += -(gnScreenWidth / 4);
                break;

            case PANEL_LEFT:
                nXPos += gnScreenWidth / 4;
                break;
            }
        }

        bForcedOff = FALSE;
        bUnitPerspective = D2GFX_CheckUnitPerspective();
        if (bUnitPerspective && bLayerPerspective && pUnit == CUNIT_GetPlayer() && nUnitType == UNIT_PLAYER)
        {
            D2GFX_SetUnitPerspective(0);
            bForcedOff = TRUE;
            bLayerPerspective = 0;
        }

        if (!GFX_IsUnitCofOnScreen(pUnit, nXPos, nYPos, 1, 0))
        {
            if (bForcedOff)
                D2GFX_SetUnitPerspective(bUnitPerspective);
            return 0;
        }

        if (bDrawCel)
            GFX_DrawUnitOverlays(pUnit, (uint32_t)nColor, 1, -1, -1, bForcedOff);

        D2COMMON_UNITS_GetShiftedFrameMetrics(pUnit, &nFrame, &nFrameCount);
        pShiftTable = NULL;
        if (pUnit->nPaletteIndex)
            pShiftTable = D2WIN_10175_GetPL2Shift(pUnit->nPaletteIndex - 1);

        pInventory = GFXUTIL_GetUnitDisplayInventory(pUnit, NULL);
        nWeaponClassCode = D2COMMON_COMPOSIT_GetWeaponClassId(pUnit, pInventory, &nWeaponClassId, nAnimMode, FALSE);

        //  BUG: the unit perspective is not put back on two exits, as in GFX_DrawUnitShadow: a failed rebuild (`jnz` not taken, 0x6FB5AA02,
        //  returning its
        //  0) and the missing COF's trace (0x6FB5AA60) leave it off; the cel-context exit (0x6FB5A95D) and the end (0x6FB5AE2C) put it back.
        //  NOT ESTABLISHED: it is forced off only for the local player drawn as a player, so it needs the player's own COF to fail.
        if (!GFX_GetUnitAnimNode(pUnit, nWeaponClassCode, -1) && !GFX_RebuildUnitGfx(pUnit, nWeaponClassCode, -1))
            return 0;

        pCofData = (uint8_t *)GFX_GetUnitCofData(pUnit, nAnimMode);
        if (!pCofData)
        {
            FOG_Trace("Unable to get cof file for unit type:%d  class:%d  mode:%d in file:%s  line:%d", nUnitType, nClassId, nAnimMode, szGfxCpp, 1398);
            return 0;
        }

        pCof = (uint8_t *)GFXUTIL_ReturnArg(pCofData);
        if (!pShiftTable)
            pShiftTable = PALSHIFT_GetUnitShiftTable(pUnit);

        bHighlight = CUNIT_ShouldHighlightUnit(pUnit);
        nUnitDirection = D2COMMON_UNITS_GetDirection(pUnit);
        GFXUTIL_ResolveAnimDirection(pUnit, (D2C_UnitTypes)nUnitType, nClassId, nAnimMode, &nUnitDirection, &nCelDirection, &nNumDirections, pCof[2], 1);
        pLayerOrder = GFXUTIL_GetCofLayerOrder(pCofData, nCelDirection, nFrame);
        if (!pLayerOrder)
        {
            FOG_DisplayAssert((uint32_t)"lpbCompByte", (uint32_t)szGfxCpp, 1418);
            exit(-1);
        }

        bFlatSprite = FALSE;
        if (nUnitType == UNIT_OBJECT)
        {
            //  BUG: the objects.txt row is used with no NULL test (`cmp byte ptr [eax+ecx+131h], 1` 0x6FB5AB32), where the head of this function
            //  asserts
            //  the same row. UNREACHABLE: D2Common's getter asserts both bounds and exits.
            pObjectsTxt = D2COMMON_DATATBLS_GetObjectsTxtRecord(pUnit->dwClassId);
            if (pObjectsTxt->nOrderFlag[nAnimMode] == 1)
                bFlatSprite = TRUE;
        }

        bComponentColor = TRUE;
        if (D2COMMON_STATES_CheckStateMaskGreenOnUnit(pUnit) && (pUnit != CUNIT_GetPlayer() || !DRAW_IsDisplayType3D()))
            bComponentColor = FALSE;

        for (iLayer = 0; iLayer < pCof[0]; ++iLayer)
        {
            pLayerPalette = pShiftTable;
            nComponent = pLayerOrder[iLayer];
            if (nComponent == COMPOSIT_SPECIAL8)
            {
                pLayerPalette = NULL;
                if (nUnitType == UNIT_MONSTER)
                {
                    pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord(pUnit->dwClassId);
                    if (pMonStats2Txt && pMonStats2Txt->nLocalBlood && GAME_IsGreenBloodEnabled())
                        pLayerPalette = PALSHIFT_GetGreenBloodRemap();
                }
            }
            else if (nComponent == COMPOSIT_SPECIAL7)
            {
                pOwner = CUNIT_GetOwnerUnit(pUnit);
                if (pOwner && (pOwner->dwUnitType == UNIT_MISSILE || D2COMMON_STATES_CheckState(pOwner, STATE_ATTACHED)))
                    UNITGFX_GfxDrawUnit(pOwner, dwLight, 1);
                continue;
            }

            memset(&tGfxData, 0, sizeof(tGfxData));
            nItemClassId = -1;
            if (!GFXUTIL_FillGfxData(&tGfxData, pUnit, NULL, nUnitDirection, nFrame, &nItemClassId, nComponent, -1))
                continue;

            eDrawMode = DRAWMODE_NORMAL;
            bTransLevel = GFXUTIL_ResolveUnitTransLevel(pUnit, (D2Composits)nComponent, &eDrawMode, nItemClassId);
            if (bComponentColor && (nUnitType != UNIT_PLAYER || pUnit->nPaletteIndex != 0x6C))
                pLayerPalette = GFXUTIL_GetComponentColor(pUnit, nComponent, pLayerPalette);

            if (GFXUTIL_GetCofLayerOverrideTransLvl(pCofData, nComponent))
            {
                if (bGhostly)
                    eDrawMode = DRAWMODE_TRANS50;
                else if (!bTransLevel)
                    eDrawMode = GFXUTIL_GetCofLayerTransLvl(pCofData, nComponent);

                if (bLayerPerspective && bFlatSprite)
                {
                    nPrecisionX = D2COMMON_UNITS_GetPrecisionX(pUnit);
                    nPrecisionY = D2COMMON_UNITS_GetPrecisionY(pUnit);
                    D2GFX_TEXTURE_CelFlatSpriteDraw(&tGfxData, nPrecisionX, nPrecisionY, dwLight, eDrawMode, nPanels, pLayerPalette);
                }
                else
                {
                    D2GFX_TEXTURE_CelDraw(&tGfxData, nXPos, nYPos, dwLight, eDrawMode, pLayerPalette);
                }
            }
            else if (bHighlight || bTransLevel)
            {
                if (bGhostly)
                {
                    eDrawMode = DRAWMODE_TRANS50;
                }
                else if (!bTransLevel && bHighlight)
                {
                    //  BUG: the brightened light is kept: dwGamma is written back (`mov [esp+124h+dwGamma], ecx` 0x6FB5ADCC), so each highlighted
                    //  layer doubles
                    //  the light the layer before it doubled, and the pass-0 overlays and an owner drawn through COMPOSIT_SPECIAL7 get the result.
                    //  Reproduced.
                    //  REACHABLE: the selected unit's layers in light below 128 brighten one after the other.
                    eDrawMode = DRAWMODE_HIGHLIGHT;
                    dwLight = GFX_BrightenLight(dwLight);
                }

                D2GFX_TEXTURE_CelDraw(&tGfxData, nXPos, nYPos, dwLight, eDrawMode, pLayerPalette);
            }
            else if (bGhostly)
            {
                eDrawMode = DRAWMODE_TRANS50;
                D2GFX_TEXTURE_CelDraw(&tGfxData, nXPos, nYPos, dwLight, eDrawMode, pLayerPalette);
            }
            else if (bLayerPerspective && bFlatSprite)
            {
                nPrecisionX = D2COMMON_UNITS_GetPrecisionX(pUnit);
                nPrecisionY = D2COMMON_UNITS_GetPrecisionY(pUnit);
                D2GFX_TEXTURE_CelFlatSpriteDraw(&tGfxData, nPrecisionX, nPrecisionY, dwLight, DRAWMODE_NORMAL, nPanels, pLayerPalette);
            }
            else
            {
                D2GFX_TEXTURE_CelDraw(&tGfxData, nXPos, nYPos, dwLight, DRAWMODE_NORMAL, pLayerPalette);
            }
        }

        if (bDrawCel)
            GFX_DrawUnitOverlays(pUnit, dwLight, 0, -1, -1, bForcedOff);

        if (bForcedOff)
            D2GFX_SetUnitPerspective(bUnitPerspective);

        return 1;
    }

    nCelDirection = D2COMMON_UNITS_GetDirection(pUnit);
    D2COMMON_UNITS_GetShiftedFrameMetrics(pUnit, &nCelFrame, &nCelFrameCount);
    pCelPalette = NULL;
    if (pUnit->nPaletteIndex && pUnit != CUNIT_GetPlayer())
        pCelPalette = D2WIN_10175_GetPL2Shift(pUnit->nPaletteIndex - 1);

    if (nType == UNIT_ITEM && D2COMMON_ITEMS_GetItemType(pUnit) == ITEMTYPE_GOLD)
    {
        nGold = (uint32_t)D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_GOLD, 0);
        if (nGold < 100)
            nCelDirection = 0;
        else if (nGold < 500)
            nCelDirection = 1;
        else
            nCelDirection = 3 - (nGold < 5000);
    }

    memset(&tCelData, 0, sizeof(tCelData));
    if (!GFXUTIL_FillGfxData(&tCelData, pUnit, NULL, nCelDirection, nCelFrame, &nClassOut, 1, -1))
        return 0;

    if (!D2CMP_SpriteValidate(&tCelData, FALSE, TRUE))
        return 0;

    nCelX = nOffsetX + nX - SCROLL_GetViewportOriginX();
    nCelY = nPosY + nOffsetY - SCROLL_GetViewportOriginY();
    if (bScreenPerspective)
    {
        switch (SCROLL_GetUiShiftScreen())
        {
        case PANEL_RIGHT:
            nCelX += -(gnScreenWidth / 4);
            break;

        case PANEL_LEFT:
            nCelX += gnScreenWidth / 4;
            break;
        }
    }

    bSelected = SELECT_GetSelectedUnit() == pUnit;
    if (bSelected)
        nColor = (int32_t)GFX_BrightenLight((uint32_t)nColor);

    if (nType == UNIT_MISSILE)
    {
        //  The missiles.txt record lookup, written out in place.
        nClassId = (int32_t)pUnit->dwClassId;
        if (nClassId < 0 || nClassId >= sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = NULL;
        else
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nClassId];

        //  BUG: the missiles.txt row is read whether or not the lookup found one: a class outside the table makes the copy's NULL (`xor ecx, ecx`
        //  0x6FB5B00F) and `mov al, [ecx+18Dh]` (0x6FB5B011) reads it. Reproduced. LATENT: MISSILE_InitClientUnit refuses such a class before the
        //  unit
        //  exists.
        eDrawMode = DRAWMODE_NORMAL;
        if (pMissilesTxt->nTrans == 1)
            eDrawMode = DRAWMODE_MODULATE;
        if (pMissilesTxt->nTrans == 2)
            eDrawMode = DRAWMODE_BURN;
        if (bSelected)
            eDrawMode = DRAWMODE_HIGHLIGHT;

        if (pMissilesTxt->nLocalBlood && GAME_IsGreenBloodEnabled())
            pPalette = PALSHIFT_GetGreenBloodRemap();
        else
            pPalette = pCelPalette;

        D2GFX_TEXTURE_CelDraw(&tCelData, nCelX, nCelY, (uint32_t)nColor, eDrawMode, pPalette);
        return 1;
    }

    eDrawMode = bSelected ? DRAWMODE_HIGHLIGHT : DRAWMODE_NORMAL;
    pPalette = nType == UNIT_ITEM ? D2COMMON_ITEMS_GetColor(NULL, pUnit, &nItemColor, 0) : NULL;
    if (!pPalette)
        pPalette = pCelPalette;

    D2GFX_TEXTURE_CelDraw(&tCelData, nCelX, nCelY, (uint32_t)nColor, eDrawMode, pPalette);
    if (bDrawCel)
        GFX_DrawUnitOverlays(pUnit, (uint32_t)nColor, 0, -1, -1, 0);

    return 1;
}


//1.10f: D2Client.0x6FB5B0F0
//  A filled box from a corner and a size. D2Gfx's DrawBox takes two corners, so this adds the width to x and the height to y (`add eax, ecx` at
//  0x6FB5B105, `add eax, edx` at 0x6FB5B0FE) and passes the colour and the draw mode on as they came, each loaded from its stack slot and pushed
//  whole (0x6FB5B0F0..0x6FB5B0F9). `retn 10h`: nX and nY in ecx and edx, the other four on the stack. The colour is a byte at every hop: the
//  render drivers mask it to its byte, so bits 8..31 are read by nobody.
void __fastcall GFX_DrawFilledRect(int32_t nX, int32_t nY, int32_t nWidth, int32_t nHeight, uint8_t nColor, int32_t eDrawMode)
{
    D2GFX_DrawBox(nX, nY, nX + nWidth, nY + nHeight, nColor, eDrawMode);
}


//1.10f: D2Client.0x6FB5B120
//  A unit's graphics reset, sixteen times over. The body is one loop of eComponent from 0 to 15 (`cmp eax, 10h` / `jl` 0x6FB5B182..0x6FB5B189),
//  and each pass asserts its own counter -- `eComponent < NUM_COMPONENTS`, Gfx.cpp:1627 (`cmp dword ptr [esp+0Ch], 10h` 0x6FB5B12E, `push 65Bh`
//  0x6FB5B190), which cannot fire -- clears the first sixteen mode-loaded flags through GFXUTIL_SetUnitModeLoaded (0x6FB5B137..0x6FB5B146),
//  asserts the gfx block (1636, `push 664h` 0x6FB5B1AE), frees the animation list node by node (FOG_Free, 1641, `push 669h` 0x6FB5B161) and
//  clears its head (0x6FB5B17A). Only the first pass frees anything; the other fifteen clear the same sixteen flags again and find the list
//  empty. Lines 1627..1641 are pushed by no other function of the unit. void: eax leaves the pass count, 16 (`mov eax, [esp+0Ch]` / `inc eax`
//  0x6FB5B176..0x6FB5B181), and neither caller reads it.
//  BUG: the mode loop stops at 16 (`cmp esi, 10h` 0x6FB5B143) where the block holds twenty flags (wModeLoaded[20]; GFX_SetAllUnitModesLoaded
//  writes all twenty). PLRMODE_SPECIAL4, PLRMODE_DEAD, PLRMODE_SEQUENCE and PLRMODE_KNOCKBACK (16..19) keep their flag while the list that held
//  their nodes is freed. INERT: the flags are read only by GFXUTIL_IsUnitModeLoaded, for a monster's modes 0..15 and an object's 0..7, so no
//  player's flag is ever read and no later load is skipped.
void __fastcall GFX_ResetUnitGfx(D2UnitStrc *pUnit)
{
    int32_t eComponent;
    int32_t nMode;
    D2ClientUnitGfxInfoStrc *pGfxInfo;
    D2ClientCofNodeStrc *pNode;
    D2ClientCofNodeStrc *pNext;

    for (eComponent = 0; eComponent < (int32_t)NUM_COMPONENTS; ++eComponent)
    {
        if (eComponent >= (int32_t)NUM_COMPONENTS)
        {
            FOG_DisplayAssert((uint32_t)"eComponent < NUM_COMPONENTS", (uint32_t)szGfxCpp, 1627);
            exit(-1);
        }

        //  16 bare, as the bytes have it: NUM_COMPONENTS and NUMBER_OF_MONMODES by value.
        for (nMode = 0; nMode < 16; ++nMode)
            GFXUTIL_SetUnitModeLoaded(pUnit, nMode, 0);

        pGfxInfo = CUNIT_GfxGetUnitGfxInfo(pUnit);
        if (!pGfxInfo)
        {
            FOG_DisplayAssert((uint32_t)"ptGfxInfo", (uint32_t)szGfxCpp, 1636);
            exit(-1);
        }

        for (pNode = pGfxInfo->pAnimList; pNode; pNode = pNext)
        {
            pNext = pNode->pNext;
            FOG_Free((uint32_t)pNode, (uint32_t)szGfxCpp, 1641, 0);
        }

        pGfxInfo->pAnimList = NULL;
    }
}


//1.10f: D2Client.0x6FB5B1D0
//  Every mode-loaded flag of a unit set to one value: bLoaded written into the low byte of each of the block's twenty wModeLoaded entries
//  (`mov [eax+ecx*2], bl` 0x6FB5B1FC, `cmp ecx, 14h` 0x6FB5B200) -- twenty, where GFX_ResetUnitGfx clears sixteen. The block is asserted,
//  Gfx.cpp:2990 (`push 0BAEh` 0x6FB5B1DC). Every caller passes 0 (`xor dl, dl` before each call), so bLoaded is a parameter no call in this
//  binary sets to anything but 0. void: eax leaves the gfx block, and no caller reads it.
void __fastcall GFX_SetAllUnitModesLoaded(D2UnitStrc *pUnit, uint8_t bLoaded)
{
    D2ClientUnitGfxInfoStrc *pGfxInfo;
    int32_t nMode;

    pGfxInfo = CUNIT_GfxGetUnitGfxInfo(pUnit);
    if (!pGfxInfo)
    {
        FOG_DisplayAssert((uint32_t)"ptGfxInfo", (uint32_t)szGfxCpp, 2990);
        exit(-1);
    }

    for (nMode = 0; nMode < 20; ++nMode) *(uint8_t *)&pGfxInfo->wModeLoaded[nMode] = (uint8_t)bLoaded;
}


//1.10f: D2Client.0x6FB5B210
//  The preload sweep over the rooms around the player. GAME_UpdateFrame calls it twice (0x6FAAA0A3, 0x6FAAA1DC) through an inlined
//  CLIENT_MainUpdate.
//  Unless a pass is active (gbPreloadPassActive), a sweep runs at most once in 600 ms: `GetTickCount() < gdwPreloadThrottleTick + 258h` returns
//  at once, unsigned (`jb` 0x6FB5B236), without touching the tick. A sweep walks the rooms adjacent to the player's (D2Common's #10035) and every
//  unit in each: the unit's drawn type, class and mode through #11013 (Gfx.cpp:1954, `push 7A2h` 0x6FB5B304); a monster within monstats.txt --
//  unless it is MONMODE_DEAD and its DEAD mode is preloaded already -- has its modes preloaded (GFX_PreloadAllAnimModes), a player its
//  (GFX_PreloadAllPlayerModes), anything else nothing. While no pass is active the sweep stops once 30 or more of D2CMP's asynchronous requests
//  are pending (#10057, `jnb` 0x6FB5B2BD, 0x6FB5B559), and the tick is stamped on every way out but the throttle's.
//  bAsync is handed down to every load, and it gates the loading screen. During an active pass, a sweep that is synchronous, or must preload
//  (gbPreloadRequired, D2Win's #10185), counts the rooms and draws the loading screen for each whose count is not a multiple of 32
//  (`and dl, 1Fh` / `test dl, dl` / `jz` 0x6FB5B297..0x6FB5B2A1). A player met while gbPreloadRequired is set clears bAsync (`mov [esp+14h], ebp`
//  0x6FB5B456) -- for that player and every unit after it in the sweep, monsters included: it applies to the units before the first player only
//  while the archive must be preloaded. void; bAsync is passed in ecx (copied to the frame at 0x6FB5B23E).
//  The original inlines GFX_PreloadAllAnimModes and GFX_PreloadAllPlayerModes here.
//  BUG: the loading screen is drawn for 31 rooms in 32 and skipped on the multiples -- a test that reads inverted, which the bytes cannot settle.
//  REACHABLE: on every such sweep; whether it is a defect is NOT ESTABLISHED.
//  BUG: the cleared bAsync is kept for the rest of the sweep. REACHABLE: where gbPreloadRequired is 1 -- D2Win's #10185 finding no d2char.mpq in
//  the install or the current directory, a minimum install.
void __fastcall GFX_PreloadVisibleUnits(int32_t bAsync)
{
    D2ActiveRoomStrc *pRoom;
    D2ActiveRoomStrc **ppRoomList;
    int32_t nNumRooms;
    uint32_t iRoom;
    int32_t nLoadingCount;
    D2UnitStrc *pUnit;
    int32_t nType;
    int32_t nClass;
    int32_t nMode;

    if (!gbPreloadPassActive && GetTickCount() < gdwPreloadThrottleTick + 600)
        return;

    pRoom = D2COMMON_UNITS_GetRoom(CUNIT_GetPlayer());
    if (pRoom)
    {
        D2COMMON_DUNGEON_GetAdjacentRoomsListFromRoom(pRoom, &ppRoomList, &nNumRooms);
        nLoadingCount = 0;
        for (iRoom = 0; iRoom < (uint32_t)nNumRooms; ++iRoom)
        {
            if (gbPreloadPassActive && (!bAsync || gbPreloadRequired) && (nLoadingCount++ & 0x1F))
                UI_DrawLoadingScreen(1);

            if (!gbPreloadPassActive && (uint32_t)D2CMP_10057() >= 30)
                break;

            for (pUnit = ppRoomList[iRoom]->pUnitFirst; pUnit; pUnit = pUnit->pRoomNext)
            {
                nType = pUnit ? (int32_t)pUnit->dwUnitType : (int32_t)UNIT_TYPES_COUNT;
                nClass = pUnit ? (int32_t)pUnit->dwClassId : -1;
                nMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
                D2COMMON_11013_ConvertMode(pUnit, &nType, &nClass, &nMode, szGfxCpp, 1954);

                if (nType == UNIT_MONSTER)
                {
                    if (nClass < sgptDataTables->nMonStatsTxtRecordCount && (nMode != MONMODE_DEAD || !gpMonsterPreloadTable[nClass].bLoaded[MONMODE_DEAD]))
                    {
                        GFX_PreloadAllAnimModes(pUnit, bAsync);
                    }
                }
                else if (nType == UNIT_PLAYER)
                {
                    if (gbPreloadRequired)
                        bAsync = 0;

                    GFX_PreloadAllPlayerModes(pUnit, bAsync);
                }

                if (!gbPreloadPassActive && (uint32_t)D2CMP_10057() >= 30)
                    goto SWEEP_DONE;
            }
        }
    }

SWEEP_DONE:
    gdwPreloadThrottleTick = GetTickCount();
}


//1.10f: D2Client.0x6FB5B5A0
//  One mode of a unit loaded -- every layer of its COF, asynchronously or not. Its own failure trace names it, "sAsyncLoadBlock failed on
//  async:%d unittype:%d ..." (the literal pushed at 0x6FB5B6D1; the `s` is the source's prefix of a static function).
//  The unit's weapon class code is taken (D2Common's #10888, over the owner's inventory for a shapeshifted unit), its animation node for the mode
//  and that class found or built (GFX_GetUnitAnimNode; GFX_RebuildUnitGfx), the mode's COF fetched (GFX_GetUnitCofData). A rebuild that fails is
//  traced with the request -- bAsync, the unit's type and class, the mode, the weapon class id and code -- and answers 0; a COF that is missing is
//  traced "Unable to get cof file..." with Gfx.cpp:1719 and answers 0. Then the unit's drawn type and class (#11013, Gfx.cpp:1733, `push 6C5h`
//  0x6FB5B7C2) pick the cel direction (GFXUTIL_ResolveAnimDirection, the COF's direction count, bMayRemap 0), and every layer the COF orders for it
//  at frame 0 is asserted below NUM_COMPONENTS (1749) and filled in a cleared request -- the unit's facing, frame 0, the mode as the override --
//  and validated by D2CMP's SpriteValidate with bAsync for both its flags. A layer that does not validate has its cel file cleared and goes to
//  #10056; a 0 from it answers -1 at once, and a request left without flag 4 (fFlags) makes the answer 0. Otherwise 1. Every caller compares the
//  answer with 1.
//  The converted mode is not read: #11013 is asked for the type and class (`mov edx, [esp+1A4h+nConvType]` 0x6FB5B7F5, `mov edx, [esp+194h+
//  nConvClass]` 0x6FB5B7E9), and the mode handed on is the one being loaded (`push ebx` 0x6FB5B7F3) -- the unit's current mode, which #11013
//  converted, is not it. FillGfxData's class out-parameter lands in the slot that held nAnimMode (`lea edx, [esp+190h+nAnimMode2]` 0x6FB5B85A),
//  dead by then; ours has a local of its own.
//  The original inlines GFXUTIL_GetUnitDisplayInventory, GFX_GetUnitAnimNode and GFX_GetUnitCofData here.
//  BUG: nAnimMode -1 would be resolved to the drawn mode for the node (the inlined lookup's `cmp ebx, 0FFFFFFFFh` 0x6FB5B62F) and not for the
//  COF, whose search compares the -1 (`cmp [eax], ebx` 0x6FB5B70D) and so traces "Unable to get cof file". UNREACHABLE: no call site passes -1;
//  they pass a mode read from gPreloadAnimModes or gPreloadComponents.
int32_t __fastcall GFX_AsyncLoadBlock(D2UnitStrc *pUnit, int32_t nAnimMode, int32_t bAsync)
{
    D2InventoryStrc *pInventory;
    int32_t nWeaponClassCode;
    int32_t nWeaponClassId;
    void *pCofData;
    uint8_t *pCof;
    int32_t nUnitDirection;
    int32_t nCelDirection;
    int32_t nNumDirections;
    int32_t nConvType;
    int32_t nConvClass;
    int32_t nConvMode;
    uint8_t *pLayerOrder;
    int32_t iLayer;
    int32_t nComponent;
    int32_t nClassOut;
    D2GfxDataStrc tGfxData;
    int32_t bResult;
    char szBuffer[256];

    bResult = 1;

    pInventory = GFXUTIL_GetUnitDisplayInventory(pUnit, NULL);
    nWeaponClassCode = D2COMMON_COMPOSIT_GetWeaponClassId(pUnit, pInventory, &nWeaponClassId, nAnimMode, FALSE);

    if (!GFX_GetUnitAnimNode(pUnit, nWeaponClassCode, nAnimMode) && !GFX_RebuildUnitGfx(pUnit, nWeaponClassCode, nAnimMode))
    {
        sprintf(szBuffer, "sAsyncLoadBlock failed on async:%d unittype:%d unitclass:%d unitmode:%d weaponclass:%d wclass:%d\n", bAsync, (int32_t)pUnit->dwUnitType, (int32_t)pUnit->dwClassId, nAnimMode, nWeaponClassId, nWeaponClassCode);
        FOG_Trace(szBuffer);
        return 0;
    }

    pCofData = GFX_GetUnitCofData(pUnit, nAnimMode);
    if (!pCofData)
    {
        FOG_Trace("Unable to get cof file for unit type:%d  class:%d  mode:%d in file:%s  line:%d", (int32_t)pUnit->dwUnitType, (int32_t)pUnit->dwClassId, D2COMMON_UNITS_GetAnimOrSeqMode(pUnit), szGfxCpp, 1719);
        return 0;
    }

    pCof = (uint8_t *)GFXUTIL_ReturnArg(pCofData);
    if (!pCof)
    {
        FOG_DisplayAssert((uint32_t)"ptCofHeader", (uint32_t)szGfxCpp, 1723);
        exit(-1);
    }

    nCelDirection = 0;
    nUnitDirection = D2COMMON_UNITS_GetDirection(pUnit);
    nConvType = (int32_t)pUnit->dwUnitType;
    nConvClass = (int32_t)pUnit->dwClassId;
    nConvMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
    D2COMMON_11013_ConvertMode(pUnit, &nConvType, &nConvClass, &nConvMode, szGfxCpp, 1733);
    GFXUTIL_ResolveAnimDirection(pUnit, (D2C_UnitTypes)nConvType, nConvClass, nAnimMode, &nUnitDirection, &nCelDirection, &nNumDirections, pCof[2], 0);
    pLayerOrder = GFXUTIL_GetCofLayerOrder((uint8_t *)pCofData, nCelDirection, 0);
    if (!pLayerOrder)
    {
        FOG_DisplayAssert((uint32_t)"lpbCompByte", (uint32_t)szGfxCpp, 1742);
        exit(-1);
    }

    for (iLayer = 0; iLayer < pCof[0]; ++iLayer)
    {
        nComponent = pLayerOrder[iLayer];
        if (nComponent >= (int32_t)NUM_COMPONENTS)
        {
            FOG_DisplayAssert((uint32_t)"eComponent < NUM_COMPONENTS", (uint32_t)szGfxCpp, 1749);
            exit(-1);
        }

        memset(&tGfxData, 0, sizeof(tGfxData));
        if (GFXUTIL_FillGfxData(&tGfxData, pUnit, NULL, D2COMMON_UNITS_GetDirection(pUnit), 0, &nClassOut, nComponent, nAnimMode) && !D2CMP_SpriteValidate(&tGfxData, bAsync, bAsync))
        {
            tGfxData.pCellFile = NULL;
            if (!D2CMP_10056((uint32_t)&tGfxData))
                return -1;

            if (!(tGfxData.fFlags & 4))
                bResult = 0;
        }
    }

    return bResult;
}


//1.10f: D2Client.0x6FB5B920
//  One mode of a monster preloaded, once. The unit's drawn type and class (#11013, Gfx.cpp:1794, `push 702h` 0x6FB5B95B) pick its record of the
//  monster preload table -- gpMonsterPreloadTable, one D2ClientModePreloadStrc per monstats.txt row (GFX_InitCelCache allocates it): twenty
//  bLoaded flags and twenty bQueued after them, read as the bytes do at `[edx+ecx*4]` and `[edx+ecx*4+50h]` with ecx the mode plus forty times
//  the class (0x6FB5B989..0x6FB5B99F), and the four stores the same. A class past monstats.txt does nothing (`jge` 0x6FB5B983). A mode loaded
//  does nothing. A mode queued is asked of D2CMP -- #10058 with the type, the mode and the class, which walks the sprite cache for a record keyed
//  on those three and answers 1 while its asynchronous read is still pending -- and marked loaded once nothing is pending. A mode neither is
//  loaded through GFX_AsyncLoadBlock: an answer of 1 marks it queued, anything else clears both flags, to be tried again.
//  The class is bounded above only: a NULL unit's -1 would index the record before the table. Every call site passes a unit walked to in a room.
//  The mode indexes a half of twenty unchecked; the table's modes run to MONMODE_RUN, 15.
void __fastcall GFX_PreloadAnimMode(D2UnitStrc *pUnit, int32_t nAnimMode, int32_t bAsync)
{
    int32_t nType;
    int32_t nClass;
    int32_t nMode;

    nType = pUnit ? (int32_t)pUnit->dwUnitType : (int32_t)UNIT_TYPES_COUNT;
    nClass = pUnit ? (int32_t)pUnit->dwClassId : -1;
    nMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
    D2COMMON_11013_ConvertMode(pUnit, &nType, &nClass, &nMode, szGfxCpp, 1794);
    if (nClass >= sgptDataTables->nMonStatsTxtRecordCount)
        return;

    if (gpMonsterPreloadTable[nClass].bLoaded[nAnimMode])
        return;

    if (gpMonsterPreloadTable[nClass].bQueued[nAnimMode])
    {
        if (!D2CMP_10058(nType, nAnimMode, nClass))
            gpMonsterPreloadTable[nClass].bLoaded[nAnimMode] = 1;
        return;
    }

    if (GFX_AsyncLoadBlock(pUnit, nAnimMode, bAsync) == 1)
    {
        gpMonsterPreloadTable[nClass].bQueued[nAnimMode] = 1;
    }
    else
    {
        gpMonsterPreloadTable[nClass].bQueued[nAnimMode] = 0;
        gpMonsterPreloadTable[nClass].bLoaded[nAnimMode] = 0;
    }
}


//1.10f: D2Client.0x6FB5BA40
//  A unit's particle block: +30h of its gfx block, or NULL when it has none (`mov eax, [eax+30h]` 0x6FB5BA49, `xor eax, eax` 0x6FB5BA4D). The
//  member is `void *`, so the C casts once.
D2ClientParticleStrc *__fastcall GFX_GetUnitParticles(D2UnitStrc *pUnit)
{
    D2ClientUnitGfxInfoStrc *pGfxInfo;

    pGfxInfo = CUNIT_GfxGetUnitGfxInfo(pUnit);
    if (!pGfxInfo)
        return NULL;

    return (D2ClientParticleStrc *)pGfxInfo->pParticles;
}


//1.10f: D2Client.0x6FB5BA50
//  A unit's particle block stored: pParticle into +30h of its gfx block, and nothing when it has none (`test eax, eax` / `jz` 0x6FB5BA58,
//  `mov [eax+30h], esi` 0x6FB5BA5C). Called by PARTICLE_AllocUnitParticles with a new block and PARTICLE_FreeUnitParticles with NULL, both as
//  tail calls in void functions.
void __fastcall GFX_SetUnitParticles(D2UnitStrc *pUnit, D2ClientParticleStrc *pParticle)
{
    D2ClientUnitGfxInfoStrc *pGfxInfo;

    pGfxInfo = CUNIT_GfxGetUnitGfxInfo(pUnit);
    if (pGfxInfo)
        pGfxInfo->pParticles = pParticle;
}


//1.10f: D2Client.0x6FB5BA70
//  An overlay added to a unit: a new D2ClientGfxEntryStrc at the front of its overlay list, filled from the overlay's overlay.txt row. Nothing is
//  added for an id outside overlay.txt, a row whose version is 100 or more in a classic game (`cmp word ptr [ebp+42h], 64h` / `jge` 0x6FB5BAC3,
//  signed), a unit marked UNITFLAGEX_NODRAW (bit 18 shifted down and masked, 0x6FB5BADD..0x6FB5BAE6), a monster whose monstats2.txt row carries
//  NoOvly (0x6FB5BB46..0x6FB5BB50), or an aura the unit carries already. Any other copy of the overlay is removed first (GFX_RemoveOverlay, the
//  row's PreDraw byte in edx, which it does not read) -- except for the three Bone Armor overlays, 0x8C..0x8E, and the Hand of God's, 0x9E, whose
//  copies stack. The entry is allocated (FOG_Alloc, 168 bytes, Gfx.cpp:3132; the `ptOverlay` assert, 3133), cleared (2Ah dwords), keyed on the id
//  and the kind, and given the animation's frame count in 8.8 (GFXUTIL_GetOverlayFrameCount). ONCE sets field_08 to 1; DELAYED starts on a random
//  frame of the whole animation; any other kind takes nTargetOrStateId, nX and nY into field_08, nX and nY. The draw offsets are the row's, the Y
//  raised by the unit's overlay height (D2Common's #10430: 75 for -1, the row's Height1..4 for 0..3). A row with a Radius gets a light
//  (DLIGHTMAP_AddLight, type 1, InitRadius, intensity 255, the row's colour) easing to Radius when the two differ; the colours are passed as
//  bytes (`mov al, [ebp+7Fh]` 0x6FB5BD03; DLIGHTMAP_AddLight reads each as a byte, 0x6FAA4317..0x6FAA4336). The entry is linked at the head
//  (Gfx.cpp:3182 asserts the block); an aura takes nTargetOrStateId as its state and, walking the list from the head, the active flag and count
//  of the first other aura of the same state, or inactive if an aura of another state is met first that is active, or active with a count of 0
//  if none. Last the start frame, the frame rate (AnimRate << 4) and its jitter, and LoopWaitTime.
//  nRandomStartFrames is a frame count: the body tests it for zero and shifts it left 8 (`shl ecx, 8` 0x6FB5BDB7), frames in the entry's 8.8,
//  as the bound of a random start frame. ITEM_OnUnitModeUpdate (0x6FB2AC88) passes 8 with a DELAYED item gleam, so its random start is within
//  the first 8 frames; the other callers pass 0.
//  The three rolls (the DELAYED frame, the start frame and the jitter) draw from CUNIT_GetPlayer()'s seed (+20h, `mov eax, [ebx+20h]`), never
//  from the unit's, and the player is not tested: a roll with no player would read [0+20h]. A kind that is not DELAYED, a zero count and a zero
//  jitter roll nothing.
//  The original inlines GFXUTIL_GetOverlayTxtRecord, DATATBLS_GetMonStats2TxtRecord, GFX_HasOverlay and SEED_RollLimitedRandomNumber here.
void __fastcall GFX_AddOverlay(D2UnitStrc *pUnit, D2_OverlayIds nOverlayId, D2C_OverlayTypes nKind, int32_t nRandomStartFrames, int32_t nTargetOrStateId, int32_t nX, int32_t nY, int32_t nRateJitter)
{
    D2OverlayTxt *pOverlayTxt;
    D2UnitStrc *pPlayer;
    D2MonStats2Txt *pMonStats2Txt;
    D2ClientUnitGfxInfoStrc *pGfxInfo;
    D2ClientGfxEntryStrc *pEntry;
    D2ClientGfxEntryStrc *pOverlay;

    pOverlayTxt = GFXUTIL_GetOverlayTxtRecord(nOverlayId);
    if (!pOverlayTxt)
        return;

    if (!FOG_IsExpansion() && (int16_t)pOverlayTxt->wVersion >= 100)
        return;

    pPlayer = CUNIT_GetPlayer();
    if (pUnit)
    {
        //  UNITFLAGEX_NODRAW, bit 18, as the bytes test it.
        if ((pUnit->dwFlagEx >> 18) & 1)
            return;

        if (pUnit->dwUnitType == UNIT_MONSTER)
        {
            pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord((int32_t)pUnit->dwClassId);
            if (pMonStats2Txt && (pMonStats2Txt->nFlags[MONSTATS2FLAGINDEX_NOOVLY >> 3] & gdwBitMasks[MONSTATS2FLAGINDEX_NOOVLY & 7]))
            {
                return;
            }
        }
    }

    if (nKind == OVERLAYTYPE_AURA && GFX_HasOverlay(pUnit, nOverlayId))
        return;

    if ((int32_t)nOverlayId < (int32_t)OVERLAY_BONEARMOR_CAST || ((int32_t)nOverlayId > (int32_t)OVERLAY_BONEARMOR_BACK && nOverlayId != OVERLAY_HANDOFGOD))
    {
        GFX_RemoveOverlay(pUnit, pOverlayTxt->nPreDraw, nOverlayId);
    }

    pEntry = (D2ClientGfxEntryStrc *)FOG_Alloc(sizeof(D2ClientGfxEntryStrc), (uint32_t)szGfxCpp, 3132, 0);
    if (!pEntry)
    {
        FOG_DisplayAssert((uint32_t)"ptOverlay", (uint32_t)szGfxCpp, 3133);
        exit(-1);
    }

    memset(pEntry, 0, sizeof(D2ClientGfxEntryStrc));
    pEntry->nOverlayId = nOverlayId;
    pEntry->nKind = nKind;
    pEntry->nFrameCount = GFXUTIL_GetOverlayFrameCount(pUnit, pEntry);
    if (nKind == OVERLAYTYPE_ONCE)
    {
        pEntry->field_08 = 1;
    }
    else if (nKind == OVERLAYTYPE_DELAYED)
    {
        pEntry->nFrame = SEED_RollLimitedRandomNumber(&pPlayer->pSeed, pEntry->nFrameCount);
    }
    else
    {
        pEntry->field_08 = nTargetOrStateId;
        pEntry->nX = nX;
        pEntry->nY = nY;
    }

    pEntry->nOffsetX = (int32_t)pOverlayTxt->dwOffsetX;
    pEntry->nOffsetY = (int32_t)pOverlayTxt->dwOffsetY;
    switch (D2COMMON_UNITS_GetOverlayHeight(pUnit))
    {
    case -1:
        pEntry->nOffsetY += 75;
        break;
    case 0:
        pEntry->nOffsetY += pOverlayTxt->dwHeight[0];
        break;
    case 1:
        pEntry->nOffsetY += pOverlayTxt->dwHeight[1];
        break;
    case 2:
        pEntry->nOffsetY += pOverlayTxt->dwHeight[2];
        break;
    case 3:
        pEntry->nOffsetY += pOverlayTxt->dwHeight[3];
        break;
    }

    if (pOverlayTxt->dwRadius)
    {
        pEntry->pLight = DLIGHTMAP_AddLight(pUnit, 1, pOverlayTxt->dwInitRadius, 255, pOverlayTxt->nRed, pOverlayTxt->nGreen, pOverlayTxt->nBlue);
        if (pOverlayTxt->dwRadius != pOverlayTxt->dwInitRadius)
            DLIGHTMAP_SetTargetRadius(pEntry->pLight, pOverlayTxt->dwRadius);
    }

    pGfxInfo = CUNIT_GfxGetUnitGfxInfo(pUnit);
    if (!pGfxInfo)
    {
        FOG_DisplayAssert((uint32_t)"ptGfxInfo", (uint32_t)szGfxCpp, 3182);
        exit(-1);
    }

    pEntry->bPreDraw = pOverlayTxt->nPreDraw;
    pEntry->pNext = pGfxInfo->pOverlayList;
    pGfxInfo->pOverlayList = pEntry;
    if (pEntry->nKind == OVERLAYTYPE_AURA)
    {
        pEntry->nStateId = nTargetOrStateId;
        for (pOverlay = pGfxInfo->pOverlayList; pOverlay; pOverlay = pOverlay->pNext)
        {
            if (pOverlay->nKind != OVERLAYTYPE_AURA || pOverlay == pEntry)
                continue;

            if (pOverlay->nStateId == nTargetOrStateId)
            {
                pEntry->bAuraActive = pOverlay->bAuraActive;
                pEntry->field_08 = pOverlay->field_08;
                break;
            }

            if (pOverlay->bAuraActive)
            {
                pEntry->bAuraActive = 0;
                break;
            }
        }

        if (!pOverlay)
        {
            pEntry->bAuraActive = 1;
            pEntry->field_08 = 0;
        }
    }

    if (nRandomStartFrames)
    {
        pEntry->nFrame = SEED_RollLimitedRandomNumber(&pPlayer->pSeed, (int32_t)((uint32_t)nRandomStartFrames << 8));
    }

    pEntry->nFrameRate = (int32_t)(pOverlayTxt->dwAnimRate << 4);
    if (nRateJitter)
    {
        pEntry->nFrameRate += SEED_RollLimitedRandomNumber(&pPlayer->pSeed, (int32_t)((uint32_t)nRateJitter << 4));
    }

    pEntry->nLoopWaitTime = (int32_t)pOverlayTxt->dwLoopWaitTime;
}


//1.10f: D2Client.0x6FB5BEA0
//  Whether a unit carries any of the given dwFlagEx bits: 0 for a NULL unit (`xor eax, eax` 0x6FB5BEA4), else `(dwFlagEx & nFlagEx) != 0` --
//  `and eax, edx` / `neg eax` / `sbb eax, eax` / `neg eax` (0x6FB5BEAD..0x6FB5BEB3), 1 for any bit set. Every caller passes 8
//  (UNITFLAGEX_ISSHAPESHIFTED) and reads the answer. The parameter is `int`.
int32_t __fastcall GFX_TestFlagEx(D2UnitStrc *pUnit, int32_t nFlagEx)
{
    if (!pUnit)
        return 0;

    return (pUnit->dwFlagEx & nFlagEx) != 0;
}


//1.10f: D2Client.0x6FB5BEC0
//  One overlay removed from a unit: the first entry of its overlay list whose id is nOverlayId -- or, for a LOOPEX or AURAEX entry, whose field_08
//  or nX is, the two overlays such an entry chains to -- handed to GFX_RemoveGfxEntry, which unlinks and frees it. A NULL unit or a unit with no
//  gfx block removes nothing. The id is looked up in overlay.txt first and asserted, `ptStats` at Gfx.cpp:3229 (`push 0C9Dh` 0x6FB5BF05) -- the
//  row is read for nothing else. The walk takes each entry's pNext before it compares (0x6FB5BF2D). The original inlines
//  GFXUTIL_GetOverlayTxtRecord here.
//  `retn 4`: nOverlayId on the stack. The second parameter, edx, is not read -- overwritten by the tables pointer before any use (`mov edx,
//  ds:sgptDataTables` 0x6FB5BEE1) -- and GFX_AddOverlay hands it the row's PreDraw byte.
void __fastcall GFX_RemoveOverlay(D2UnitStrc *pUnit, int32_t nUnused, D2_OverlayIds nOverlayId)
{
    D2ClientUnitGfxInfoStrc *pGfxInfo;
    D2ClientGfxEntryStrc *pEntry;
    D2ClientGfxEntryStrc *pNext;

    if (!pUnit)
        return;

    pGfxInfo = CUNIT_GfxGetUnitGfxInfo(pUnit);
    if (!pGfxInfo)
        return;

    if (!GFXUTIL_GetOverlayTxtRecord(nOverlayId))
    {
        FOG_DisplayAssert((uint32_t)"ptStats", (uint32_t)szGfxCpp, 3229);
        exit(-1);
    }

    for (pEntry = pGfxInfo->pOverlayList; pEntry; pEntry = pNext)
    {
        pNext = pEntry->pNext;
        if (pEntry->nOverlayId == (int32_t)nOverlayId || ((pEntry->nKind == OVERLAYTYPE_LOOPEX || pEntry->nKind == OVERLAYTYPE_AURAEX) && (pEntry->field_08 == (int32_t)nOverlayId || pEntry->nX == (int32_t)nOverlayId)))
        {
            GFX_RemoveGfxEntry(pUnit, pEntry);
            return;
        }
    }
}


//1.10f: D2Client.0x6FB5BF70
//  Every overlay of a unit removed: the list walked, each entry's pNext taken before GFX_RemoveGfxEntry frees it (0x6FB5BFA2). The gfx block is
//  asserted, Gfx.cpp:3267 (`push 0CC3h` 0x6FB5BF7C).
void __fastcall GFX_RemoveAllOverlays(D2UnitStrc *pUnit)
{
    D2ClientUnitGfxInfoStrc *pGfxInfo;
    D2ClientGfxEntryStrc *pEntry;
    D2ClientGfxEntryStrc *pNext;

    pGfxInfo = CUNIT_GfxGetUnitGfxInfo(pUnit);
    if (!pGfxInfo)
    {
        FOG_DisplayAssert((uint32_t)"ptGfxInfo", (uint32_t)szGfxCpp, 3267);
        exit(-1);
    }

    for (pEntry = pGfxInfo->pOverlayList; pEntry; pEntry = pNext)
    {
        pNext = pEntry->pNext;
        GFX_RemoveGfxEntry(pUnit, pEntry);
    }
}


//1.10f: D2Client.0x6FB5BFC0
//  The first palette shift stored: +34h of a unit's gfx block (`mov [ecx+34h], edx` 0x6FB5BFE2), the block asserted, `ptInfo` at Gfx.cpp:3288.
//  The four palette-shift accessors take the gfx block, not the unit (CUNIT_SetUnitPaletteShift1/2 and CUNIT_GetUnitPaletteShift1/2 hand them
//  the unit's), and assert it under the source's name, ptInfo.
void __fastcall GFX_SetPaletteShift1(D2ClientUnitGfxInfoStrc *pGfxData, int32_t nShift)
{
    if (!pGfxData)
    {
        FOG_DisplayAssert((uint32_t)"ptInfo", (uint32_t)szGfxCpp, 3288);
        exit(-1);
    }

    pGfxData->nPaletteShift1 = nShift;
}


//1.10f: D2Client.0x6FB5BFF0
//  The first palette shift read: +34h (`mov eax, [ecx+34h]` 0x6FB5C012), `ptInfo` asserted at Gfx.cpp:3301. Reached by the tail jump of
//  CUNIT_GetUnitPaletteShift1 (0x6FB28465).
int32_t __fastcall GFX_GetPaletteShift1(D2ClientUnitGfxInfoStrc *ptInfo)
{
    if (!ptInfo)
    {
        FOG_DisplayAssert((uint32_t)"ptInfo", (uint32_t)szGfxCpp, 3301);
        exit(-1);
    }

    return ptInfo->nPaletteShift1;
}


//1.10f: D2Client.0x6FB5C020
//  The second palette shift stored: +38h (`mov [ecx+38h], edx` 0x6FB5C042), `ptInfo` asserted at Gfx.cpp:3313.
void __fastcall GFX_SetPaletteShift2(D2ClientUnitGfxInfoStrc *pGfxData, int32_t nShift)
{
    if (!pGfxData)
    {
        FOG_DisplayAssert((uint32_t)"ptInfo", (uint32_t)szGfxCpp, 3313);
        exit(-1);
    }

    pGfxData->nPaletteShift2 = nShift;
}


//1.10f: D2Client.0x6FB5C050
//  The second palette shift read: +38h (`mov eax, [ecx+38h]` 0x6FB5C072), `ptInfo` asserted at Gfx.cpp:3326. Reached by the tail jump of
//  CUNIT_GetUnitPaletteShift2 (0x6FB284C5).
int32_t __fastcall GFX_GetPaletteShift2(D2ClientUnitGfxInfoStrc *ptInfo)
{
    if (!ptInfo)
    {
        FOG_DisplayAssert((uint32_t)"ptInfo", (uint32_t)szGfxCpp, 3326);
        exit(-1);
    }

    return ptInfo->nPaletteShift2;
}


//1.10f: D2Client.0x6FB5C080
//  A unit's current mode loaded: its type, class and mode converted (#11013, this file's line 3339, `push 0D0Bh` 0x6FB5C0B9; with no unit the
//  defaults 6 and -1), then by the converted type:
//
//      PLAYER   the mode's COF node got or built (GFX_GetUnitCofNode) and, when it is, the mode marked loaded (GFXUTIL_SetUnitModeLoaded)
//      MONSTER  the mode loaded if monstats2.txt has it (Gfx.cpp:1175); MONMODE_DEAD frees the unit's animation list and clears every
//               mode-loaded flag first
//      OBJECT   the mode loaded (GFX_LoadObjectModeGfx)
//      other    assert "0", line 3354 (`push 0D1Ah` 0x6FB5C0E3), and exit
//
//  GFX_RebuildUnitGfx has three arms on these types too, for a mode it is given, and answers a node after them; this one takes the unit's own
//  mode and answers nothing. A bare `retn` (0x6FB5C2D8): nothing on the stack. edx, nUnused, is not read, but every caller loads it first
//  (`xor edx, edx`, or `mov edx, 1` in STATES_ToggleStateWithSound, PLRSKILLS_EndStateAndSkill and UNITGFX_UnitInitGfx), so the source passed a
//  second argument the body ignores, and the parameter stays.
//  The PLAYER arm hands GFX_GetUnitCofNode the address of the slot the MONSTER arm stores the mode bit in (`lea edx, [esp+28h+nModeEnabled]`
//  0x6FB5C2B5), with nothing stored there on this path. GFX_GetUnitCofNode writes it only when it replaces a node and never reads it, so the C
//  hands it a local of its own, uninitialised -- defined, as nothing reads it. The MONSTER arm's store of the bit (0x6FB5C1EC) is read by
//  nothing either: the `jz` at 0x6FB5C1F0 tests the flags of the `and` at 0x6FB5C1E9.
//  The original inlines a monster's mode loaded (Gfx.cpp:1175), DATATBLS_GetMonStats2TxtRecord, GFX_FreeUnitAnimList and
//  GFX_SetAllUnitModesLoaded here.
void __fastcall GFX_RebuildUnitAnimForMode(D2UnitStrc *pUnit, int32_t nUnused)
{
    D2MonStats2Txt *pMonStats2Txt;
    void *pReplacedCofData;
    int32_t nType;
    int32_t nClass;
    int32_t nUnitMode;
    int32_t nLoadMode;
    int32_t nMonType;
    int32_t nMonClass;
    int32_t nMonMode;

    nType = pUnit ? (int32_t)pUnit->dwUnitType : (int32_t)UNIT_TYPES_COUNT;
    nClass = pUnit ? (int32_t)pUnit->dwClassId : -1;
    nUnitMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
    D2COMMON_11013_ConvertMode(pUnit, &nType, &nClass, &nUnitMode, szGfxCpp, 3339);

    switch (nType)
    {
    case UNIT_PLAYER:
        if (GFX_GetUnitCofNode(pUnit, &pReplacedCofData, nUnitMode))
            GFXUTIL_SetUnitModeLoaded(pUnit, nUnitMode, 1);
        return;

    case UNIT_MONSTER:
        nLoadMode = nUnitMode;
        nMonType = pUnit ? (int32_t)pUnit->dwUnitType : (int32_t)UNIT_TYPES_COUNT;
        nMonClass = pUnit ? (int32_t)pUnit->dwClassId : -1;
        nMonMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
        D2COMMON_11013_ConvertMode(pUnit, &nMonType, &nMonClass, &nMonMode, szGfxCpp, 1175);
        if (nLoadMode == MONMODE_SEQUENCE)
            nLoadMode = nMonMode;

        pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord(nMonClass);
        if (!pMonStats2Txt || !(gdwBitMasks[nLoadMode & 7] & pMonStats2Txt->nModeFlags[nLoadMode >> 3]))
            return;

        if (nLoadMode == MONMODE_DEAD)
        {
            GFX_FreeUnitAnimList(pUnit);
            GFX_SetAllUnitModesLoaded(pUnit, 0);
            GFX_LoadMonsterModeGfx(pUnit, MONMODE_DEAD);
            return;
        }

        if (nLoadMode == MONMODE_SEQUENCE)
            nLoadMode = nMonMode;

        GFX_LoadMonsterModeGfx(pUnit, nLoadMode);
        return;

    case UNIT_OBJECT:
        GFX_LoadObjectModeGfx(pUnit, nUnitMode);
        return;

    default:
        FOG_DisplayAssert((uint32_t)"0", (uint32_t)szGfxCpp, 3354);
        exit(-1);
    }
}


//1.10f: D2Client.0x6FB5C2E0
//  Every mode-loaded flag of a unit cleared, by its drawn type: unless nComponent is 16 or more -- `cmp edx, 10h` / `jge` (0x6FB5C2E3, 0x6FB5C2EA),
//  a signed compare, so the C casts the enum -- GFXUTIL_SetUnitModeLoaded(pUnit, nMode, 0) for each of a player's 20 modes, a monster's 16 or an
//  object's 8; any other type asserts "0", line 3378 (`push 0D32h` 0x6FB5C356), and exits. nComponent is read by that compare alone. The type is
//  Engine.h's drawn type: with no unit, 6, so a NULL unit reaches the assert. The counts are NUMBER_OF_PLRMODES, NUMBER_OF_MONMODES and
//  NUM_OBJECTMODES by value, bare as the bytes have them. The player loop compares unsigned (`cmp edi, 14h` / `jb` 0x6FB5C3B4, 0x6FB5C3B7), the
//  other two signed (`jl` 0x6FB5C39E, 0x6FB5C385). Called from ten ITEM_ functions of Item.cpp and PLAYER_AssignCorpse. void: eax is unread
//  after every call.
void __fastcall GFX_ClearUnitModesLoaded(D2UnitStrc *pUnit, D2Composits nComponent)
{
    uint32_t nPlayerMode;
    int32_t nMode;
    int32_t nType;
    int32_t nConvType;
    int32_t nConvClass;
    int32_t nConvMode;

    if ((int32_t)nComponent >= (int32_t)NUM_COMPONENTS)
        return;

    if (!pUnit)
    {
        nType = (int32_t)UNIT_TYPES_COUNT;
    }
    else if ((pUnit->dwFlagEx >> 3) & 1)
    {
        nConvType = (int32_t)pUnit->dwUnitType;
        nConvClass = (int32_t)pUnit->dwClassId;
        nConvMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
        D2COMMON_11013_ConvertMode(pUnit, &nConvType, &nConvClass, &nConvMode, szEngineH, 242);
        nType = nConvType;
    }
    else
    {
        nType = (int32_t)pUnit->dwUnitType;
    }

    switch (nType)
    {
    case UNIT_PLAYER:
        for (nPlayerMode = 0; nPlayerMode < 20; ++nPlayerMode)
            GFXUTIL_SetUnitModeLoaded(pUnit, (int32_t)nPlayerMode, 0);
        return;

    case UNIT_MONSTER:
        for (nMode = 0; nMode < 16; ++nMode)
            GFXUTIL_SetUnitModeLoaded(pUnit, nMode, 0);
        return;

    case UNIT_OBJECT:
        for (nMode = 0; nMode < 8; ++nMode)
            GFXUTIL_SetUnitModeLoaded(pUnit, nMode, 0);
        return;

    default:
        FOG_DisplayAssert((uint32_t)"0", (uint32_t)szGfxCpp, 3378);
        exit(-1);
    }
}


//1.10f: D2Client.0x6FB5C3C0
//  The offsets of a unit component's current cel: the unit's type, class and mode converted (#11013, line 3393, `push 0D41h` 0x6FB5C3ED), the
//  drawn inventory's weapon class code for that mode (D2Common's COMPOSIT_GetWeaponClassId), the animation node of the drawn mode and that code
//  found or, failing that, built (GFX_RebuildUnitGfx -- a NULL from it returns with nothing written); then the converted mode's COF looked up
//  (GFX_GetUnitCofData) -- none traces "Unable to get cof file for unit type:%d  class:%d  mode:%d in file:%s  line:%d" (line 3413) and
//  returns -- and the component's cel resolved into a cel context: a D2GfxDataStrc zeroed and filled by GFXUTIL_FillGfxData at the unit's frame
//  (the 8.8 counter shifted right 8, `sar edx, 8` 0x6FB5C56E) and its path's direction (a byte, `and eax, 0FFh` 0x6FB5C579), then D2CMP's
//  SpriteValidate. A context with no cel asserts `tCelContext.hCel`, line 3436 (`push 0D6Ch` 0x6FB5C5AA). The cel's X and Y offsets (D2CMP's
//  #10039 and #10040) are written through pnOffsetX and pnOffsetY; every other exit writes nothing. `retn 8`: the two out-parameters on the
//  stack.
//  pUnit is not tested: its path is read first (`mov eax, [esi+2Ch]` 0x6FB5C3CA) and kept for the direction at the end, so a NULL unit faults
//  there -- and the first two inlined copies drop the NULL tests their bodies make. The original inlines GFXUTIL_GetUnitDisplayInventory,
//  GFX_GetUnitAnimNode and GFX_GetUnitCofData here.
void __fastcall GFX_GetUnitCel(D2UnitStrc *pUnit, D2Composits nComponent, int32_t *pnOffsetX, int32_t *pnOffsetY)
{
    D2DynamicPathStrc *pDynamicPath;
    D2InventoryStrc *pInventory;
    D2GfxDataStrc tCelContext;
    int32_t nType;
    int32_t nClass;
    int32_t nMode;
    int32_t nWeaponClassCode;
    int32_t nWeaponClassId;
    int32_t nCelClass;
    int32_t nFrame;

    pDynamicPath = pUnit->pDynamicPath;
    nType = (int32_t)pUnit->dwUnitType;
    nClass = (int32_t)pUnit->dwClassId;
    nMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
    D2COMMON_11013_ConvertMode(pUnit, &nType, &nClass, &nMode, szGfxCpp, 3393);

    pInventory = GFXUTIL_GetUnitDisplayInventory(pUnit, NULL);
    nWeaponClassCode = D2COMMON_COMPOSIT_GetWeaponClassId(pUnit, pInventory, &nWeaponClassId, nMode, FALSE);

    if (!GFX_GetUnitAnimNode(pUnit, nWeaponClassCode, -1) && !GFX_RebuildUnitGfx(pUnit, nWeaponClassCode, -1))
        return;

    if (!GFX_GetUnitCofData(pUnit, nMode))
    {
        FOG_Trace("Unable to get cof file for unit type:%d  class:%d  mode:%d in file:%s  line:%d", nType, nClass, nMode, szGfxCpp, 3413);
        return;
    }

    nFrame = pUnit->nSeqCurrentFramePrecise >> 8;
    memset(&tCelContext, 0, sizeof(tCelContext));
    if (!GFXUTIL_FillGfxData(&tCelContext, pUnit, NULL, D2COMMON_PATH_GetDirection(pDynamicPath), nFrame, &nCelClass, nComponent, -1))
        return;

    if (!D2CMP_SpriteValidate(&tCelContext, FALSE, TRUE))
        return;

    if (!tCelContext.pCurrentCell)
    {
        FOG_DisplayAssert((uint32_t)"tCelContext.hCel", (uint32_t)szGfxCpp, 3436);
        exit(-1);
    }

    *pnOffsetX = D2CMP_CelGetOffsetX(tCelContext.pCurrentCell);
    *pnOffsetY = D2CMP_CelGetOffsetY(tCelContext.pCurrentCell);
}


//1.10f: D2Client.0x6FB5C600
//  Whether a point hits a unit's COF box: the drawn inventory's weapon class code (COMPOSIT_GetWeaponClassId with the mode -1 and TRUE), the
//  animation node of the drawn mode and that code found or, failing that, built (GFX_RebuildUnitGfx, its answer not read); then the unit's type,
//  class and mode converted (line 3458, `push 0D82h` 0x6FB5C711), the converted mode's COF looked up -- none answers FALSE -- its header taken
//  (GFXUTIL_ReturnArg) and asserted, `ptCofHeader` at line 3478 (`push 0D96h` 0x6FB5C768), and the point (nHitX, nHitY) tested against the
//  header's box placed at (nX, nY) and widened by 16 on every side: nXMin + nX - 16 <= nHitX < nXMax + nX + 16, and the same for Y. The four
//  compares are signed (`jl`, `jge`, `jl`, `jge`: 0x6FB5C793, 0x6FB5C79E, 0x6FB5C7B1, 0x6FB5C7BC). `retn 0Ch`: nY, nHitX and nHitY on the
//  stack. No cel is read: the box is the COF header's -- D2CofHeaderStrc's nXMin..nYMax at +8..+14h, beside nDirections at +2 -- one box
//  whatever the frame or direction. Both callers are in CUNIT_HitTestUnit; the PLAYER and MONSTER arms share the first, the monster's pushes
//  jumping into the player's (`jmp 0x6FB289CA` at 0x6FB289BE). The original inlines GFXUTIL_GetUnitDisplayInventory, GFX_GetUnitAnimNode and
//  GFX_GetUnitCofData here.
BOOL __fastcall GFX_HitTestUnitCof(D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t nHitX, int32_t nHitY)
{
    D2InventoryStrc *pInventory;
    D2CofHeaderStrc *pCofHeader;
    void *pCofData;
    int32_t nWeaponClassCode;
    int32_t nWeaponClassId;
    int32_t nType;
    int32_t nClass;
    int32_t nUnitMode;

    pInventory = GFXUTIL_GetUnitDisplayInventory(pUnit, NULL);
    nWeaponClassCode = D2COMMON_COMPOSIT_GetWeaponClassId(pUnit, pInventory, &nWeaponClassId, -1, TRUE);

    if (!GFX_GetUnitAnimNode(pUnit, nWeaponClassCode, -1))
        GFX_RebuildUnitGfx(pUnit, nWeaponClassCode, -1);

    nType = (int32_t)pUnit->dwUnitType;
    nClass = (int32_t)pUnit->dwClassId;
    nUnitMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
    D2COMMON_11013_ConvertMode(pUnit, &nType, &nClass, &nUnitMode, szGfxCpp, 3458);

    pCofData = GFX_GetUnitCofData(pUnit, nUnitMode);
    if (!pCofData)
        return FALSE;

    pCofHeader = (D2CofHeaderStrc *)GFXUTIL_ReturnArg(pCofData);
    if (!pCofHeader)
    {
        FOG_DisplayAssert((uint32_t)"ptCofHeader", (uint32_t)szGfxCpp, 3478);
        exit(-1);
    }

    return nHitX >= pCofHeader->nXMin + nX - 16 && nHitX < pCofHeader->nXMax + nX + 16 && nHitY >= pCofHeader->nYMin + nY - 16 && nHitY < pCofHeader->nYMax + nY + 16;
}


//1.10f: D2Client.0x6FB5C7D0
//  Whether a unit's COF box at (nX, nY) is on screen: the node found or built and the unit converted as GFX_HitTestUnitCof does (line 3508,
//  `push 0DB4h` 0x6FB5C8E1), the converted mode's COF and its header (`ptCofHeader` asserted, line 3527, `push 0DC7h` 0x6FB5C944); FALSE
//  without a COF. Then the box placed at (nX, nY): its left edge nX + nXMin -- moved left by half the box's height when bShadow is set,
//  `(nYMax - nYMin) / 2` divided signed and negated (`cdq` / `sub eax, edx` / `sar eax, 1` / `neg eax`, 0x6FB5C983..0x6FB5C988) -- must be less
//  than gnScreenWidth - 1, its right edge nX + nXMax (never moved) not below 0, its bottom nY + nYMax not below 0, and its top nY + nYMin less
//  than gnScreenHeight - 1 (`setl al` 0x6FB5C9D4). Every compare is signed: `jl` (0x6FB5C98F), the sign of the two sums (`jns` 0x6FB5C9A2,
//  0x6FB5C9B9), `setl`. `retn 0Ch`: nY, nUnused and bShadow on the stack. It fills no cel context; the box is the COF header's. bShadow is set
//  by GFX_DrawUnitCelShadow and GFX_DrawUnitShadow (`push 1` 0x6FB595E5, 0x6FB598B9); it moves the left edge by half the height, not the width.
//  nUnused is read nowhere; GFX_DrawUnitCels passes 1 (`push 1` 0x6FB5A94D), the other callers 0. The original inlines
//  GFXUTIL_GetUnitDisplayInventory, GFX_GetUnitAnimNode and GFX_GetUnitCofData here.
//  BUG: a candidate defect: a shadow's COF box is moved left by half its height on the X axis, and the right edge's test is not moved with the
//  left. NOT ESTABLISHED: it may be the shear D2Gfx gives a shadow (D2Gfx's #10075 TEXTURE_CelDrawShadow adds 2 to X and hands the draw to the
//  renderer's interface slot +90h, so the shear is the renderer's) or a slip.
BOOL __fastcall GFX_IsUnitCofOnScreen(D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t nUnused, int32_t bShadow)
{
    D2InventoryStrc *pInventory;
    D2CofHeaderStrc *pCofHeader;
    void *pCofData;
    int32_t nWeaponClassCode;
    int32_t nWeaponClassId;
    int32_t nType;
    int32_t nClass;
    int32_t nUnitMode;
    int32_t nLeft;

    pInventory = GFXUTIL_GetUnitDisplayInventory(pUnit, NULL);
    nWeaponClassCode = D2COMMON_COMPOSIT_GetWeaponClassId(pUnit, pInventory, &nWeaponClassId, -1, TRUE);

    if (!GFX_GetUnitAnimNode(pUnit, nWeaponClassCode, -1))
        GFX_RebuildUnitGfx(pUnit, nWeaponClassCode, -1);

    nType = (int32_t)pUnit->dwUnitType;
    nClass = (int32_t)pUnit->dwClassId;
    nUnitMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
    D2COMMON_11013_ConvertMode(pUnit, &nType, &nClass, &nUnitMode, szGfxCpp, 3508);

    pCofData = GFX_GetUnitCofData(pUnit, nUnitMode);
    if (!pCofData)
        return FALSE;

    pCofHeader = (D2CofHeaderStrc *)GFXUTIL_ReturnArg(pCofData);
    if (!pCofHeader)
    {
        FOG_DisplayAssert((uint32_t)"ptCofHeader", (uint32_t)szGfxCpp, 3527);
        exit(-1);
    }

    nLeft = nX + pCofHeader->nXMin;
    if (bShadow)
        nLeft += -((pCofHeader->nYMax - pCofHeader->nYMin) / 2);

    if (nLeft >= gnScreenWidth - 1)
        return FALSE;

    if (nX + pCofHeader->nXMax < 0)
        return FALSE;

    if (nY + pCofHeader->nYMax < 0)
        return FALSE;

    return nY + pCofHeader->nYMin < gnScreenHeight - 1;
}


//1.10f: D2Client.0x6FB5C9E0
//  An act's common COF file loaded into the other cache: the path "DATA\GLOBAL\<name>" built (wsprintfA, "%s\\%s") from gActCofDataFiles --
//  cmncof_a1.d2, a2, a3, a4 and, for act V, cmncof_a6.d2 -- and, unless the cache is loaded already, the file read whole (ARCHIVE_LoadFile,
//  line 269, `push 10Dh` 0x6FB5CA1D; none asserts "ptHash->lpbFileData", line 270) and its size added to gnCofMemoryUsed. The block is a run of
//  records, each a 24-byte cache node with its COF after it: the node's flags cleared, when set (`test` / `jz` 0x6FB5CA87); a COF of more than
//  64 directions asserting "swapCofHeader->bNumDirections <= 64", line 296 (`cmp byte ptr [ebp+1Ah], 40h` / `ja` 0x6FB5CA92, unsigned); pData
//  set to the COF; the node hashed by its name into the cache's buckets; and the next record nSize bytes past the COF, until the bytes left are
//  exactly 0 (`jnz` 0x6FB5CB07). Then the cache is marked loaded. The path is built before the test, so a loaded cache builds it for nothing.
//  A bare `retn`. nActIndex is a uint8_t: the body widens the byte itself (`and ecx, 0FFh` 0x6FB5C9E6) and both callers set cl alone
//  (GAME_PreloadActData's `mov cl, bl` 0x6FAA93C9, SCMD_HandleSrv7E's `mov cl, al` 0x6FAB4297, a tail jump). The original inlines GFX_HashCofName
//  here.
//  BUG: the walk stops only at exactly zero bytes left, so a block whose records did not add up to its size would be walked past its end.
//  LATENT: the stock files were not checked for it.
void __fastcall GFX_LoadActCofData(uint8_t nActIndex)
{
    D2ClientCofCacheStrc *pCache;
    D2ClientCofCacheNodeStrc *pNode;
    char *pRecord;
    uint32_t nBucket;
    int32_t nBytesLeft;
    char szFilename[260];

    //  BUG: nActIndex is not bounded (`and ecx, 0FFh` 0x6FB5C9E6, the load 0x6FB5C9EC). gActCofDataFiles is ours, so an act of 5 or more reads past
    //  our array where the original read on into its own .data. LATENT: stock levels.txt holds acts 0..4, and the stock server sends the act as the
    //  index of its own five acts; only another server could hand 5 or more.
    wsprintfA(szFilename, "%s\\%s", "DATA\\GLOBAL", gActCofDataFiles[nActIndex]);

    pCache = GFX_OtherCofCache();
    if (pCache->bLoaded)
        return;

    pCache->pBlock = ARCHIVE_LoadFile(D2WIN_GetArchive(), szFilename, (uint32_t *)&pCache->nBlockSize, szGfxCpp, 269);
    if (!pCache->pBlock)
    {
        FOG_DisplayAssert((uint32_t)"ptHash->lpbFileData", (uint32_t)szGfxCpp, 270);
        exit(-1);
    }

    pRecord = (char *)pCache->pBlock;
    nBytesLeft = pCache->nBlockSize;
    gnCofMemoryUsed += pCache->nBlockSize;
    while (nBytesLeft)
    {
        pNode = (D2ClientCofCacheNodeStrc *)pRecord;
        if (pNode->dwFlags)
            pNode->dwFlags = 0;

        if (((D2CofHeaderStrc *)&pNode->unk0x18)->nDirections > 64)
        {
            FOG_DisplayAssert((uint32_t)"swapCofHeader->bNumDirections <= 64", (uint32_t)szGfxCpp, 296);
            exit(-1);
        }

        pNode->pData = &pNode->unk0x18;
        nBucket = GFX_HashCofName(pNode->szName);
        pNode->pNext = pCache->pBuckets[nBucket];
        pCache->pBuckets[nBucket] = pNode;
        pRecord = (char *)pNode->pData + pNode->nSize;
        nBytesLeft += -(int32_t)offsetof(D2ClientCofCacheNodeStrc, unk0x18) - pNode->nSize;
    }

    pCache->bLoaded = 1;
}


//1.10f: D2Client.0x6FB5CB60
//  The other COF cache freed: its 512 buckets walked (`add ebp, 4` / `cmp ebp, 800h` / `jb` 0x6FB5CBC0..0x6FB5CBC9) and every node flagged 1 --
//  loaded alone by GFX_LoadCofFile; an act block's nodes are flag 0 since GFX_LoadActCofData -- freed with its COF (FOG_Free lines 3632 and 3633)
//  and taken off gnCofMemoryUsed with its 28 bytes; then the block's size taken off, the block freed (3669) and the cache's 0x80C bytes cleared
//  (`rep stosd` 203h dwords, 0x6FB5CBFA..0x6FB5CC06). The flag is tested as a byte (`test bl, al` 0x6FB5CB77, bl 1). void. Its one caller is
//  GAME_PreloadActData.
void __cdecl GFX_FreeCofCache(void)
{
    D2ClientCofCacheStrc *pCache;
    D2ClientCofCacheNodeStrc *pNode;
    D2ClientCofCacheNodeStrc *pNext;
    uint32_t nBucket;

    pCache = GFX_OtherCofCache();
    for (nBucket = 0; nBucket < 512; ++nBucket)
    {
        for (pNode = pCache->pBuckets[nBucket]; pNode; pNode = pNext)
        {
            pNext = pNode->pNext;
            if (pNode->dwFlags & 1)
            {
                gnCofMemoryUsed += -(int32_t)sizeof(D2ClientCofCacheNodeStrc) - pNode->nSize;
                FOG_Free((uint32_t)pNode->pData, (uint32_t)szGfxCpp, 3632, 0);
                FOG_Free((uint32_t)pNode, (uint32_t)szGfxCpp, 3633, 0);
            }
        }
    }

    gnCofMemoryUsed -= pCache->nBlockSize;
    if (pCache->pBlock)
        FOG_Free((uint32_t)pCache->pBlock, (uint32_t)szGfxCpp, 3669, 0);

    memset(pCache, 0, sizeof(D2ClientCofCacheStrc));
}


//1.10f: D2Client.0x6FB5CC10
//  A monstats2.txt row by its index: NULL below 0 (`test ecx, ecx` / `jl` 0x6FB5CC12) or at the record count (`jge` 0x6FB5CC21), else the row
//  (stride 134h). The index is a monstats.txt row's wMonStatsEx, not a monster id. A bare `retn`.
D2MonStats2Txt *__fastcall GFX_GetMonStats2Record(int32_t nMonStats2Id)
{
    if (nMonStats2Id < 0 || nMonStats2Id >= sgptDataTables->nMonStats2TxtRecordCount)
        return NULL;

    return &sgptDataTables->pMonStats2Txt[nMonStats2Id];
}


//1.10f: D2Client.0x6FB5CC40
//  A monster's COF files loaded, and its sprites requested: its monstats.txt and monstats2.txt rows (nothing without both), then for each monster
//  mode of gPreloadAnimModes the class has (monstats2.txt's nModeFlags bit, the byte tested against gdwBitMasks' low byte, `test [edi+edx*4],
//  cl` 0x6FB5CCE9): the COF's path built (GFX_BuildMonsterCofName, which also answers the weapon class code), its title lower-cased and looked up
//  in the other cache's bucket (GFX_HashCofName, the eight-byte compare) or loaded into it (GFX_LoadCofFile) -- a COF that cannot be loaded
//  halts, "Could not add cof file: %s", line 3767, FOG_DisplayHalt and exit; then its COF asserted (`lpbCofData`, 3772), its header
//  (`ptCofHeader`, 3774) and its layer order for direction 0, frame 0 (`lpbCompByte`, 3779), and each layer's component (`eComponent <
//  NUM_COMPONENTS`, 3786, a signed `jge` 0x6FB5CE26) resolved into a cel context by GFXUTIL_FillCelContextTokens -- once per COF direction of
//  the token record, or once for direction 0 when its count is 0 -- and validated by D2CMP's SpriteValidate: waited for and loaded (1, 1) when
//  bValidateSprites, and on a failure the context's cel file cleared and handed to D2CMP's #10056; else requested (0, 0). A bare `retn`.
//  The token record is a D2ClientMonCofTokensStrc: 52 bytes zeroed (`rep stosd` 0Dh dwords, 0x6FB5CD04) and given the monster id as a word
//  (0x6FB5CD2A), and nothing else -- so its count of COF directions at +3 is 0 and the per-direction arm never runs in this build, and the
//  variants GFXUTIL_FillCelContextTokens reads at +4 are 0.
//  nMonsterId is an int: the bounds test reads the whole of esi (`test esi, esi` / `cmp esi, [ecx+0A80h]`, 0x6FB5CC4B, 0x6FB5CC5F), and the
//  callers set the whole of ecx (`mov ecx, 0D3h` 0x6FB0693D; SCMD_HandleSrvA4's word zero-extended, `and ecx, 0FFFFh` 0x6FAB2EE9). The record and
//  GFX_BuildMonsterCofName take the id as a word (`mov word ptr [...], si`, `movsx edx, si`). The frame holds the token record at +38h, two cel
//  contexts at +6Ch and +0B4h, the title and the path at +0FCh and +200h. The original inlines DATATBLS_GetMonStatsTxtRecord and
//  GFX_GetMonStats2Record here.
void __fastcall GFX_AddMonsterCofFiles(int32_t nMonsterId, BOOL bValidateSprites)
{
    D2ClientCofCacheStrc *pCache;
    D2ClientCofCacheNodeStrc *pCofFile;
    D2MonStatsTxt *pMonStatsTxt;
    D2MonStats2Txt *pMonStats2Txt;
    D2CofHeaderStrc *pCofHeader;
    uint8_t *pCofData;
    uint8_t *pLayerOrder;
    D2ClientMonCofTokensStrc tMonTokens;
    D2GfxDataStrc tDirCelContext;
    D2GfxDataStrc tCelContext;
    int32_t nWeaponClassCode;
    int32_t iMode;
    int32_t nMode;
    int32_t iLayer;
    int32_t nComponent;
    uint32_t nCofDirection;
    char szTitle[260];
    char szCofPath[260];

    pMonStatsTxt = DATATBLS_GetMonStatsTxtRecord(nMonsterId);
    if (!pMonStatsTxt)
        return;

    pMonStats2Txt = GFX_GetMonStats2Record((int16_t)pMonStatsTxt->wMonStatsEx);
    if (!pMonStats2Txt)
        return;

    pCache = GFX_OtherCofCache();
    for (iMode = 0; iMode < gnPreloadAnimModeCount; ++iMode)
    {
        nMode = gPreloadAnimModes[iMode];
        if (!(pMonStats2Txt->nModeFlags[nMode >> 3] & (uint8_t)gdwBitMasks[nMode & 7]))
            continue;

        memset(szCofPath, 0, sizeof(szCofPath));
        memset(&tMonTokens, 0, sizeof(tMonTokens));
        tMonTokens.nMonsterId = (int16_t)nMonsterId;
        GFX_BuildMonsterCofName(pMonStatsTxt, tMonTokens.nMonsterId, nMode, &nWeaponClassCode, szCofPath);
        memset(szTitle, 0, sizeof(szTitle));
        GFX_ExtractFileTitle(szCofPath, szTitle);
        _strlwr(szTitle);
        for (pCofFile = pCache->pBuckets[GFX_HashCofName(szTitle)]; pCofFile; pCofFile = pCofFile->pNext)
        {
            if (GFX_CofNameEquals(szTitle, pCofFile))
                break;
        }

        if (!pCofFile)
        {
            pCofFile = GFX_LoadCofFile(pCache, szCofPath);
            if (!pCofFile)
            {
                FOG_DisplayHalt((const char *)FOG_csprintf((uint32_t)szTitle, "Could not add cof file: %s", szCofPath), szGfxCpp, 3767);
                exit(-1);
            }
        }

        pCofData = (uint8_t *)pCofFile->pData;
        if (!pCofData)
        {
            FOG_DisplayAssert((uint32_t)"lpbCofData", (uint32_t)szGfxCpp, 3772);
            exit(-1);
        }

        pCofHeader = (D2CofHeaderStrc *)GFXUTIL_ReturnArg(pCofData);
        if (!pCofHeader)
        {
            FOG_DisplayAssert((uint32_t)"ptCofHeader", (uint32_t)szGfxCpp, 3774);
            exit(-1);
        }

        pLayerOrder = GFXUTIL_GetCofLayerOrder(pCofData, 0, 0);
        if (!pLayerOrder)
        {
            FOG_DisplayAssert((uint32_t)"lpbCompByte", (uint32_t)szGfxCpp, 3779);
            exit(-1);
        }

        //  The layer count is the header's first byte.
        for (iLayer = 0; iLayer < pCofHeader->unk0x00; ++iLayer)
        {
            nComponent = pLayerOrder[iLayer];
            if (nComponent >= (int32_t)NUM_COMPONENTS)
            {
                FOG_DisplayAssert((uint32_t)"eComponent < NUM_COMPONENTS", (uint32_t)szGfxCpp, 3786);
                exit(-1);
            }

            if (tMonTokens.nCofDirections)
            {
                for (nCofDirection = 0; nCofDirection < tMonTokens.nCofDirections; ++nCofDirection)
                {
                    memset(&tDirCelContext, 0, sizeof(tDirCelContext));
                    if (!GFXUTIL_FillCelContextTokens(&tDirCelContext, 0, 0, nComponent, nMode, nWeaponClassCode, pCofFile, &tMonTokens, pMonStatsTxt, (int32_t)nCofDirection))
                        continue;

                    if (bValidateSprites)
                    {
                        if (!D2CMP_SpriteValidate(&tDirCelContext, TRUE, TRUE))
                        {
                            tDirCelContext.pCellFile = NULL;
                            D2CMP_10056((uint32_t)&tDirCelContext);
                        }
                    }
                    else
                    {
                        D2CMP_SpriteValidate(&tDirCelContext, FALSE, FALSE);
                    }
                }
            }
            else
            {
                memset(&tCelContext, 0, sizeof(tCelContext));
                if (!GFXUTIL_FillCelContextTokens(&tCelContext, 0, 0, nComponent, nMode, nWeaponClassCode, pCofFile, &tMonTokens, pMonStatsTxt, 0))
                    continue;

                if (bValidateSprites)
                {
                    if (!D2CMP_SpriteValidate(&tCelContext, TRUE, TRUE))
                    {
                        tCelContext.pCellFile = NULL;
                        D2CMP_10056((uint32_t)&tCelContext);
                    }
                }
                else
                {
                    D2CMP_SpriteValidate(&tCelContext, FALSE, FALSE);
                }
            }
        }
    }
}


//1.10f: D2Client.0x6FB5D040
//  The COF cache's hash: a name of 8 characters or more asserts `strlen(pszFileName) < MAX_COFDATA_FILENAME`, line 121 (`push 79h` 0x6FB5D055;
//  `cmp ecx, 8` / `jb`, unsigned); then Storm's SStrHash(name, 1, 0) folded to nine bits -- three `shr 8` over the masks 1FF0000h, 1FF00h and
//  1FFh (0x6FB5D07A..0x6FB5D09E) -- the bucket of 512. A bare `retn`.
uint32_t __fastcall GFX_HashCofName(const char *pszFileName)
{
    uint32_t nHash;

    if (strlen(pszFileName) >= 8)
    {
        FOG_DisplayAssert((uint32_t)"strlen(pszFileName) < MAX_COFDATA_FILENAME", (uint32_t)szGfxCpp, 121);
        exit(-1);
    }

    nHash = STORM_SStrHash(pszFileName, 1, 0);
    return (nHash & 0x1FF) ^ (((nHash & 0x1FF00) ^ (((nHash >> 8) ^ (nHash & 0x1FF0000)) >> 8)) >> 8);
}


//1.10f: D2Client.0x6FB5D0B0
//  A monster mode's COF path, and its weapon class: nothing unless the monster's monstats.txt and monstats2.txt rows exist (looked up again from
//  nMonsterId; pMonStatsTxt, the caller's row in ecx, is not read); then the mode's token from D2Common's #10647 (DATATBLS_GetMonModeTxtRecord
//  with 1), asserted `ptToken`, line 3706 (`push 0E7Ah` 0x6FB5D135); the weapon class written through pnWeaponClassCode -- for MONMODE_DEATH and
//  MONMODE_DEAD, monstats2.txt's BaseW when the row has MONSTATS2FLAGINDEX_COMPOSITEDEATH (`mov dl, [esi+6]` / `test byte ptr [eax], dl`,
//  gdwBitMasks[0]'s byte, 0x6FB5D173..0x6FB5D176), else "hth " (`mov dword ptr [esi], 20687468h` 0x6FB5D189); for every other mode, BaseW; then
//  the mode token replaced for a mode the override pairs name (gnMonModeOverrideCount of them, the last match winning, `jbe` / `jb` unsigned,
//  0x6FB5D19A, 0x6FB5D1AF) -- one pair in this build: MONMODE_KNOCKBACK (13) drawn with "gh  ", the get-hit COF; and the path written by
//  wsprintfA: "DATA\GLOBAL\MONSTERS\<code>\COF\<code><mode><weapon>.COF", each token its first three characters with a space made a NUL (`cmp al,
//  20h` / `sete dl` / `dec dl` / `and edx, eax`). The monster's code is built in nMonModeIndex's argument slot (`mov byte ptr [esp+2Ch], al`
//  0x6FB5D208), read once into edi at 0x6FB5D11E; a local here. `retn 0Ch`: nMonModeIndex, pnWeaponClassCode and pszOut.
//  pnWeaponClassCode receives the weapon class's four characters (0x6FB5D16A, 0x6FB5D181, 0x6FB5D189), three of which are read back;
//  GFX_AddMonsterCofFiles passes its slot as the weapon class to GFXUTIL_FillCelContextTokens. The monster's code is monstats.txt's (`mov ebx,
//  [ecx+10h]` 0x6FB5D122). The override pairs are gMonModeTokenOverrides, a D2ClientMonModeTokenOverrideStrc table indexed as the bytes do
//  (`[eax*8+6FB96E84h]`, `[eax*8+6FB96E80h]`). The original inlines DATATBLS_GetMonStatsTxtRecord and GFX_GetMonStats2Record here.
void __fastcall GFX_BuildMonsterCofName(D2MonStatsTxt *pMonStatsTxt, int32_t nMonsterId, int32_t nMonModeIndex, int32_t *pnWeaponClassCode, LPSTR pszOut)
{
    D2MonStatsTxt *pMonStats;
    D2MonStats2Txt *pMonStats2Txt;
    D2MonModeTxt *pMonModeTxt;
    const D2ClientMonModeTokenOverrideStrc *pOverrides;
    const char *pszWeapon;
    uint32_t dwMonCode;
    uint32_t dwModeToken;
    uint32_t nOverride;
    char szMonCode[4];
    char szModeToken[4];
    char szWeaponToken[4];

    pMonStats = DATATBLS_GetMonStatsTxtRecord(nMonsterId);
    if (!pMonStats)
        return;

    pMonStats2Txt = GFX_GetMonStats2Record((int16_t)pMonStats->wMonStatsEx);
    if (!pMonStats2Txt)
        return;

    dwMonCode = pMonStats->dwCode;
    pMonModeTxt = D2COMMON_DATATBLS_GetMonModeTxtRecord(nMonModeIndex, 1);
    if (!pMonModeTxt)
    {
        FOG_DisplayAssert((uint32_t)"ptToken", (uint32_t)szGfxCpp, 3706);
        exit(-1);
    }

    dwModeToken = pMonModeTxt->dwToken;
    if (nMonModeIndex == MONMODE_DEATH || nMonModeIndex == MONMODE_DEAD)
    {
        if (pMonStats2Txt->nFlags[MONSTATS2FLAGINDEX_COMPOSITEDEATH >> 3] & (uint8_t)gdwBitMasks[MONSTATS2FLAGINDEX_COMPOSITEDEATH & 7])
            *pnWeaponClassCode = pMonStats2Txt->nBaseW;
        else
            *pnWeaponClassCode = 0x20687468;    //  "hth "
    }
    else
    {
        *pnWeaponClassCode = pMonStats2Txt->nBaseW;
    }

    pOverrides = gMonModeTokenOverrides;
    for (nOverride = 0; nOverride < gnMonModeOverrideCount; ++nOverride)
    {
        if (nMonModeIndex == pOverrides[nOverride].nMonMode)
            dwModeToken = pOverrides[nOverride].dwToken;
    }

    pszWeapon = (const char *)pnWeaponClassCode;
    szWeaponToken[0] = pszWeapon[0] == ' ' ? 0 : pszWeapon[0];
    szWeaponToken[1] = pszWeapon[1] == ' ' ? 0 : pszWeapon[1];
    szWeaponToken[2] = pszWeapon[2] == ' ' ? 0 : pszWeapon[2];
    szWeaponToken[3] = 0;
    szMonCode[0] = (char)dwMonCode == ' ' ? 0 : (char)dwMonCode;
    szMonCode[1] = (char)(dwMonCode >> 8) == ' ' ? 0 : (char)(dwMonCode >> 8);
    szMonCode[2] = (char)(dwMonCode >> 16) == ' ' ? 0 : (char)(dwMonCode >> 16);
    szMonCode[3] = 0;
    szModeToken[0] = (char)dwModeToken == ' ' ? 0 : (char)dwModeToken;
    szModeToken[1] = (char)(dwModeToken >> 8) == ' ' ? 0 : (char)(dwModeToken >> 8);
    szModeToken[2] = (char)(dwModeToken >> 16) == ' ' ? 0 : (char)(dwModeToken >> 16);
    szModeToken[3] = 0;
    wsprintfA(pszOut, "%s\\%s\\COF\\%s%s%s.COF", "DATA\\GLOBAL\\MONSTERS", szMonCode, szMonCode, szModeToken, szWeaponToken);
}


//  The player preload table's extent: a D2ClientModePreloadStrc per player class -- the 118h dwords GFX_InitCelCache clears from its head.
static_assert(NUMBER_OF_PLAYERCLASSES * sizeof(D2ClientModePreloadStrc) == 0x118 * 4,
              "gaPlayerPreloadTable: GFX_InitCelCache `mov ecx, 118h` 0x6FB590D2, `mov edi, offset gaPlayerPreloadTable` 0x6FB590D7");


//1.10f: D2Client.0x6FB5D290
//  The preload when loading completes: a pass marked active (gbPreloadPassActive = 1, 0x6FB5D293), the preload sweep run twice -- synchronously,
//  then asynchronously -- and the pass marked over (= 0, 0x6FB5D601; on the second sweep's throttle exit, 0x6FB5D613). A bare `retn`. Its one
//  caller is SCMD_HandleSrv04_LoadComplete (packet 0x04).
//  nAct is never read: the body touches no ecx before writing it. The caller computes it for this call alone -- the player's room's act, `and eax,
//  0FFh` / `mov cl, al` (0x6FAB21B8, 0x6FAB21BD) -- so the source passed it, and the parameter stays, as GFX_RebuildUnitAnimForMode's nUnused does.
//  Both sweeps are GFX_PreloadVisibleUnits, inlined -- each pushes that function's own line, Gfx.cpp:1954 (`push 7A2h` 0x6FB5D348, 0x6FB5D4EF).
//  The first has bAsync 0 folded in: no throttle test (the flag was stored above it, with no call between), the loading screen's `!bAsync ||
//  gbPreloadRequired` true, nothing for a player's gbPreloadRequired to clear; its monster arm calls GFX_PreloadAllAnimModes with edx 0 (`xor
//  edx, edx` 0x6FB5D395) and its player arm GFX_PreloadPlayerMode in a loop with `push 0` (0x6FB5D3B6) -- GFX_PreloadAllPlayerModes inlined.
//  The second has bAsync 1 (`mov ebp, 1` 0x6FB5D425) and the throttle test, whose way out (`jb` 0x6FB5D41F) is this function's own
//  `gbPreloadPassActive = 0`; its monster arm inlines GFX_PreloadAllAnimModes, calling GFXUTIL_GetEffectiveClassId and
//  DATATBLS_GetMonStats2TxtRecord (0x6FB5D547, 0x6FB5D54E), and its player arm calls GFX_PreloadAllPlayerModes (0x6FB5D5BB). The two share one
//  frame, some slots for another value in each: pType holds the first sweep's type and the second's bAsync (0x6FB5D324, 0x6FB5D42A).
//  Nothing else writes gbPreloadPassActive: its three writes are this function's (0x6FB5D293, 0x6FB5D601, 0x6FB5D613). So inside the pass the
//  flag is 1 at every test -- neither sweep stops on D2CMP's pending requests, and the second's throttle never takes its way out: 0x6FB5D613 is
//  dead. The pass's loading screen is drawn by the first sweep and, in the second, only while gbPreloadRequired is set.
void __fastcall GFX_PreloadOnLoadComplete(uint8_t nAct)
{
    gbPreloadPassActive = 1;

    GFX_PreloadVisibleUnits(0);

    GFX_PreloadVisibleUnits(1);

    gbPreloadPassActive = 0;
}


//1.10f: D2Client.0x6FB5D630
//  A monster's modes preloaded. Its drawn class (GFXUTIL_GetEffectiveClassId, Engine.h:278) picks its monstats2.txt row
//  (DATATBLS_GetMonStats2TxtRecord); with none, nothing. Each mode of gPreloadAnimModes -- 15 -- whose bit the row's nModeFlags has (its byte
//  against gdwBitMasks' low byte, `mov al, [edx+ebp+0F0h]` / `test [edx+ecx*4], al` 0x6FB5D716, 0x6FB5D723) is preloaded once
//  (GFX_PreloadAnimMode, Gfx.cpp:1794). A bare `retn`. Its one caller is the first sweep in GFX_PreloadOnLoadComplete (0x6FB5D399). The original
//  inlines
//  GFXUTIL_GetEffectiveClassId, DATATBLS_GetMonStats2TxtRecord and GFX_PreloadAnimMode itself.
void __fastcall GFX_PreloadAllAnimModes(D2UnitStrc *pUnit, int32_t bAsync)
{
    D2MonStats2Txt *pMonStats2Txt;
    uint32_t iMode;
    uint32_t nAnimMode;

    pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord(GFXUTIL_GetEffectiveClassId(pUnit));
    if (!pMonStats2Txt)
        return;

    for (iMode = 0; iMode < (uint32_t)gnPreloadAnimModeCount; ++iMode)
    {
        nAnimMode = gPreloadAnimModes[iMode];
        if (!(pMonStats2Txt->nModeFlags[nAnimMode >> 3] & (uint8_t)gdwBitMasks[nAnimMode & 7]))
            continue;

        GFX_PreloadAnimMode(pUnit, (int32_t)nAnimMode, bAsync);
    }
}


//1.10f: D2Client.0x6FB5D830
//  One mode of a player preloaded, once -- GFX_PreloadAnimMode's twin over the player table. The unit's drawn type and class (#11013,
//  Gfx.cpp:1865, `push 749h` 0x6FB5D86A) pick its record of gaPlayerPreloadTable -- seven D2ClientModePreloadStrc, one per player class. A mode
//  loaded does nothing; a mode queued is asked of D2CMP's #10058 with the type, the mode and the class, and marked loaded once nothing is
//  pending; a mode neither is loaded through GFX_AsyncLoadBlock -- an answer of 1 marks it queued, anything else clears both flags. `retn 4`:
//  bAsync is the stack argument. nAnimMode is an int, the type of GFX_PreloadAnimMode's: it is handed gPreloadComponents' values, player modes.
//  The class has no bound: GFX_PreloadAnimMode tests monstats.txt's count, this function nothing, and a class of 7 or more would index past the
//  table. Every caller reaches it for a unit the same conversion has just called a player (the sweeps' `nType == UNIT_PLAYER`, line 1954);
//  whether a player's class can reach 7 was not looked for.
void __fastcall GFX_PreloadPlayerMode(D2UnitStrc *pUnit, int32_t nAnimMode, int32_t bAsync)
{
    int32_t nType;
    int32_t nClass;
    int32_t nMode;

    nType = pUnit ? (int32_t)pUnit->dwUnitType : (int32_t)UNIT_TYPES_COUNT;
    nClass = pUnit ? (int32_t)pUnit->dwClassId : -1;
    nMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
    D2COMMON_11013_ConvertMode(pUnit, &nType, &nClass, &nMode, szGfxCpp, 1865);

    if (gaPlayerPreloadTable[nClass].bLoaded[nAnimMode])
        return;

    if (gaPlayerPreloadTable[nClass].bQueued[nAnimMode])
    {
        if (!D2CMP_10058(nType, nAnimMode, nClass))
            gaPlayerPreloadTable[nClass].bLoaded[nAnimMode] = 1;
        return;
    }

    if (GFX_AsyncLoadBlock(pUnit, nAnimMode, bAsync) == 1)
    {
        gaPlayerPreloadTable[nClass].bQueued[nAnimMode] = 1;
    }
    else
    {
        gaPlayerPreloadTable[nClass].bQueued[nAnimMode] = 0;
        gaPlayerPreloadTable[nClass].bLoaded[nAnimMode] = 0;
    }
}


//1.10f: D2Client.0x6FB5D930
//  Every player mode of gPreloadComponents preloaded: its eight dwords are player modes -- 5, 6, 1, 2, 3, 7, 10, 4 -- each handed to
//  GFX_PreloadPlayerMode with this function's bAsync; the walk begins at zero (`xor ebp, ebp` 0x6FB5D93C). A bare `retn`. Its one caller is the
//  second sweep in GFX_PreloadOnLoadComplete (0x6FB5D5BB). The original inlines GFX_PreloadPlayerMode here.
void __fastcall GFX_PreloadAllPlayerModes(D2UnitStrc *pUnit, int32_t bAsync)
{
    uint32_t iMode;

    for (iMode = 0; iMode < (uint32_t)gnPreloadComponentCount; ++iMode)
    {
        GFX_PreloadPlayerMode(pUnit, gPreloadComponents[iMode], bAsync);
    }
}


//1.10f: D2Client.0x6FB5DA40
//  A living player stopped: a bare `retn`, pUnit in ecx. Unless CUNIT_IsUnitDead says the unit is dead or dying, a PLAYER is given
//  CLTACTION_NEUTRAL through PLAYER_DoClientAction, with no parameters and bInterruptKnockback TRUE; anything else, nothing. It sits between two
//  of this file's asserts. Its callers are all in Item.cpp, one in each of 13 ITEM_ functions. void: the body leaves CUNIT_IsUnitDead's answer or
//  PLAYER_DoClientAction's in eax, and no caller reads it.
//  BUG: the NULL test comes after CUNIT_IsUnitDead, which reads pUnit->dwFlags first, so a NULL unit faults there and the test cannot be taken.
//  Reproduced. UNREACHABLE: the test cannot be taken.
void __fastcall GFX_StopIfAlive(D2UnitStrc *pUnit)
{
    if (CUNIT_IsUnitDead(pUnit))
        return;

    if (pUnit && pUnit->dwUnitType == UNIT_PLAYER)
        PLAYER_DoClientAction(CLTACTION_NEUTRAL, pUnit, NULL, TRUE);
}


//1.10f: D2Client.0x6FB5DA70
//  The "GFX Memory" figure: gnGfxMemoryUsed, six bytes (`mov eax`, `retn`). Its one caller is DRAW_GameDrawDebugOverlay (0x6FAA2A91), which
//  prints it as "GFX Memory->%i". The global has one reference, this read, and lies past .data's raw size: the loader zero-fills it, so the
//  figure is always 0.
int32_t __cdecl GFX_GetGfxMemoryUsed(void)
{
    return gnGfxMemoryUsed;
}


//1.10f: D2Client.0x6FB5DA80
//  The "COF Memory" figure: gnCofMemoryUsed, the bytes the two COF caches hold -- added by GFX_LoadCofFile and GFX_LoadActCofData, taken off by
//  GFX_FreeCofCache and GFX_FreeCelCache. Six bytes. Its one caller is DRAW_GameDrawDebugOverlay (0x6FAA2AAE), the first figure of "COF
//  Memory->%i of %i".
int32_t __cdecl GFX_GetCofMemoryUsed(void)
{
    return gnCofMemoryUsed;
}


//1.10f: D2Client.0x6FB5DA90
//  The second figure of that line: gnCofMemoryBudget, six bytes. Written once, by GFX_InitCelCache from its argument (0x6FB590B2) -- 2 MB from
//  MEMPART_InitMemoryPartitions' tail jump -- and read only here. Its one caller is DRAW_GameDrawDebugOverlay (0x6FAA2AA8).
int32_t __cdecl GFX_GetCofMemoryBudget(void)
{
    return gnCofMemoryBudget;
}


//1.10f: D2Client.0x6FB5DAA0
//  A monster class's record of the monster preload table: gpMonsterPreloadTable plus 160 times the class (`lea eax, [ecx+ecx*4]` / `shl eax, 5` /
//  `add eax, ecx`), unbounded. Its one caller is GFXUTIL_ResolveAnimDirection (0x6FB5F178), which reads the record's bLoaded at the mode. The
//  preload functions index the table themselves (GFX_PreloadAnimMode).
D2ClientModePreloadStrc *__fastcall GFX_GetMonsterPreloadEntry(int32_t nMonsterId)
{
    return &gpMonsterPreloadTable[nMonsterId];
}


//1.10f: D2Client.0x6FB5DAB0
//  One unit preloaded when the server introduces it. Its drawn type, class and mode (#11013, Gfx.cpp:4197, `push 1065h` 0x6FB5DAEB); anything but a
//  monster within monstats.txt returns (`dec eax` / `jnz` 0x6FB5DB06, `jge` 0x6FB5DB1E, signed). A monster's current mode is marked neither
//  loaded nor queued (0x6FB5DB3D, 0x6FB5DB53), so that it is asked of the loader again, and its modes are preloaded (GFX_PreloadAllAnimModes,
//  inlined whole) -- asynchronously only when it stands in a room, no window is up (D2Gfx's #10026, WINDOW_GetState) and the loading screen is not
//  active: bAsync starts 1 (0x6FB5DB2C) and any of the three clears it (0x6FB5DB72). The current mode indexes the record's twenty flags
//  unchecked; whether #11013 can answer 20 or more was not looked for. A bare `retn`. Its callers are the packet handlers
//  SCMD_HandleSrv09_SpawnUnit (a tail jump, 0x6FAB23BE) and SCMD_HandleSrvAC (0x6FAB4E85).
void __fastcall GFX_PreloadUnit(D2UnitStrc *pUnit)
{
    int32_t nType;
    int32_t nClass;
    int32_t nMode;
    int32_t bAsync;

    nType = pUnit ? (int32_t)pUnit->dwUnitType : (int32_t)UNIT_TYPES_COUNT;
    nClass = pUnit ? (int32_t)pUnit->dwClassId : -1;
    nMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
    D2COMMON_11013_ConvertMode(pUnit, &nType, &nClass, &nMode, szGfxCpp, 4197);
    if (nType != UNIT_MONSTER || nClass >= sgptDataTables->nMonStatsTxtRecordCount)
        return;

    bAsync = 1;
    gpMonsterPreloadTable[nClass].bLoaded[nMode] = 0;
    gpMonsterPreloadTable[nClass].bQueued[nMode] = 0;
    if (!D2COMMON_UNITS_GetRoom(pUnit) || D2GFX_WINDOW_GetState() || UI_IsLoadingScreenActive())
        bAsync = 0;

    GFX_PreloadAllAnimModes(pUnit, bAsync);
}

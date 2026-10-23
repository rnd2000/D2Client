#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Game.h"
#include "Object.h"

//  Object.cpp -- the client's object code of D2Client.dll 1.10: construction and freeing, day and night modes, idle overlays, animation, the shrine
//  chart's client effects, the client process functions (objects.txt ClientFn), and a portal's state and owner name. 41 OBJECT_ functions, address
//  range 0x6FB464A0..0x6FB4856F. The first three handle NPC speech: the greeting sound of an NPC, the sound of a spoken line and the speech
//  option's audio flag. The original source file is UNIT\Object.cpp (the asserts in this unit name it).

static const char szObjectCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\UNIT\\Object.cpp";

static_assert(sizeof(D2ObjectDataStrc) == 0x38
                  && offsetof(D2ObjectDataStrc, pObjectTxt) == 0
                  && offsetof(D2ObjectDataStrc, bPermanent) == 0x10
                  && offsetof(D2ObjectDataStrc, unk0x014) == 0x14,
              "OBJECT_InitClientUnit: FOG_Alloc(38h) and `mov [edx], edi` "
              "(0x6FB469EB, 0x6FB46A50); the zeros at [ecx+10h] and [edx+14h] "
              "(0x6FB46A11, 0x6FB46A17)");
static_assert(offsetof(D2ObjectsTxt, nSelectable) == 0xC4
                  && offsetof(D2ObjectsTxt, nLit) == 0x110
                  && offsetof(D2ObjectsTxt, nBlocksLight) == 0x118
                  && offsetof(D2ObjectsTxt, nHasCollision) == 0x120
                  && offsetof(D2ObjectsTxt, nIsAttackable0) == 0x128
                  && offsetof(D2ObjectsTxt, nOrderFlag) == 0x131
                  && offsetof(D2ObjectsTxt, nEnvEffect) == 0x139,
              "Object.cpp: objects.txt reads at 0x6FB4683D, 0x6FB46871, "
              "0x6FB46AB3, 0x6FB46A5A, 0x6FB46A72, 0x6FB46AD1 and 0x6FB467EC");
static_assert(offsetof(D2ObjectsTxt, nRed) == 0x164
                  && offsetof(D2ObjectsTxt, nGreen) == 0x165
                  && offsetof(D2ObjectsTxt, nBlue) == 0x166
                  && offsetof(D2ObjectsTxt, nOverlay) == 0x1B5,
              "Object.cpp: RGB at 0x6FB46869/62/5C; overlay gate at 0x6FB46BA5");
static_assert(OBJECT_DUMMY_BRAZIER == 29 && OBJECT_DUMMY_TORCH1_TIKI == 37,
              "OBJECT_SetLight: `cmp eax, 1Dh` (0x6FB46967) and "
              "`cmp eax, 25h` (0x6FB46949)");
static_assert(OBJECT_TAINTEDSUNALTAR == 149 && OBJECT_ORIFICE == 152
                  && OBJECT_HORADRIC_CUBE_CHEST == 354
                  && OBJECT_HORADRIC_SCROLL_CHEST == 355
                  && OBJECT_STAFF_OF_KINGS_CHEST == 356
                  && OBJECT_SPARKLY_CHEST == 397
                  && OBJECT_KHALIM_CHEST1 == 405 && OBJECT_KHALIM_CHEST2 == 406
                  && OBJECT_KHALIM_CHEST3 == 407
                  && OBJECT_CHEST_SPECIALCHEST == 455,
              "OBJECT_AddIdleOverlay: switch and compares at 0x6FB46BB2..0x6FB46C06");
static_assert(OVERLAY_MULTIGLEAM == 71,
              "OBJECT_AddIdleOverlay: `mov edx, 47h` (0x6FB46C19)");
static_assert(OBJMODE_OPERATING == 1 && UNITFLAG_PREDRAW == 0x100000,
              "OBJECT_ApplyDayNightMode: `mov dword ptr [esi+10h], 1` "
              "(0x6FB46822); OBJECT_InitClientUnit: `or dword ptr "
              "[esi+0C4h], 100000h` (0x6FB46ADC)");
static_assert(OBJECT_PERMANENT_TOWN_PORTAL == 60 && LEVEL_TRISTRAM == 38
                  && LEVEL_STONYFIELD == 4,
              "OBJECT_InitClientUnit: `sub eax, 3Bh` / `dec eax` (0x6FB46AE9, "
              "0x6FB46AEE); `cmp eax, 26h` (0x6FB46AF7), `cmp eax, ebx` with "
              "ebx 4 (0x6FB46B04)");

//  The numbers the three NPC-speech functions read: the six greeting class ids no other file pins, the two day cycles the greeting compares, the
//  records' and the sound table's fields at the offsets the bytes read, and the tbl index's width, the word the scan compares.
static_assert(MONSTER_WARRIV2 == 175 && MONSTER_ATMA == 176 && MONSTER_GEGLASH == 200
                  && MONSTER_MESHIF1 == 210 && MONSTER_TYRAEL1 == 251 && MONSTER_NATALYA == 297
                  && ENVCYCLE_MORNING == 1 && ENVCYCLE_AFTERNOON == 3
                  && offsetof(D2ClientNpcSpeechStrc, nAltSoundId) == 0x04
                  && offsetof(D2ClientNpcSpeechStrc, nDayCycleSoundBase) == 0x08
                  && offsetof(D2ClientNpcSpeechStrc, nSoundIdKind2) == 0x0C
                  && offsetof(D2ClientNpcSpeechStrc, nLastPicked) == 0x10
                  && offsetof(D2ClientNpcSpeechStrc, nLastFrame) == 0x14
                  && offsetof(D2ClientSpeechSoundStrc, nTblStringId) == 0x04 && sizeof(D2C_StringIndices) == 2,
              "OBJECT_PickNpcSpeechSound: `cmp eax, 0AFh` (0x6FB464AF), `cmp eax, 129h` (0x6FB46596) and the index "
              "table's 176, 200, 210, 251 (b4_switch.py); `cmp eax, 1`, `cmp eax, 3` (0x6FB46687, 0x6FB4668E); `mov "
              "eax, [esi+0Ch]` (0x6FB464DB), `[esi+4]` (0x6FB4664A), `[esi+8]` (0x6FB46662), `[esi+10h]` (0x6FB466AD, "
              "0x6FB466BA), `[esi+14h]` (0x6FB466C2); OBJECT_FindSpeechSoundByTblIndex `cmp [eax+4], cx` (0x6FB467B3)");

//1.10f: D2Client.0x6FB464A0
//  An NPC's greeting: the sound id to play when an NPC is spoken to, from the record gaNpcGreetingSounds holds for its class. nSpeechKind is 0, 1,
//  or 2 when the menu entry's +12h flag was set. Callers: NPCMENU_OpenMainMenu, NPCMENU_OpenInterruptMenu and UNITSND_PlayUnitSoundEvent.
//  Returns a sounds.txt row (D2C_Sounds): SOUNDHDR_PickRandomInGroup's own, a record's field, or 0; every caller hands it to SOUNDINST_PlaySound as
//  it is. No read of a field is signed; the +1 and +2 are rows, and the C casts their sums back.
//  The records are 29 of six dwords (D2ClientNpcSpeechStrc[29], 0x6FB91C54..0x6FB91F0B): four sounds.txt rows (akara's record holds
//  akara_greeting_1, _greeting_inactive_1, _greeting_time_1 and _greeting_return), and this function's state, the last pick and
//  GAME_GetFrameCount() at it. The records are written.
//  The switch maps 35 class ids onto 28 records, all compares unsigned; the one record left, 15 (0x6FB91DBC, jamella_young_greeting_1), nothing
//  selects: Jamella (405) takes jamella_old's record. The six Cains share one record, the two Tyraels and the two Warrivs one each.
//  The pick. nSpeechKind 2 returns the record's fourth sound, with no roll and nothing stored: _greeting_return in 23 of the 29 records, its
//  _greeting_1 again in Cain's, Elzix's, Halbu's, Tyrael's and both Jamellas' (Halbu's and the Jamellas' records have no time line, 0).
//  Otherwise, up to twenty times (unsigned, 0x6FB466B4..0x6FB466B8):
//    * the greeting; with nSpeechKind 0 and an inactive line, a one-in-two roll for it instead (SOUNDSONG_RollRandom(2) answering 0);
//    * with a time line, when there is no greeting or a one-in-three roll answers 0: the line for the act's day cycle, ENVCYCLE_MORNING the base,
//      NOON or AFTERNOON the base + 1, SUNRISE, SUNSET, NIGHT or any other value the base + 2, which are _greeting_time_1, _2 and _3 (signed
//      compares, 0x6FB46687..0x6FB46691);
//    * SOUNDHDR_PickRandomInGroup on that group, kept when it differs from the last pick.
//  Then the pick and the frame are stored and the pick returned, also after twenty picks equal to the last: a group that can give only one sound
//  rolls twenty times on every greeting after its first (the image's last pick is 0) and gives that sound.
D2C_Sounds __fastcall OBJECT_PickNpcSpeechSound(D2UnitStrc *pUnit, int32_t nSpeechKind)
{
    D2ClientNpcSpeechStrc *pSpeechRec;
    D2C_Sounds nPickedSound;
    uint32_t i;
    D2C_Sounds nSoundId;
    int32_t nCycleIndex;

    if (!pUnit)
        return SOUND_NONE;

    switch (pUnit->dwClassId)
    {
    case MONSTER_AKARA:                                         //  148
        pSpeechRec = &gaNpcGreetingSounds[0];          //  akara_greeting_1
        break;
    case MONSTER_ALKOR:                                         //  254
        pSpeechRec = &gaNpcGreetingSounds[1];          //  alkor_greeting_1
        break;
    case MONSTER_ASHEARA:                                       //  252
        pSpeechRec = &gaNpcGreetingSounds[2];          //  asheara_greeting_1
        break;
    case MONSTER_ATMA:                                          //  176
        pSpeechRec = &gaNpcGreetingSounds[3];          //  atma_greeting_1
        break;
    case MONSTER_CAIN1:                                         //  146
    case MONSTER_CAIN2:                                         //  244
    case MONSTER_CAIN3:                                         //  245
    case MONSTER_CAIN4:                                         //  246
    case MONSTER_CAIN5:                                         //  265
    case MONSTER_CAIN6:                                         //  520
        pSpeechRec = &gaNpcGreetingSounds[4];          //  cain_greeting_1
        break;
    case MONSTER_CHARSI:                                        //  154
        pSpeechRec = &gaNpcGreetingSounds[5];          //  charsi_greeting_1
        break;
    case MONSTER_DROGNAN:                                       //  177
        pSpeechRec = &gaNpcGreetingSounds[6];          //  drognan_greeting_1
        break;
    case MONSTER_ELZIX:                                         //  199
        pSpeechRec = &gaNpcGreetingSounds[7];          //  elzix_greeting_1
        break;
    case MONSTER_FARA:                                          //  178
        pSpeechRec = &gaNpcGreetingSounds[8];          //  fara_greeting_1
        break;
    case MONSTER_GEGLASH:                                       //  200
        pSpeechRec = &gaNpcGreetingSounds[9];          //  geglash_greeting_1
        break;
    case MONSTER_GHEED:                                         //  147
        pSpeechRec = &gaNpcGreetingSounds[10];         //  gheed_greeting_1
        break;
    case MONSTER_GREIZ:                                         //  198
        pSpeechRec = &gaNpcGreetingSounds[11];         //  greiz_greeting_1
        break;
    case MONSTER_HALBU:                                         //  257
        pSpeechRec = &gaNpcGreetingSounds[12];         //  halbu_greeting_1
        break;
    case MONSTER_HRATLI:                                        //  253
        pSpeechRec = &gaNpcGreetingSounds[13];         //  hratli_greeting_1
        break;
    case MONSTER_JAMELLA:                                       //  405
        pSpeechRec = &gaNpcGreetingSounds[14];         //  jamella_old_greeting_1
        break;
        //  [15] jamella_young_greeting_1: no case
    case MONSTER_JERHYN:                                        //  201
        pSpeechRec = &gaNpcGreetingSounds[16];         //  jerhyn_greeting_1
        break;
    case MONSTER_KASHYA:                                        //  150
        pSpeechRec = &gaNpcGreetingSounds[17];         //  kashya_greeting_1
        break;
    case MONSTER_LYSANDER:                                      //  202
        pSpeechRec = &gaNpcGreetingSounds[18];         //  lysander_greeting_1
        break;
    case MONSTER_MESHIF1:                                       //  210
        pSpeechRec = &gaNpcGreetingSounds[19];         //  meshif_greeting_1
        break;
    case MONSTER_NATALYA:                                       //  297
        pSpeechRec = &gaNpcGreetingSounds[20];         //  natalya_greeting_1
        break;
    case MONSTER_ORMUS:                                         //  255
        pSpeechRec = &gaNpcGreetingSounds[21];         //  ormus_greeting_1
        break;
    case MONSTER_TYRAEL1:                                       //  251
    case MONSTER_TYRAEL3:                                       //  521
        pSpeechRec = &gaNpcGreetingSounds[22];         //  tyrael_greeting_1
        break;
    case MONSTER_WARRIV1:                                       //  155
    case MONSTER_WARRIV2:                                       //  175
        pSpeechRec = &gaNpcGreetingSounds[23];         //  warriv_greeting_inactive_1, no greeting
        break;
    case MONSTER_DREHYA:                                        //  512
        pSpeechRec = &gaNpcGreetingSounds[24];         //  anya_greeting_inactive_1, no greeting
        break;
    case MONSTER_QUAL_KEHK:                                     //  515
        pSpeechRec = &gaNpcGreetingSounds[25];         //  qualkehk_greeting_inactive_1, no greeting
        break;
    case MONSTER_MALAH:                                         //  513
        pSpeechRec = &gaNpcGreetingSounds[26];         //  malah_greeting_inactive_1, no greeting
        break;
    case MONSTER_LARZUK:                                        //  511
        pSpeechRec = &gaNpcGreetingSounds[27];         //  larzuk_greeting_inactive_1, no greeting
        break;
    case MONSTER_NIHLATHAK:                                     //  514
        pSpeechRec = &gaNpcGreetingSounds[28];         //  nihlathak_greeting_inactive_1, no greeting
        break;
    default:
        return SOUND_NONE;
    }

    if (nSpeechKind == 2)
        return pSpeechRec->nSoundIdKind2;

    i = 0;
    do
    {
        nSoundId = pSpeechRec->nSoundId;
        if (!nSpeechKind && pSpeechRec->nAltSoundId && !SOUNDSONG_RollRandom(2))
            nSoundId = pSpeechRec->nAltSoundId;

        if (pSpeechRec->nDayCycleSoundBase && (!nSoundId || !SOUNDSONG_RollRandom(3)))
        {
            //  The compares below are signed in the bytes.
            nCycleIndex = D2COMMON_ENVIRONMENT_GetCycleIndexFromAct(gpAct);
            if (nCycleIndex == (int32_t)ENVCYCLE_MORNING)
                nSoundId = pSpeechRec->nDayCycleSoundBase;
            else if (nCycleIndex <= (int32_t)ENVCYCLE_MORNING || nCycleIndex > (int32_t)ENVCYCLE_AFTERNOON)
                nSoundId = (D2C_Sounds)(pSpeechRec->nDayCycleSoundBase + 2);   //  _greeting_time_3's row
            else
                nSoundId = (D2C_Sounds)(pSpeechRec->nDayCycleSoundBase + 1);   //  _greeting_time_2's row
        }

        nPickedSound = SOUNDHDR_PickRandomInGroup(nSoundId);
        if (nPickedSound != pSpeechRec->nLastPicked)
            break;
        ++i;
    }
    while (i < 20);

    pSpeechRec->nLastPicked = nPickedSound;
    pSpeechRec->nLastFrame = GAME_GetFrameCount();
    return nPickedSound;
}

//1.10f: D2Client.0x6FB467A0
//  The sound of a spoken line: the sound id gSpeechSoundTable gives a tbl string, or 0 -- and 0 at once while the NPC speech option leaves the audio
//  off (gbNpcSpeechAudio, see OBJECT_SetNpcSpeechAudio). Callers: TEXT_ShowSpeechPopup and TEXT_ShowQuestScrollingText.
//  It takes a D2C_StringIndices and answers a D2C_Sounds. The tbl index is a word compared for equality (cmp [eax+4], cx, 0x6FB467B3), the enum's
//  own width, so its sign is read nowhere; ours compares two D2C_StringIndices. The sound is tested for 0 and returned.
//  The table, 0x6FB91F10, is 864 pairs of a sounds.txt row and a tbl index and a pair of zeros at 0x6FB93A10 that ends the scan:
//  D2ClientSpeechSoundStrc[865], 8 bytes each (int, D2C_StringIndices, a pad word that is 0 in every pair).
//  The step add eax, 8 / jnz (0x6FB467B9) tests the pointer for 0, which a table at a fixed address cannot reach; the C keeps the test.
D2C_Sounds __fastcall OBJECT_FindSpeechSoundByTblIndex(D2C_StringIndices nTblStringId)
{
    const D2ClientSpeechSoundStrc *pEntry;

    if (gbNpcSpeechAudio)
    {
        pEntry = gSpeechSoundTable;
        while (pEntry->nSoundId)
        {
            if (pEntry->nTblStringId == nTblStringId)
                return pEntry->nSoundId;
            ++pEntry;
            if (!pEntry)
                break;
        }
    }
    return SOUND_NONE;
}

//1.10f: D2Client.0x6FB467D0
//  The NPC speech option's audio half: gbNpcSpeechAudio = bAudio. Its only caller is the ESC menu's NPCSpeech row, whose three choices (the images
//  AudioOnly, TextOnly and AudioText, in that order) pass 1, 0 and 1. Its only reader is OBJECT_FindSpeechSoundByTblIndex, which answers 0 while
//  it is 0. The image holds 1, the audio on.
void __fastcall OBJECT_SetNpcSpeechAudio(int32_t bAudio)
{
    gbNpcSpeechAudio = bAudio;
}

//1.10f: D2Client.0x6FB467E0
//  Applies an object's day or night mode. The record's nEnvEffect byte gates the whole function. A nonzero period from D2Common #10933 must be 1..3;
//  the other nonzero values warn at Object.cpp:98 and return 0. During the nonzero-period arm, mode zero becomes one and the selectable bit is
//  updated before the light. During the zero-period arm, modes one and two become zero, the light is set first, and the selectable bit follows.
//  Both arms skip rebuilding the animation when bSkipAnimRebuild is true.
//  OBJECT_SetLight takes three colour arguments on the stack (pushes at 0x6FB46868..70 and 0x6FB468E3..E8). The original's partial-register colour
//  writes may carry high bits, but DLIGHTMAP_AddLight only stores each channel into a byte of D2ClientLightStrc (+0x25..+0x27), so they are not
//  observable; likewise movzx dx fills only the radius's low word (0x6FB46871, 0x6FB468EB), and OBJECT_SetLight reads di alone.
int32_t __fastcall OBJECT_ApplyDayNightMode(D2UnitStrc *pObject, int32_t bSkipAnimRebuild)
{
    D2ObjectsTxt *pObjectTxt = pObject->pObjectData->pObjectTxt;
    if (!pObjectTxt->nEnvEffect)
        return 0;

    int32_t nPeriodOfDay = D2COMMON_ENVIRONMENT_GetPeriodOfDayFromAct(gpAct, NULL);
    if (nPeriodOfDay)
    {
        if (nPeriodOfDay <= 0 || nPeriodOfDay > 3)
        {
            FOG_DisplayWarning("0", szObjectCpp, 98);
            return 0;
        }

        if (pObject->dwAnimMode == OBJMODE_NEUTRAL)
        {
            pObject->dwAnimMode = OBJMODE_OPERATING;
            if (!bSkipAnimRebuild)
                GFX_RebuildUnitAnimForMode(pObject, 0);
            D2COMMON_UNITS_SetAnimStartFrame(pObject);

            if (pObjectTxt->nSelectable[pObject->dwAnimMode])
                pObject->dwFlags |= UNITFLAG_TARGETABLE;
            else
                pObject->dwFlags &= ~UNITFLAG_TARGETABLE;
        }

        OBJECT_SetLight(pObject, pObjectTxt->nLit[pObject->dwAnimMode], pObjectTxt->nRed, pObjectTxt->nGreen, pObjectTxt->nBlue);
        return 1;
    }

    if (pObject->dwAnimMode != OBJMODE_OPERATING && pObject->dwAnimMode != OBJMODE_OPENED)
        return 0;

    pObject->dwAnimMode = OBJMODE_NEUTRAL;
    if (!bSkipAnimRebuild)
        GFX_RebuildUnitAnimForMode(pObject, 0);
    D2COMMON_UNITS_SetAnimStartFrame(pObject);
    OBJECT_SetLight(pObject, pObjectTxt->nLit[pObject->dwAnimMode], pObjectTxt->nRed, pObjectTxt->nGreen, pObjectTxt->nBlue);

    if (pObjectTxt->nSelectable[pObject->dwAnimMode])
        pObject->dwFlags |= UNITFLAG_TARGETABLE;
    else
        pObject->dwFlags &= ~UNITFLAG_TARGETABLE;
    return 1;
}

//1.10f: D2Client.0x6FB46930
//  Gives an object its static light or frees it. The radius test is 16-bit (test di, di, 0x6FB46936); both the creation and target-radius calls
//  take its unsigned low word shifted logically by one. The colour arguments arrive on the stack (retn 0Ch) and the constant intensity is 255.
//  Ids 37 and 29 are DUMMY_TORCH1_TIKI and DUMMY_BRAZIER (the trace strings call them Torch1 Tiki and Brazier). +0x64 of the client unit is its
//  pLight union arm.
void __fastcall OBJECT_SetLight(D2UnitStrc *pObject, int32_t nRadius, int32_t nRed, int32_t nGreen, int32_t nBlue)
{
    D2ClientLightStrc *pLight = pObject->pLight;
    uint32_t nRadiusWord = (uint16_t)nRadius;

    if (!nRadiusWord)
    {
        if (!pLight)
            return;
        D2Objects nObjectId = (D2Objects)pObject->dwClassId;
        if (nObjectId == OBJECT_DUMMY_TORCH1_TIKI)
            FOG_Trace("removed light source for tiki torch\n");
        else if (nObjectId == OBJECT_DUMMY_BRAZIER)
            FOG_Trace("removed light source for brazier\n");
        CUNIT_GfxFreeUnitLightMap(pObject);
        return;
    }

    if (!pLight)
    {
        pLight = DLIGHTMAP_AddLight(pObject, 2, nRadiusWord >> 1, 255, nRed, nGreen, nBlue);
        pObject->pLight = pLight;
        if (!pLight)
            return;
    }
    DLIGHTMAP_SetTargetRadius(pLight, nRadiusWord >> 1);
}

//1.10f: D2Client.0x6FB469D0
//  Constructs a client object. Four stack arguments beyond the two fastcall registers (retn 10h): the pushes at 0x6FB46A4C..0x6FB46A4F hand D2Common
//  #10350 the unit, the room (ebp, from edx), X and Y (ebx). The 56-byte allocation is D2ObjectDataStrc.
void __fastcall OBJECT_InitClientUnit(D2UnitStrc *pObject, D2ActiveRoomStrc *pRoom, int32_t nX, int32_t nY, D2Objects nObjectId, uint8_t nMode)
{
    pObject->pLight = NULL;
    pObject->dwAnimMode = (D2C_ObjModes)nMode;

    D2ObjectDataStrc *pObjectData = (D2ObjectDataStrc *)FOG_Alloc(sizeof(D2ObjectDataStrc), szObjectCpp, 114, 0);
    pObject->pObjectData = pObjectData;
    memset(pObjectData, 0, sizeof(D2ObjectDataStrc));
    pObjectData->bPermanent = 0;
    pObjectData->unk0x014 = 0;

    D2ObjectsTxt *pObjectTxt = D2COMMON_DATATBLS_GetObjectsTxtRecord(nObjectId);
    if (pObjectTxt->nSelectable[pObject->dwAnimMode])
        pObject->dwFlags |= UNITFLAG_TARGETABLE;
    else
        pObject->dwFlags &= ~UNITFLAG_TARGETABLE;
    pObjectData->pObjectTxt = pObjectTxt;

    D2COMMON_UNITS_InitializeStaticPath(pObject, pRoom, nX, nY);
    if (pObjectTxt->nHasCollision[pObject->dwAnimMode])
        D2COMMON_UNITS_BlockCollisionPath(pObject, pRoom, nX, nY);

    if (pObjectTxt->nIsAttackable0)
        pObject->dwFlags |= UNITFLAG_CANBEATTACKED;
    else
        pObject->dwFlags &= ~UNITFLAG_CANBEATTACKED;
    D2COMMON_UNITROOM_AddUnitToRoom(pObject, pRoom);

    pObject->dwFlags &= ~UNITFLAG_ISVALIDTARGET;
    D2COMMON_UNITS_SetAnimStartFrame(pObject);
    if (pObjectTxt->nBlocksLight[pObject->dwAnimMode])
        pObject->dwFlags &= ~UNITFLAG_DRAWSHADOW;
    else
        pObject->dwFlags |= UNITFLAG_DRAWSHADOW;
    if (pObjectTxt->nOrderFlag[pObject->dwAnimMode] == 1)
        pObject->dwFlags |= UNITFLAG_PREDRAW;

    //  For a portal the InteractType byte holds its destination level id. The act travels as a byte written over nX's argument slot (0x6FB46B1D)
    //  and the whole dword is pushed (0x6FB46B25); D2Common #10085 uses its low byte only, so the slot's upper bytes are inert.
    D2Objects nClassId = (D2Objects)pObject->dwClassId;
    if (nClassId == OBJECT_TOWN_PORTAL)
    {
        int32_t nLevelId = D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom);
        uint8_t nAct = D2COMMON_DRLG_GetActNoFromLevelId((D2C_Levels)nLevelId);
        int32_t nTownLevelId = D2COMMON_DUNGEON_GetTownLevelIdFromActNo(nAct);
        if (nTownLevelId > 255)
        {
            FOG_DisplayAssert("eLevelId <= 255", szObjectCpp, 145);
            exit(-1);
        }
        pObjectData->InteractType = (uint8_t)nTownLevelId;
    }
    else if (nClassId == OBJECT_PERMANENT_TOWN_PORTAL)
    {
        //  Tristram's permanent portal leads to the Stony Field and back. The 4 stored is ebx, loaded as the UNITFLAG_CANBEATTACKED mask at
        //  0x6FB46A78 and reused (0x6FB46AFF).
        int32_t nLevelId = D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom);
        if (nLevelId == LEVEL_TRISTRAM)
            pObjectData->InteractType = LEVEL_STONYFIELD;
        else if (nLevelId == LEVEL_STONYFIELD)
            pObjectData->InteractType = LEVEL_TRISTRAM;
    }

    if (!OBJECT_ApplyDayNightMode(pObject, 1))
        OBJECT_SetLight(pObject, pObjectTxt->nLit[pObject->dwAnimMode], pObjectTxt->nRed, pObjectTxt->nGreen, pObjectTxt->nBlue);
}

//1.10f: D2Client.0x6FB46BA0
//  Applies the object's idle overlay, if its objects.txt nOverlay byte is set. The id ranges are tested unsigned (ja, jb, jbe). Objects.txt ids
//  354..356, 397, 405..407 and 455 are chest rows in the stock table; the two overlay rows are MULTIGLEAM and HORADRIC_LIGHT.
void __fastcall OBJECT_AddIdleOverlay(D2UnitStrc *pObject)
{
    if (!pObject->pObjectData->pObjectTxt->nOverlay)
        return;

    switch ((D2Objects)pObject->dwClassId)
    {
    case OBJECT_TAINTEDSUNALTAR:
        if (pObject->dwAnimMode == OBJMODE_OPERATING)
            GFX_AddOverlay(pObject, OVERLAY_HORADRIC_LIGHT, OVERLAYTYPE_LOOP, 0, 0, 0, 0, 0);
        return;
    case OBJECT_ORIFICE:
        GFX_AddOverlay(pObject, OVERLAY_HORADRIC_LIGHT, OVERLAYTYPE_LOOP, 0, 0, 0, 0, 0);
        return;
    case OBJECT_HORADRIC_CUBE_CHEST:
    case OBJECT_HORADRIC_SCROLL_CHEST:
    case OBJECT_STAFF_OF_KINGS_CHEST:
    case OBJECT_SPARKLY_CHEST:
    case OBJECT_KHALIM_CHEST1:
    case OBJECT_KHALIM_CHEST2:
    case OBJECT_KHALIM_CHEST3:
    case OBJECT_CHEST_SPECIALCHEST:
        if (pObject->dwAnimMode == OBJMODE_NEUTRAL)
            GFX_AddOverlay(pObject, OVERLAY_MULTIGLEAM, OVERLAYTYPE_LOOP, 0, 0, 0, 0, 0);
        return;
    default:
        return;
    }
}

static_assert(offsetof(D2ObjectsTxt, dwFrameCnt) == 0xD8
                  && offsetof(D2ObjectsTxt, nCycleAnim) == 0x108
                  && offsetof(D2ObjectsTxt, nStart) == 0x129
                  && offsetof(D2ObjectsTxt, nIsDoor) == 0x13A
                  && offsetof(D2ObjectsTxt, dwParm) == 0x178
                  && offsetof(D2ObjectsTxt, nClientFn) == 0x1B4,
              "OBJECT_AdvanceAnim: [edi+eax*4+0D8h], [eax+edi+108h], "
              "[eax+edi+129h], [edi+13Ah], dwParm[7] at [edi+194h], "
              "[ecx+1B4h] (0x6FB46D5E, 0x6FB46D73, 0x6FB46DC4, 0x6FB46D82, "
              "0x6FB46E04, 0x6FB46F87)");
static_assert(OBJECT_ROGUEFOUNTAIN == 12 && OBJECT_CAINPORTAL == 189
                  && OBJMODE_SPECIAL1 == 3 && MONSTER_MINISPIDER == 339
                  && MONSTER_RAT == 151,
              "OBJECT_AdvanceAnim: `cmp edx, 0Ch` (0x6FB46EF6), `cmp ..., 0BDh` "
              "(0x6FB46DA9, 0x6FB46F12), `cmp eax, 3` (0x6FB46DB7), "
              "`mov edx, 153h` / `mov edx, 97h` (0x6FB46E17, 0x6FB46E1E)");

//1.10f: D2Client.0x6FB46D10
//  Frees a client object. The gfx info goes first and always; the collision path, the room membership and the light map only while *gpAct is set
//  (0x6FB46D18), and the object data last (Object.cpp:250). Why those three need the act is not established.
//  The answer is FOG_Free's own (its body ends mov eax, 1); the one caller, CUNIT_DestroyUnit, discards it, so the int the bytes return is kept
//  and read by nobody.
int32_t __fastcall OBJECT_FreeClientUnit(D2UnitStrc *pObject)
{
    GFX_FreeUnitGfxInfo(pObject);
    if (gpAct)
    {
        D2COMMON_UNITS_FreeCollisionPath(pObject);
        D2COMMON_UNITROOM_RemoveUnitFromRoom(pObject);
        CUNIT_GfxFreeUnitLightMap(pObject);
    }
    return FOG_Free(pObject->pObjectData, szObjectCpp, 250, 0);
}

//1.10f: D2Client.0x6FB46D50
//  Advances an object's animation one frame, from UNITMODE_UpdateUnit. The counter nSeqCurrentFramePrecise is 8.8 fixed point, stepped by the
//  unit's signed wAnimSpeed (movsx), against the mode's dwFrameCnt, also 8.8; a mode of one frame (0x100, 0x6FB46D65) is not stepped at all.
//  A looping mode (nCycleAnim) steps and wraps to its nStart frame; a non-looping one steps until its last frame. A door goes to
//  OBJECT_AdvanceDoorAnim instead. Two objects play backwards: the rogue fountain (12) always, wrapping by the frame count, and Cain's portal
//  (189) in mode 3, stepping to mode 4 at its start frame when the count runs out.
//  At the last frame of a non-looping mode: Cain's portal in mode 2 or 3 steps to the next mode; mode 1, OPERATING, becomes OPENED --
//  UNITFLAG_PREDRAW when nOrderFlag[2] is 1; when dwParm[7] is set, a call to UNITS_FindFreeSpotForMonster with the mini spider for 255, the rat
//  for any other value, and 4; the start frame; the chests' gleam removed; the selectable bit; the static light; the collision path freed when
//  OPENED has none and OPERATING had one.
//  Last, every object with nClientFn above 3 must have OBJECT_ClientProcess answer nonzero before its mode sound is updated; the rest always are.
void __fastcall OBJECT_AdvanceAnim(D2UnitStrc *pUnit)
{
    D2ObjectsTxt *pObjectTxt = pUnit->pObjectData->pObjectTxt;
    uint32_t nMode = pUnit->dwAnimMode;
    int32_t nFrameCount = (int32_t)pObjectTxt->dwFrameCnt[nMode];

    if (nFrameCount != 0x100)
    {
        int32_t bCycleAnim = pObjectTxt->nCycleAnim[nMode];
        if (!bCycleAnim && pObjectTxt->nIsDoor)
        {
            OBJECT_AdvanceDoorAnim(pUnit);
        }
        else if (!bCycleAnim && pUnit->nSeqCurrentFramePrecise >= nFrameCount - 0x100)
        {
            if (pUnit->dwClassId == OBJECT_CAINPORTAL && (nMode == OBJMODE_OPENED || nMode == OBJMODE_SPECIAL1))
            {
                ++nMode;
                pUnit->dwAnimMode = nMode;
                pUnit->nSeqCurrentFramePrecise = pObjectTxt->nStart[nMode] << 8;
                GFX_RebuildUnitAnimForMode(pUnit, 0);
                D2COMMON_UNITS_SetAnimStartFrame(pUnit);
            }
            else if (nMode == OBJMODE_OPERATING)
            {
                pUnit->dwAnimMode = OBJMODE_OPENED;
                if (pObjectTxt->nOrderFlag[OBJMODE_OPENED] == 1)
                    pUnit->dwFlags |= UNITFLAG_PREDRAW;

                uint32_t nParm7 = pObjectTxt->dwParm[7];
                if (nParm7)
                    UNITS_FindFreeSpotForMonster(pUnit, nParm7 == 255 ? MONSTER_MINISPIDER : MONSTER_RAT, 4);

                pUnit->nSeqCurrentFramePrecise = pObjectTxt->nStart[OBJMODE_OPENED] << 8;
                GFX_RebuildUnitAnimForMode(pUnit, 0);
                D2COMMON_UNITS_SetAnimStartFrame(pUnit);

                if (pObjectTxt->nOverlay)
                {
                    //  BUG: the gleam comes off seven chests (0x6FB46E4F..0x6FB46E79) where OBJECT_AddIdleOverlay puts it on eight:
                    //  OBJECT_CHEST_SPECIALCHEST (455), whose stock row has Overlay 1, is not among the seven. REACHABLE: act 5 DS1 files place that
                    //  chest, so one opened in view keeps its gleam. Whether anything else takes the overlay off is not established.
                    int32_t nClassId = pUnit->dwClassId;
                    if (nClassId == OBJECT_HORADRIC_CUBE_CHEST || nClassId == OBJECT_HORADRIC_SCROLL_CHEST || nClassId == OBJECT_SPARKLY_CHEST || nClassId == OBJECT_KHALIM_CHEST1 || nClassId == OBJECT_KHALIM_CHEST3 || nClassId == OBJECT_KHALIM_CHEST2
                        || nClassId == OBJECT_STAFF_OF_KINGS_CHEST)
                        GFX_RemoveOverlay(pUnit, 0, OVERLAY_MULTIGLEAM);
                }

                NPCMENU_SetUnitFlag(pUnit, UNITFLAG_TARGETABLE, pObjectTxt->nSelectable[pUnit->dwAnimMode]);
                OBJECT_SetLight(pUnit, pObjectTxt->nLit[OBJMODE_OPENED], pObjectTxt->nRed, pObjectTxt->nGreen, pObjectTxt->nBlue);
                if (!pObjectTxt->nHasCollision[pUnit->dwAnimMode] && pObjectTxt->nHasCollision[OBJMODE_OPERATING])
                    D2COMMON_UNITS_FreeCollisionPath(pUnit);
            }
        }
        else if (pUnit->dwClassId == OBJECT_ROGUEFOUNTAIN)
        {
            int32_t nFrame = pUnit->nSeqCurrentFramePrecise - pUnit->wAnimSpeed;
            pUnit->nSeqCurrentFramePrecise = nFrame;
            if (nFrame < 0)
                pUnit->nSeqCurrentFramePrecise = nFrame + nFrameCount;
        }
        else if (pUnit->dwClassId == OBJECT_CAINPORTAL && nMode == OBJMODE_SPECIAL1)
        {
            int32_t nFrame = pUnit->nSeqCurrentFramePrecise - pUnit->wAnimSpeed;
            pUnit->nSeqCurrentFramePrecise = nFrame;
            if (nFrame < 0)
            {
                pUnit->dwAnimMode = OBJMODE_SPECIAL2;
                pUnit->nSeqCurrentFramePrecise = pObjectTxt->nStart[OBJMODE_SPECIAL2] << 8;
                GFX_RebuildUnitAnimForMode(pUnit, 0);
                D2COMMON_UNITS_SetAnimStartFrame(pUnit);
            }
        }
        else
        {
            int32_t nFrame = pUnit->nSeqCurrentFramePrecise + pUnit->wAnimSpeed;
            pUnit->nSeqCurrentFramePrecise = nFrame;
            if (nFrame >= nFrameCount)
            {
                if (!bCycleAnim)
                    pUnit->nSeqCurrentFramePrecise = nFrameCount - 0x100;
                else
                    pUnit->nSeqCurrentFramePrecise = (pObjectTxt->nStart[nMode] << 8) - nFrameCount + nFrame;
            }
        }
    }

    if (pUnit->pObjectData->pObjectTxt->nClientFn <= 3 || OBJECT_ClientProcess(pUnit))
        UNITSND_UpdateUnitModeSound(pUnit);
}

//1.10f: D2Client.0x6FB46FB0
//  Advances a door's animation, OBJECT_AdvanceAnim's door arm. Mode 1 (OPERATING, opening) steps forward and clamps at the last frame; mode 3
//  (SPECIAL1, closing) steps back and clamps at zero. A door already at that end is handed to OBJECT_FinishDoorAnim by a tail jump (0x6FB46FD1,
//  0x6FB46FF5); any other mode warns at Object.cpp:337.
void __fastcall OBJECT_AdvanceDoorAnim(D2UnitStrc *pUnit)
{
    uint32_t nMode = pUnit->dwAnimMode;
    int32_t nFrameCount = (int32_t)pUnit->pObjectData->pObjectTxt->dwFrameCnt[nMode];

    if (nMode == OBJMODE_OPERATING)
    {
        int32_t nLastFrame = nFrameCount - 0x100;
        int32_t nFrame = pUnit->nSeqCurrentFramePrecise;
        if (nFrame == nLastFrame)
        {
            OBJECT_FinishDoorAnim(pUnit);
            return;
        }
        nFrame += pUnit->wAnimSpeed;
        pUnit->nSeqCurrentFramePrecise = nFrame;
        if (nFrame > nLastFrame)
            pUnit->nSeqCurrentFramePrecise = nLastFrame;
        return;
    }

    if (nMode == OBJMODE_SPECIAL1)
    {
        int32_t nFrame = pUnit->nSeqCurrentFramePrecise;
        if (nFrame <= 0)
        {
            OBJECT_FinishDoorAnim(pUnit);
            return;
        }
        int32_t nSpeed = pUnit->wAnimSpeed;
        if (nSpeed > nFrame)
            pUnit->nSeqCurrentFramePrecise = 0;
        else
            pUnit->nSeqCurrentFramePrecise = nFrame - nSpeed;
        return;
    }

    FOG_DisplayWarning("0", szObjectCpp, 337);
}

//1.10f: D2Client.0x6FB47030
//  Ends a door's opening or closing. Closing (SPECIAL1) becomes NEUTRAL; opening (OPERATING) frees the collision path and becomes OPENED; any
//  other mode warns at Object.cpp:283, dead from its one caller, which arrives in those two modes only. Then the selectable bit follows the new
//  mode's nSelectable.
//  BUG: a dead store: the start frame goes into the counter's fraction byte (xor edx, edx / mov dl, [ecx+129h], 0x6FB47045, 0x6FB47049 and
//  mov dl, [ecx+12Bh], 0x6FB4707C), where OBJECT_AdvanceAnim shifts it into the frame byte. INERT: both arms then call UNITS_SetAnimStartFrame,
//  whose object arm writes the counter again as nStart[mode] << 8 on every path, so a door starts each mode at that mode's nStart and the raw
//  byte is never seen. Reproduced.
void __fastcall OBJECT_FinishDoorAnim(D2UnitStrc *pUnit)
{
    uint32_t nMode = pUnit->dwAnimMode;
    if (nMode == OBJMODE_SPECIAL1)
    {
        pUnit->dwAnimMode = OBJMODE_NEUTRAL;
        pUnit->nSeqCurrentFramePrecise = pUnit->pObjectData->pObjectTxt->nStart[OBJMODE_NEUTRAL];
        GFX_RebuildUnitAnimForMode(pUnit, 0);
        D2COMMON_UNITS_SetAnimStartFrame(pUnit);
    }
    else if (nMode == OBJMODE_OPERATING)
    {
        D2COMMON_UNITS_FreeCollisionPath(pUnit);
        pUnit->dwAnimMode = OBJMODE_OPENED;
        pUnit->nSeqCurrentFramePrecise = pUnit->pObjectData->pObjectTxt->nStart[OBJMODE_OPENED];
        GFX_RebuildUnitAnimForMode(pUnit, 0);
        D2COMMON_UNITS_SetAnimStartFrame(pUnit);
    }
    else
    {
        FOG_DisplayWarning("0", szObjectCpp, 283);
    }

    if (pUnit->pObjectData->pObjectTxt->nSelectable[pUnit->dwAnimMode])
        pUnit->dwFlags |= UNITFLAG_TARGETABLE;
    else
        pUnit->dwFlags &= ~UNITFLAG_TARGETABLE;
}

//  The shrine chart: gShrineChart (0x6FB70D30) has 23 rows of D2ClientShrineChartStrc, one per shrines.txt Code, bounded by gnShrineChartSize (23
//  in the image; the names follow the assert "ptShrineData->bCode < sgnShrineChartSize"). pfnApplyOverlays is OBJECT_ApplyShrineOverlays on rows
//  6..15 (the Booster shrines) and NULL elsewhere; pfnClientEffect is set on rows 16, 19, 21 and 22 (the four effects below); nOverlayId is 59..68
//  (OVERLAY_SHRINE_ARMOR..EXPERIENCE) on rows 6..15 and -1 elsewhere; nShimmerOverlay is 57 or 58 on the same rows; nSoundId is a Sounds.txt id,
//  2664 shrine_armorboost .. 2680 shrine_storm, 0 on row 0.
//  The four effects are called only through the chart, by OBJECT_DoClientAction's call eax (0x6FB476F1, 0x6FB47701), which passes the shrine, the
//  player found by the GUID in its parameters (type 0, 0x6FB476D3) and the shrine's record (0x6FB476A4), and reads nothing back.

static_assert(offsetof(D2ClientShrineChartStrc, pfnApplyOverlays) == 0
                  && offsetof(D2ClientShrineChartStrc, nOverlayId) == 8
                  && offsetof(D2ClientShrineChartStrc, nShimmerOverlay) == 0xC,
              "the shrine chart: 20-byte rows, `lea [x+x*4]` (0x6FB47550, "
              "0x6FB4763D); [edx*4] (0x6FB47640), [edi+8] and [edi+0Ch] "
              "(0x6FB4757B, 0x6FB47591)");
static_assert(offsetof(D2ShrinesTxt, nCode) == 0
                  && offsetof(D2ObjectDataStrc, InteractType) == 4
                  && offsetof(D2PlayerDataStrc, szName) == 0,
              "`mov al, [edx]` (0x6FB47523), `mov cl, [eax+4]` (0x6FB475D9), "
              "the player data handed to _strrev as the name (0x6FB470DF)");
static_assert(MISSILE_FIREBALL == 62 && MISSILE_EXPLOSIVEPOTION == 45
                  && MISSILE_CHOKINGGASPOITION == 48,
              "`mov [esp+...], 3Eh` (0x6FB471EC), `2Dh` (0x6FB47368), "
              "`30h` (0x6FB474B8)");

//1.10f: D2Client.0x6FB470D0
//  The Shrine of Enirhs, chart row 16: reverse the player's name. Its shrines.txt row is Magic, "reverses character name (OUT)".
//  UNREACHABLE: no object carries shrine code 16 from a stock server.
//  `retn 4` purges the third argument, which the body never reads.
//  DIFFERENCE: the original calls strncpy(szName, _strrev(szName), 16) (0x6FB470DD..0x6FB470EA); _strrev returns its argument, so the copy is onto
//  itself, which is undefined in C. What the image's own strncpy (0x6FB60A50) computes for it is defined: it reads each dword before writing it
//  back and zero-fills after the terminator, so the reversed name stays and the bytes after its NUL, up to 16, become zero. That is written out
//  below instead of the self-copy.
void __fastcall OBJECT_ShrineOfEnirhsEffect(D2UnitStrc *pShrine, D2UnitStrc *pPlayer, D2ShrinesTxt *ptShrineData)
{
    D2PlayerDataStrc *pPlayerData = D2COMMON_UNITS_GetPlayerData(pPlayer);
    if (!pPlayerData)
        return;

    char *szName = pPlayerData->szName;
    _strrev(szName);
    size_t nLength = strnlen(szName, 16);
    memset(szName + nLength, 0, 16 - nLength);
}

//1.10f: D2Client.0x6FB47100
//  The Storm shrine, chart row 19: sixteen fireballs (missile 62) from the shrine, owned by the unit that used it, at level clamp(clvl / 5, 1, 8).
//  shrines.txt: "all players and monsters lose 1/2 current HP, shoots fireballs"; the health is the server's.
//  The targets are offsets (MISSILESPAWN_TARGETOFFSET). Each axis walks a positive and a negative counter, each stepping by 5, and a parity counter
//  picks one: +5, -10, +15, -20. The row's parity is taken once per row (and eax, 1, 0x6FB4712F), the column's per missile (test byte ptr,
//  0x6FB471CE). The shrine's position is read again for every missile.
//  Signed throughout: clvl / 5 is imul 66666667h, sar edx, 1 and the sign bit added (0x6FB471FB..0x6FB47209); the clamp is jge/jle, both loop
//  bounds jl. void: the dispatcher reads nothing. The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall OBJECT_StormShrineEffect(D2UnitStrc *pShrine, D2UnitStrc *pPlayer, D2ShrinesTxt *ptShrineData)
{
    int32_t nXParity = 1;
    int32_t nXNegative = -5;
    int32_t nXPositive = 5;
    do
    {
        int32_t bXOdd = nXParity & 1;
        int32_t nYParity = 1;
        int32_t nYNegative = -5;
        for (int32_t nYPositive = 5; nYPositive < 25; nYPositive += 5)
        {
            D2MissileStrc tParams;
            memset(&tParams, 0, sizeof(tParams));
            tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETOFFSET;
            tParams.nX = UNITS_GetXPosition(pShrine);
            tParams.nY = UNITS_GetYPosition(pShrine);
            tParams.nTargetX = bXOdd ? nXPositive : nXNegative;
            tParams.nTargetY = (nYParity & 1) ? nYPositive : nYNegative;
            tParams.pOwner = pPlayer;
            tParams.nMissile = MISSILE_FIREBALL;
            int32_t nLevel = D2COMMON_STATLIST_UnitGetStatValue(pPlayer, STAT_LEVEL, 0) / 5;
            if (nLevel < 1)
                nLevel = 1;
            else if (nLevel > 8)
                nLevel = 8;
            tParams.nSkillLevel = nLevel;
            MISSILE_ClientSpawn(&tParams);
            ++nYParity;
            nYNegative -= 5;
        }
        ++nXParity;
        nXPositive += 5;
        nXNegative -= 5;
    } while (nXPositive < 25);
}

//1.10f: D2Client.0x6FB47280
//  The Exploding shrine, chart row 21: six explosive potions (missile 45) lobbed from the shrine -- MISSILESPAWN_TARGETPOS | ARC | FRAMESTOTARGET,
//  0x520, an arc of nGfxArg 1 -- to the shrine's position plus {-6, -6, 0, 0, 6, 6} on X and {6, -6, 6, -6, 6, -6} on Y, a 3 by 2 grid, at level
//  clamp(clvl / 5, 1, 8). shrines.txt: "drops 5-10 exploding potions & tosses out exploding potions in random directions/distances"; the drop is
//  the server's.
//  The two tables are built on the stack at every call, twelve stores; the index is a byte offset stepped by 4 to 18h (jl, 0x6FB473B0). The
//  prologue loads -6 into ecx (mov ecx, 0FFFFFFFAh, 0x6FB4728B). The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall OBJECT_ExplodingShrineEffect(D2UnitStrc *pShrine, D2UnitStrc *pPlayer, D2ShrinesTxt *ptShrineData)
{
    int32_t aOffsetX[6] = { -6, -6, 0, 0, 6, 6 };
    int32_t aOffsetY[6] = { 6, -6, 6, -6, 6, -6 };
    int32_t nIndex = 0;
    do
    {
        D2MissileStrc tParams;
        memset(&tParams, 0, sizeof(tParams));
        tParams.dwFlags = MISSILESPAWN_TARGETPOS | MISSILESPAWN_ARC | MISSILESPAWN_FRAMESTOTARGET;
        tParams.nX = UNITS_GetXPosition(pShrine);
        tParams.nY = UNITS_GetYPosition(pShrine);
        tParams.nTargetX = tParams.nX + aOffsetX[nIndex];
        tParams.nTargetY = tParams.nY + aOffsetY[nIndex];
        tParams.pOwner = pPlayer;
        tParams.pOrigin = pShrine;
        tParams.nGfxArg = 1;
        tParams.nMissile = MISSILE_EXPLOSIVEPOTION;
        int32_t nLevel = D2COMMON_STATLIST_UnitGetStatValue(pPlayer, STAT_LEVEL, 0) / 5;
        if (nLevel < 1)
            nLevel = 1;
        else if (nLevel > 8)
            nLevel = 8;
        tParams.nSkillLevel = nLevel;
        MISSILE_ClientSpawn(&tParams);
        ++nIndex;
    } while (nIndex < 6);
}

//1.10f: D2Client.0x6FB473D0
//  The Poison shrine, chart row 22: OBJECT_ExplodingShrineEffect with choking gas potions, missile 48 (chokinggaspoition, the table's own spelling);
//  the two bodies differ in one constant, 2Dh against 30h (0x6FB47368, 0x6FB474B8). shrines.txt: "drops 5-10 poison gas potions & creates ring of
//  poison gas around shrine". Written out a second time because the image has two bodies. The original inlines UNITS_GetXPosition and
//  UNITS_GetYPosition here.
void __fastcall OBJECT_PoisonShrineEffect(D2UnitStrc *pShrine, D2UnitStrc *pPlayer, D2ShrinesTxt *ptShrineData)
{
    int32_t aOffsetX[6] = { -6, -6, 0, 0, 6, 6 };
    int32_t aOffsetY[6] = { 6, -6, 6, -6, 6, -6 };
    int32_t nIndex = 0;
    do
    {
        D2MissileStrc tParams;
        memset(&tParams, 0, sizeof(tParams));
        tParams.dwFlags = MISSILESPAWN_TARGETPOS | MISSILESPAWN_ARC | MISSILESPAWN_FRAMESTOTARGET;
        tParams.nX = UNITS_GetXPosition(pShrine);
        tParams.nY = UNITS_GetYPosition(pShrine);
        tParams.nTargetX = tParams.nX + aOffsetX[nIndex];
        tParams.nTargetY = tParams.nY + aOffsetY[nIndex];
        tParams.pOwner = pPlayer;
        tParams.pOrigin = pShrine;
        tParams.nGfxArg = 1;
        tParams.nMissile = MISSILE_CHOKINGGASPOITION;
        int32_t nLevel = D2COMMON_STATLIST_UnitGetStatValue(pPlayer, STAT_LEVEL, 0) / 5;
        if (nLevel < 1)
            nLevel = 1;
        else if (nLevel > 8)
            nLevel = 8;
        tParams.nSkillLevel = nLevel;
        MISSILE_ClientSpawn(&tParams);
        ++nIndex;
    } while (nIndex < 6);
}

//1.10f: D2Client.0x6FB47520
//  Puts a Booster shrine's overlays on it: the chart's pfnApplyOverlays on rows 6..15, called by OBJECT_InitShrine and by OBJECT_DoClientAction
//  (call edi, 0x6FB478C2). For each of the row's two overlays that is not -1 -- the shrine's own (59..68) and its shimmer (57 or 58) -- the
//  overlay is removed, and put back only while the object is NEUTRAL; an object in any other mode keeps neither.
//  The second argument is the shrine's record, and the byte read at +0 is its nCode (the assert's ptShrineData->bCode). The bound is signed (jl,
//  0x6FB47530).
//  BUG: the NULL test on pObject comes after pObject has gone to GFX_RemoveOverlay, and a NULL pObject reaches GFX_AddOverlay (both callees test
//  the unit themselves). UNREACHABLE: the chart is this function's only way in, and both of the chart's callers have read pUnit->pObjectData
//  before the call. The shape !p || !p->dwAnimMode reads like a NULL-safe mode getter inlined; it is not D2Common's UNITS_GetAnimOrSeqMode
//  (#10369), which also reads the seq mode.
void __fastcall OBJECT_ApplyShrineOverlays(D2UnitStrc *pObject, D2ShrinesTxt *ptShrineData)
{
    int32_t nCode = ptShrineData->nCode;
    if (nCode >= gnShrineChartSize)
    {
        FOG_DisplayAssert("ptShrineData->bCode < sgnShrineChartSize", szObjectCpp, 819);
        exit(-1);
    }

    D2ClientShrineChartStrc *pChartRow = &gShrineChart[nCode];
    if (pChartRow->nOverlayId != (D2_OverlayIds)-1)
    {
        GFX_RemoveOverlay(pObject, 0, pChartRow->nOverlayId);
        if (!pObject || pObject->dwAnimMode == OBJMODE_NEUTRAL)
            GFX_AddOverlay(pObject, pChartRow->nOverlayId, OVERLAYTYPE_LOOP, 0, 0, 0, 0, 0);
    }
    if (pChartRow->nShimmerOverlay != (D2_OverlayIds)-1)
    {
        GFX_RemoveOverlay(pObject, 0, pChartRow->nShimmerOverlay);
        if (!pObject || pObject->dwAnimMode == OBJMODE_NEUTRAL)
            GFX_AddOverlay(pObject, pChartRow->nShimmerOverlay, OVERLAYTYPE_LOOP, 0, 0, 0, 0, 0);
    }
}

//1.10f: D2Client.0x6FB475D0
//  Makes a new object a shrine, from CUNIT_CreateObjectUnit once UNITS_IsShrine says it is one: the shrine's code is the object data's
//  InteractType byte (0x6FB475D9, zero-extended); its shrines.txt record is stored in the object, read straight back and asserted (Object.cpp:886),
//  its code bounded by the chart (887), and the chart row's pfnApplyOverlays run on it when the row has one.
//  void: eax is the record only on the rows without a hook, and whatever OBJECT_ApplyShrineOverlays leaves on the others; the one caller discards
//  it. The original has the block from the read-back to the call twice, as an inline block with no out-of-line body: OBJECT_DoClientAction ends
//  with it, the same two asserts at the same lines, 886 and 887.
void __fastcall OBJECT_InitShrine(D2UnitStrc *pUnit)
{
    D2COMMON_UNITS_SetShrineTxtRecordInObjectData(pUnit, D2COMMON_DATATBLS_GetShrinesTxtRecord(pUnit->pObjectData->InteractType));

    D2ShrinesTxt *ptShrineData = D2COMMON_UNITS_GetShrineTxtRecordFromObject(pUnit);
    if (!ptShrineData)
    {
        FOG_DisplayAssert("ptShrineData", szObjectCpp, 886);
        exit(-1);
    }
    if ((int32_t)ptShrineData->nCode >= gnShrineChartSize)
    {
        FOG_DisplayAssert("ptShrineData->bCode < sgnShrineChartSize", szObjectCpp, 887);
        exit(-1);
    }

    D2ClientShrineChartStrc *pChartRow = &gShrineChart[ptShrineData->nCode];
    if (pChartRow->pfnApplyOverlays)
        pChartRow->pfnApplyOverlays(pUnit, ptShrineData);
}

static_assert(CLTACTION_OBJSETMODE == 3 && CLTACTION_SKILL_XY == 21
                  && NUM_OBJECTMODES == 8,
              "OBJECT_DoClientAction: `cmp ecx, 3` (0x6FB47663), `cmp ecx, 15h` "
              "(0x6FB4766F); OBJECT_SetClientMode: `cmp ebx, 8` (0x6FB478D9)");
static_assert(offsetof(D2ObjectsTxt, nShrineFunction) == 0x16F
                  && offsetof(D2ClientShrineChartStrc, pfnClientEffect) == 4
                  && offsetof(D2ClientShrineChartStrc, nSoundId) == 0x10
                  && UNITFLAG_DRAWSHADOW == 0x20,
              "`mov cl, [eax+16Fh]` (0x6FB476C3), `(gShrineChart+4)[ecx*4]` "
              "(0x6FB476F1), `(gShrineChart+10h)[edx*4]` (0x6FB47BCC), "
              "`or al, 20h` (0x6FB4783F, 0x6FB47B46)");

//1.10f: D2Client.0x6FB47660
//  An object's client action, UNITMODE_DispatchActionByUnitType's object arm: two actions, and a warning at Object.cpp:920 for any other.
//  CLTACTION_OBJSETMODE (3) puts the object into the mode pParams[1] names. A door goes to OBJECT_SetClientMode, with pParams[0] as its selectable
//  bit. Any other object changes mode here: the chests' gleam put back when the new mode is NEUTRAL, the selectable bit, the collision path freed
//  when the new mode has none, UNITFLAG_PREDRAW when its nOrderFlag is 1, and the static light from nLit[pParams[1]] -- the requested mode's,
//  where the other per-mode reads take dwAnimMode; UNITS_ChangeAnimMode has just made the two equal. Both then update the mode sound, the start
//  frame and the shadow bit, and a shrine runs its chart hook -- the block OBJECT_InitShrine also holds.
//  The seven chests are OBJECT_AdvanceAnim's (see the BUG there). The radius goes as movzx dx over pParams[1] (0x6FB47810), so its upper word is
//  the mode's; OBJECT_SetLight reads di alone, and the colours are partial-register loads as in OBJECT_ApplyDayNightMode.
//  CLTACTION_SKILL_XY (21), for an object the operate, runs the shrine's client effect -- the chart's pfnClientEffect, rows 1 up to
//  gnShrineChartSize - 1 (jle, jge, 0x6FB476E4, 0x6FB476EC) -- for the player whose GUID is pParams[0], then its sound (OBJECT_PlayShrineSound).
//  An unknown player ends it (0x6FB476DC).
//  BUG: a shrine's record is read untested here (mov al, [ebp+0], 0x6FB476A8), where the other arm asserts it. LATENT: OBJECT_InitShrine asserted
//  it when the object was made, for every object UNITS_IsShrine accepts.
//  A non-shrine takes its code from objects.txt's ShrineFunction, 0 on all 573 rows of the stock table, so it reaches no effect and
//  OBJECT_PlayShrineSound's first assert (Object.cpp:841) would stop the client. UNREACHABLE from a stock server: an object action 21 is sent only
//  for an object whose objects.txt SubClass has bit 0, the bit UNITS_IsShrine tests. The test of pUnit for NULL (0x6FB476AF) follows
//  UNITS_IsShrine, which reads pUnit->dwUnitType untested: the idiom kept in its source shape.
void __fastcall OBJECT_DoClientAction(D2C_ClientActions nAction, D2UnitStrc *pUnit, int32_t *pParams)
{
    if (nAction == CLTACTION_OBJSETMODE)
    {
        D2ObjectsTxt *pObjectTxt = pUnit->pObjectData->pObjectTxt;
        if (pObjectTxt->nIsDoor)
        {
            OBJECT_SetClientMode(pUnit, pParams[0], pParams[1]);
        }
        else
        {
            D2COMMON_UNITS_ChangeAnimMode(pUnit, pParams[1]);
            GFX_RebuildUnitAnimForMode(pUnit, 0);
            if (pObjectTxt->nOverlay)
            {
                //  BUG: OBJECT_CHEST_SPECIALCHEST is not in this list either, as in OBJECT_AdvanceAnim. REACHABLE: see there.
                int32_t nClassId = pUnit->dwClassId;
                if ((nClassId == OBJECT_HORADRIC_CUBE_CHEST || nClassId == OBJECT_HORADRIC_SCROLL_CHEST || nClassId == OBJECT_SPARKLY_CHEST || nClassId == OBJECT_KHALIM_CHEST1 || nClassId == OBJECT_KHALIM_CHEST3 || nClassId == OBJECT_KHALIM_CHEST2
                     || nClassId == OBJECT_STAFF_OF_KINGS_CHEST) && pUnit->dwAnimMode == OBJMODE_NEUTRAL)
                    GFX_AddOverlay(pUnit, OVERLAY_MULTIGLEAM, OVERLAYTYPE_LOOP, 0, 0, 0, 0, 0);
            }

            if (pObjectTxt->nSelectable[pUnit->dwAnimMode])
                pUnit->dwFlags |= UNITFLAG_TARGETABLE;
            else
                pUnit->dwFlags &= ~UNITFLAG_TARGETABLE;
            if (!pObjectTxt->nHasCollision[pUnit->dwAnimMode])
                D2COMMON_UNITS_FreeCollisionPath(pUnit);
            if (pObjectTxt->nOrderFlag[pUnit->dwAnimMode] == 1)
                pUnit->dwFlags |= UNITFLAG_PREDRAW;
            OBJECT_SetLight(pUnit, pObjectTxt->nLit[pParams[1]], pObjectTxt->nRed, pObjectTxt->nGreen, pObjectTxt->nBlue);
        }

        UNITSND_UpdateUnitModeSound(pUnit);
        D2COMMON_UNITS_SetAnimStartFrame(pUnit);
        if (pObjectTxt->nBlocksLight[pUnit->dwAnimMode])
            pUnit->dwFlags &= ~UNITFLAG_DRAWSHADOW;
        else
            pUnit->dwFlags |= UNITFLAG_DRAWSHADOW;

        if (D2COMMON_UNITS_IsShrine(pUnit))
        {
            D2ShrinesTxt *ptShrineData = D2COMMON_UNITS_GetShrineTxtRecordFromObject(pUnit);
            if (!ptShrineData)
            {
                FOG_DisplayAssert("ptShrineData", szObjectCpp, 886);
                exit(-1);
            }
            if ((int32_t)ptShrineData->nCode >= gnShrineChartSize)
            {
                FOG_DisplayAssert("ptShrineData->bCode < sgnShrineChartSize", szObjectCpp, 887);
                exit(-1);
            }

            D2ClientShrineChartStrc *pChartRow = &gShrineChart[ptShrineData->nCode];
            if (pChartRow->pfnApplyOverlays)
                pChartRow->pfnApplyOverlays(pUnit, ptShrineData);
        }
        return;
    }

    if (nAction != CLTACTION_SKILL_XY)
    {
        FOG_DisplayWarning("0", szObjectCpp, 920);
        return;
    }

    D2ShrinesTxt *pShrineTxt = NULL;
    int32_t nCode;
    if (D2COMMON_UNITS_IsShrine(pUnit))
    {
        pShrineTxt = D2COMMON_UNITS_GetShrineTxtRecordFromObject(pUnit);
        nCode = pShrineTxt->nCode;
    }
    else
    {
        nCode = D2COMMON_DATATBLS_GetObjectsTxtRecord(pUnit ? (int32_t)pUnit->dwClassId : -1)->nShrineFunction;
    }

    D2UnitStrc *pPlayer = CUNIT_FindUnitByGuidAndType((uint32_t)pParams[0], UNIT_PLAYER);
    if (!pPlayer)
        return;

    if (nCode > 0 && nCode < gnShrineChartSize)
    {
        D2ClientShrineChartStrc *pChartRow = &gShrineChart[nCode];
        if (pChartRow->pfnClientEffect)
            pChartRow->pfnClientEffect(pUnit, pPlayer, pShrineTxt);
    }
    OBJECT_PlayShrineSound(pUnit, pPlayer);
}

//1.10f: D2Client.0x6FB478D0
//  Sets a door's mode from the server, OBJECT_DoClientAction's door arm. nMode is the action block's second dword (the assert calls it nParam2,
//  "nParam2 < NUM_OBJECTMODES", Object.cpp:524) and bSelectable its first; the bound is signed (jl, 0x6FB478E1), so a negative mode passes it.
//  Two requests animate when the door has frames for them (the animating mode's dwFrameCnt not 0x100, one frame, OBJECT_AdvanceAnim's own test):
//  NEUTRAL asked of an OPENED door runs the closing animation (SPECIAL1), clears the selectable bit and blocks the collision path at once; OPENED
//  asked of a NEUTRAL door runs the opening one (OPERATING) and clears the bit. OBJECT_AdvanceDoorAnim and OBJECT_FinishDoorAnim end them. Any
//  other request sets the selectable bit from bSelectable and, when the mode changes, first frees the collision path on the way into OPENED,
//  SPECIAL2 or SPECIAL3 from outside those three, or blocks it on the way into NEUTRAL from any other mode. Last, the shadow bit from nBlocksLight,
//  through the record loaded on entry (ebp), as nStart[0] is read before any call (0x6FB4792E); the other two start frames are read through
//  pObject->pObjectData again, after the calls (0x6FB47A0D, 0x6FB47B20).
//  BUG: dead stores: the start frame goes into the counter raw, three times -- nStart[0] (0x6FB47935), nStart[2] (0x6FB47A22) and nStart[nMode]
//  (0x6FB47B2E), each a zero-extended byte where OBJECT_AdvanceAnim shifts it into the frame byte (OBJECT_FinishDoorAnim's store again). INERT:
//  UNITS_ChangeAnimMode calls UNITS_SetAnimStartFrame on a change, which writes an object's counter as nStart << 8, and the other two are followed
//  by OBJECT_DoClientAction's own UNITS_SetAnimStartFrame (0x6FB47826). Reproduced.
//  The room and BlockCollisionPath calls are the site's own; the original inlines UNITS_GetXPosition and UNITS_GetYPosition for the four position
//  reads before them.
void __fastcall OBJECT_SetClientMode(D2UnitStrc *pObject, int32_t bSelectable, int32_t nMode)
{
    D2ObjectsTxt *pObjectTxt = pObject->pObjectData->pObjectTxt;
    if (nMode >= NUM_OBJECTMODES)
    {
        FOG_DisplayAssert("nParam2 < NUM_OBJECTMODES", szObjectCpp, 524);
        exit(-1);
    }

    if (nMode == OBJMODE_NEUTRAL && pObject->dwAnimMode == OBJMODE_OPENED && pObjectTxt->dwFrameCnt[OBJMODE_SPECIAL1] != 0x100)
    {
        pObject->nSeqCurrentFramePrecise = pObjectTxt->nStart[OBJMODE_NEUTRAL];
        D2COMMON_UNITS_ChangeAnimMode(pObject, OBJMODE_SPECIAL1);
        GFX_RebuildUnitAnimForMode(pObject, 0);
        pObject->dwFlags &= ~UNITFLAG_TARGETABLE;
        int32_t nY = UNITS_GetYPosition(pObject);
        int32_t nX = UNITS_GetXPosition(pObject);
        D2COMMON_UNITS_BlockCollisionPath(pObject, D2COMMON_UNITS_GetRoom(pObject), nX, nY);
    }
    else if (nMode == OBJMODE_OPENED && pObject->dwAnimMode == OBJMODE_NEUTRAL && pObjectTxt->dwFrameCnt[OBJMODE_OPERATING] != 0x100)
    {
        D2COMMON_UNITS_ChangeAnimMode(pObject, OBJMODE_OPERATING);
        GFX_RebuildUnitAnimForMode(pObject, 0);
        pObject->dwFlags &= ~UNITFLAG_TARGETABLE;
        pObject->nSeqCurrentFramePrecise = pObject->pObjectData->pObjectTxt->nStart[OBJMODE_OPENED];
    }
    else
    {
        if (bSelectable)
            pObject->dwFlags |= UNITFLAG_TARGETABLE;
        else
            pObject->dwFlags &= ~UNITFLAG_TARGETABLE;

        int32_t nOldMode = (int32_t)pObject->dwAnimMode;
        if (nMode != nOldMode)
        {
            if (nMode == OBJMODE_SPECIAL2 || nMode == OBJMODE_OPENED || nMode == OBJMODE_SPECIAL3)
            {
                if (nOldMode != OBJMODE_SPECIAL3 && nOldMode != OBJMODE_OPENED && nOldMode != OBJMODE_SPECIAL2)
                    D2COMMON_UNITS_FreeCollisionPath(pObject);
            }
            else if (nMode == OBJMODE_NEUTRAL && nOldMode != OBJMODE_NEUTRAL)
            {
                int32_t nY = UNITS_GetYPosition(pObject);
                int32_t nX = UNITS_GetXPosition(pObject);
                D2COMMON_UNITS_BlockCollisionPath(pObject, D2COMMON_UNITS_GetRoom(pObject), nX, nY);
            }
            D2COMMON_UNITS_ChangeAnimMode(pObject, nMode);
            GFX_RebuildUnitAnimForMode(pObject, 0);
        }
        pObject->nSeqCurrentFramePrecise = pObject->pObjectData->pObjectTxt->nStart[nMode];
    }

    if (pObjectTxt->nBlocksLight[pObject->dwAnimMode])
        pObject->dwFlags &= ~UNITFLAG_DRAWSHADOW;
    else
        pObject->dwFlags |= UNITFLAG_DRAWSHADOW;
}

//1.10f: D2Client.0x6FB47B70
//  Plays a shrine's sound at the player who used it: the chart row's nSoundId (0x6FB47BCC), if not zero, through SOUNDINST_PlaySound with the player
//  as its unit. 22 of the 23 rows carry one, 2664..2680, row 0 none. Its own two asserts are Object.cpp:841 and 842, the record and its code
//  against the chart. void: the row's address left in eax by the lea at 0x6FB47BD3 is used by nothing.
void __fastcall OBJECT_PlayShrineSound(D2UnitStrc *pObject, D2UnitStrc *pSourceUnit)
{
    D2ShrinesTxt *ptShrineData = D2COMMON_UNITS_GetShrineTxtRecordFromObject(pObject);
    if (!ptShrineData)
    {
        FOG_DisplayAssert("ptShrineData", szObjectCpp, 841);
        exit(-1);
    }
    int32_t nCode = ptShrineData->nCode;
    if (nCode >= gnShrineChartSize)
    {
        FOG_DisplayAssert("ptShrineData->bCode < sgnShrineChartSize", szObjectCpp, 842);
        exit(-1);
    }

    D2C_Sounds nSoundId = gShrineChart[nCode].nSoundId;
    if (nSoundId)
        SOUNDINST_PlaySound(nSoundId, pSourceUnit, 0, 0, 0);
}


//1.10f: D2Client.0x6FB47BF0
//  ClientFn 2, the ripple (rows 67..70). Once, while dwFlagEx is zero, a random start phase of 0..63 goes into dwTickCount and dwFlagEx takes 1
//  (0x6FB47C20) -- the whole dword, but only over a zero one, so it is bit 0 set: UNITFLAGEX_HASINV by the enum, borrowed as a "seeded" mark.
//  Then, each call: at a multiple of 64 the object goes to OPERATING -- stored straight into dwAnimMode, not through UNITS_ChangeAnimMode -- with
//  its start frame; the counter steps; and in OPENED it goes back to NEUTRAL the same way, with 0..63 added to the counter so the next start
//  moves. It answers 1. The mode is read (0x6FB47C4C) before the counter's step is stored (0x6FB47C53); two members, so the order is the
//  compiler's. The original inlines SEED_RollRandomNumber here.
uint8_t __fastcall OBJECT_ProcessRipple(D2UnitStrc *pUnit)
{
    if (!pUnit->dwFlagEx)
    {
        pUnit->dwTickCount = (uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) & 0x3F;
        pUnit->dwFlagEx = 1;
    }

    if (!(pUnit->dwTickCount & 0x3F) && pUnit->dwAnimMode != OBJMODE_OPERATING)
    {
        pUnit->dwAnimMode = OBJMODE_OPERATING;
        D2COMMON_UNITS_SetAnimStartFrame(pUnit);
    }

    uint32_t nMode = pUnit->dwAnimMode;
    ++pUnit->dwTickCount;
    if (nMode == OBJMODE_OPENED)
    {
        uint32_t nDelay = (uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) & 0x3F;
        pUnit->dwAnimMode = OBJMODE_NEUTRAL;
        pUnit->dwTickCount += nDelay;
        D2COMMON_UNITS_SetAnimStartFrame(pUnit);
    }
    return 1;
}

//1.10f: D2Client.0x6FB47CA0
//  ClientFn 3, the ripple with its sound: OBJECT_ProcessRipple, and the mode sound updated before the answer. It still answers 1;
//  OBJECT_AdvanceAnim, the caller that reads the answer, never calls the table below ClientFn 4, and no stock objects.txt row has ClientFn 3.
//  Written out a second time because the image has two bodies. The original inlines SEED_RollRandomNumber here.
uint8_t __fastcall OBJECT_ProcessRippleWithSound(D2UnitStrc *pUnit)
{
    if (!pUnit->dwFlagEx)
    {
        pUnit->dwTickCount = (uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) & 0x3F;
        pUnit->dwFlagEx = 1;
    }

    if (!(pUnit->dwTickCount & 0x3F) && pUnit->dwAnimMode != OBJMODE_OPERATING)
    {
        pUnit->dwAnimMode = OBJMODE_OPERATING;
        D2COMMON_UNITS_SetAnimStartFrame(pUnit);
    }

    uint32_t nMode = pUnit->dwAnimMode;
    ++pUnit->dwTickCount;
    if (nMode == OBJMODE_OPENED)
    {
        uint32_t nDelay = (uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) & 0x3F;
        pUnit->dwAnimMode = OBJMODE_NEUTRAL;
        pUnit->dwTickCount += nDelay;
        D2COMMON_UNITS_SetAnimStartFrame(pUnit);
    }
    UNITSND_UpdateUnitModeSound(pUnit);
    return 1;
}

//1.10f: D2Client.0x6FB47D50
//  ClientFn 4, the drinker (row 110). When GetTickCount passes dwTickCount (jnb, unsigned, 0x6FB47D63) the next time is set 7000 + 0..4999 ms on,
//  the counter takes nStart[5] raw (0x6FB47DA7) -- mode 5's start, for a change into mode 3 -- and the object goes to mode 3. In mode 3, once the
//  counter reaches that mode's last frame (jl, signed, 0x6FB47DD6), it takes nStart[0] raw and mode 0. It updates the mode sound itself and
//  answers 0 (xor al, al, 0x6FB47DFB). The original inlines SEED_RollRandomNumber here.
//  BUG: the start frame is stored raw, a zero-extended byte where OBJECT_AdvanceAnim shifts it into the frame byte, and UNITS_ChangeAnimMode then
//  writes the new mode's nStart << 8 only on a change. INERT on the way back to NEUTRAL, where the mode always changes. LATENT when the timer fires
//  in the mode it asks for (0x6FB47DA7): the byte stays, and the stock row (110) has every Start 0, so the byte and the shifted value agree.
uint8_t __fastcall OBJECT_ProcessDrinker(D2UnitStrc *pUnit)
{
    uint32_t dwNow = GetTickCount();
    if (pUnit->dwTickCount < dwNow)
    {
        pUnit->dwTickCount = (uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) % 5000 + 7000 + dwNow;
        pUnit->nSeqCurrentFramePrecise = pUnit->pObjectData->pObjectTxt->nStart[5];
        D2COMMON_UNITS_ChangeAnimMode(pUnit, OBJMODE_SPECIAL1);
        GFX_RebuildUnitAnimForMode(pUnit, 0);
    }

    if (pUnit->dwAnimMode == OBJMODE_SPECIAL1)
    {
        D2ObjectsTxt *pObjectTxt = pUnit->pObjectData->pObjectTxt;
        if (pUnit->nSeqCurrentFramePrecise >= (int32_t)pObjectTxt->dwFrameCnt[OBJMODE_SPECIAL1] - 0x100)
        {
            pUnit->nSeqCurrentFramePrecise = pObjectTxt->nStart[OBJMODE_NEUTRAL];
            D2COMMON_UNITS_ChangeAnimMode(pUnit, OBJMODE_NEUTRAL);
            GFX_RebuildUnitAnimForMode(pUnit, 0);
        }
    }

    UNITSND_UpdateUnitModeSound(pUnit);
    return 0;
}

//1.10f: D2Client.0x6FB47E00
//  ClientFn 5, the gesturer (row 112): the drinker's shape, 23000 + 0..9999 ms, nStart[5] raw, and mode 3 or 4 on the low bit of a second roll --
//  odd 3, even 4 (0x6FB47E7A) -- ending from either. It answers 0. The bit is also stored into the frame slot the prologue's push ecx made
//  (0x6FB47E76) and never read: a spill the C does not ask for, not reproduced. The original inlines SEED_RollRandomNumber here.
//  BUG: the raw start frame of OBJECT_ProcessDrinker. INERT on the way back to NEUTRAL; LATENT when the timer fires in the mode it asks for
//  (0x6FB47E5C), the stock row (112) having every Start 0.
uint8_t __fastcall OBJECT_ProcessGesturer(D2UnitStrc *pUnit)
{
    uint32_t dwNow = GetTickCount();
    if (pUnit->dwTickCount < dwNow)
    {
        pUnit->dwTickCount = (uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) % 10000 + 23000 + dwNow;
        pUnit->nSeqCurrentFramePrecise = pUnit->pObjectData->pObjectTxt->nStart[5];
        int32_t bOdd = (int32_t)((uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) & 1);
        D2COMMON_UNITS_ChangeAnimMode(pUnit, bOdd ? OBJMODE_SPECIAL1 : OBJMODE_SPECIAL2);
        GFX_RebuildUnitAnimForMode(pUnit, 0);
    }

    uint32_t nMode = pUnit->dwAnimMode;
    if (nMode == OBJMODE_SPECIAL1 || nMode == OBJMODE_SPECIAL2)
    {
        D2ObjectsTxt *pObjectTxt = pUnit->pObjectData->pObjectTxt;
        if (pUnit->nSeqCurrentFramePrecise >= (int32_t)pObjectTxt->dwFrameCnt[nMode] - 0x100)
        {
            pUnit->nSeqCurrentFramePrecise = pObjectTxt->nStart[OBJMODE_NEUTRAL];
            D2COMMON_UNITS_ChangeAnimMode(pUnit, OBJMODE_NEUTRAL);
            GFX_RebuildUnitAnimForMode(pUnit, 0);
        }
    }

    UNITSND_UpdateUnitModeSound(pUnit);
    return 0;
}

//1.10f: D2Client.0x6FB47EE0
//  ClientFn 6, the turner (row 114): 20000 + 0..9999 ms, nStart[5] raw, mode 3; it ends from mode 3 or above (jb, unsigned, 0x6FB47F50) and only
//  when the counter equals that mode's last frame (jnz, 0x6FB47F69), where the drinker and the gesturer test >=. It answers 0. The original
//  inlines SEED_RollRandomNumber here.
//  BUG: the raw start frame of OBJECT_ProcessDrinker. INERT on the way back to NEUTRAL; LATENT when the timer fires in the mode it asks for
//  (0x6FB47F37), the stock row (114) having every Start 0.
uint8_t __fastcall OBJECT_ProcessTurner(D2UnitStrc *pUnit)
{
    uint32_t dwNow = GetTickCount();
    if (pUnit->dwTickCount < dwNow)
    {
        pUnit->dwTickCount = (uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) % 10000 + 20000 + dwNow;
        pUnit->nSeqCurrentFramePrecise = pUnit->pObjectData->pObjectTxt->nStart[5];
        D2COMMON_UNITS_ChangeAnimMode(pUnit, OBJMODE_SPECIAL1);
        GFX_RebuildUnitAnimForMode(pUnit, 0);
    }

    uint32_t nMode = pUnit->dwAnimMode;
    if (nMode >= OBJMODE_SPECIAL1)
    {
        D2ObjectsTxt *pObjectTxt = pUnit->pObjectData->pObjectTxt;
        if (pUnit->nSeqCurrentFramePrecise == (int32_t)pObjectTxt->dwFrameCnt[nMode] - 0x100)
        {
            pUnit->nSeqCurrentFramePrecise = pObjectTxt->nStart[OBJMODE_NEUTRAL];
            D2COMMON_UNITS_ChangeAnimMode(pUnit, OBJMODE_NEUTRAL);
            GFX_RebuildUnitAnimForMode(pUnit, 0);
        }
    }

    UNITSND_UpdateUnitModeSound(pUnit);
    return 0;
}

//1.10f: D2Client.0x6FB47FA0
//  ClientFn 7, the skeleton spawn (rows 259 and 373). In mode 0, once GetTickCount passes dwTickCount: ten seconds on, nStart[1] raw (0x6FB47FDA),
//  mode 1. In any other mode, once the time passes: 4500 + 0..2999 ms on, and mode 0. It answers 1 on every path. The "any other mode" arm carries
//  a cmp eax, 1 / jb (0x6FB47FEF) where eax is already nonzero, so it cannot fail; written as the original's shape. The original inlines
//  SEED_RollRandomNumber here.
//  BUG: the raw start frame of OBJECT_ProcessDrinker. INERT: the object always changes mode, so UNITS_ChangeAnimMode writes nStart << 8 after it.
uint8_t __fastcall OBJECT_ProcessSkeletonSpawn(D2UnitStrc *pUnit)
{
    uint32_t dwNow = GetTickCount();
    uint32_t nMode = pUnit->dwAnimMode;
    if (nMode == OBJMODE_NEUTRAL)
    {
        if (pUnit->dwTickCount < dwNow)
        {
            pUnit->dwTickCount = dwNow + 10000;
            pUnit->nSeqCurrentFramePrecise = pUnit->pObjectData->pObjectTxt->nStart[OBJMODE_OPERATING];
            D2COMMON_UNITS_ChangeAnimMode(pUnit, OBJMODE_OPERATING);
            GFX_RebuildUnitAnimForMode(pUnit, 0);
            return 1;
        }
    }
    else if (nMode >= OBJMODE_OPERATING && pUnit->dwTickCount < dwNow)
    {
        pUnit->dwTickCount = (uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) % 3000 + 4500 + dwNow;
        D2COMMON_UNITS_ChangeAnimMode(pUnit, OBJMODE_NEUTRAL);
        GFX_RebuildUnitAnimForMode(pUnit, 0);
    }
    return 1;
}

//1.10f: D2Client.0x6FB48050
//  ClientFn 10, the ice cave bubbles (row 528): OBJECT_ProcessSkeletonSpawn with other constants -- mode 1 held 3000 ms (10000 there), mode 0 for
//  2500 + 0..4999 ms (4500 + 0..2999). Written out a second time because the image has two bodies. It has the skeleton spawn's raw start frame,
//  INERT likewise. The original inlines SEED_RollRandomNumber here.
uint8_t __fastcall OBJECT_ProcessIceBubbles(D2UnitStrc *pUnit)
{
    uint32_t dwNow = GetTickCount();
    uint32_t nMode = pUnit->dwAnimMode;
    if (nMode == OBJMODE_NEUTRAL)
    {
        if (pUnit->dwTickCount < dwNow)
        {
            pUnit->dwTickCount = dwNow + 3000;
            pUnit->nSeqCurrentFramePrecise = pUnit->pObjectData->pObjectTxt->nStart[OBJMODE_OPERATING];
            D2COMMON_UNITS_ChangeAnimMode(pUnit, OBJMODE_OPERATING);
            GFX_RebuildUnitAnimForMode(pUnit, 0);
            return 1;
        }
    }
    else if (nMode >= OBJMODE_OPERATING && pUnit->dwTickCount < dwNow)
    {
        pUnit->dwTickCount = (uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) % 5000 + 2500 + dwNow;
        D2COMMON_UNITS_ChangeAnimMode(pUnit, OBJMODE_NEUTRAL);
        GFX_RebuildUnitAnimForMode(pUnit, 0);
    }
    return 1;
}

//  The slots of sgpfnProcessFunctions: 0 empty, 1 and 11 OBJECT_ProcessNothing, 2..7 and 10 the bodies above, 8 the Horadric orifice, 9 the client
//  smoke, 12 the Ancients' altar, 13 the invisible Ancient, 14 the rogue bonfire, 15 frozen Anya, 16 Baal's portal, 17 the zoo, 18 the keeper.
//  The rows by ClientFn: 1 row 45 (AmbientSound), 8 row 152 (orifice), 9 row 478 (clientsmoke), 11 row 557 ("Baal's lair"), 12 row 546, 13 row 561,
//  14 row 39 (RogueBonfire), 15 row 558, 16 rows 563 and 569, 17 row 567 and 18 row 568 (both "test data"); 0, the other 551 of the 573 data rows.

static_assert(OVERLAY_NPCALERT == 72
                  && QFLAG_REWARDGRANTED == 0 && QFLAG_ENTERAREA == 4,
              "OBJECT_ProcessFrozenAnya: `mov edx, 48h` and `push 48h` "
              "(0x6FB48289, 0x6FB482A5); OBJECT_ProcessInvisibleAncient: "
              "`push 0` and `push 4` as nState (0x6FB4819A, 0x6FB481AD)");

//1.10f: D2Client.0x6FB48100
//  ClientFn 1 and 11, nothing to do: mov al, 1 / retn, the answer that lets OBJECT_AdvanceAnim update the mode sound. One body in two slots.
uint8_t __fastcall OBJECT_ProcessNothing(D2UnitStrc *pUnit)
{
    return 1;
}

//1.10f: D2Client.0x6FB48110
//  ClientFn 9, the client smoke (row 478). The first call stamps GetTickCount into dwTickCount; while less than a second has passed (jbe,
//  unsigned, on the difference, 0x6FB48130) nothing; then, if the player is more than 25 away (jle, signed, 0x6FB48141) the object removes itself,
//  CUNIT_RemoveUnitByClientId(dwUnitId, UNIT_OBJECT), and answers 0, else it stamps the time again. The 0 keeps OBJECT_AdvanceAnim off the unit it
//  has just freed; CUNIT_UpdateAllUnits looks the unit up again by its id before its own call, so a freed one is not called twice.
//  BUG: the player is not tested for NULL: D2Common's distance code reads the second unit's type untested (D2Common 0x6FDC2E42, 0x6FDC2E48), so a
//  NULL player would fault there. LATENT: this body runs only from CUNIT_UpdateAllUnits, inside the tick GAME_UpdateFrame runs while gbInGame is
//  set, and the writer that sets it halts unless the player's room is found. Whether the server can remove the player while gbInGame is set is
//  not established.
uint8_t __fastcall OBJECT_ProcessClientSmoke(D2UnitStrc *pUnit)
{
    uint32_t dwNow = GetTickCount();
    uint32_t dwStamp = pUnit->dwTickCount;
    if (dwStamp)
    {
        if (dwNow - dwStamp <= 1000)
            return 1;
        if (D2COMMON_UNITS_GetDistanceToOtherUnit(pUnit, CUNIT_GetPlayer()) > 25)
        {
            CUNIT_RemoveUnitByClientId((int32_t)pUnit->dwUnitId, UNIT_OBJECT);
            return 0;
        }
    }
    pUnit->dwTickCount = dwNow;
    return 1;
}

//1.10f: D2Client.0x6FB48160
//  ClientFn 12, the Ancients' altar (row 546): out of NEUTRAL, preload the Ancients' statue animation (QUEST_PreloadAncientStatueCof). The NULL test
//  on pUnit cannot fire from OBJECT_ClientProcess, which has read the unit's class first (0x6FB48433); it is written as the bytes have it.
uint8_t __fastcall OBJECT_ProcessAncientsAltar(D2UnitStrc *pUnit)
{
    if (pUnit && pUnit->dwAnimMode)
        QUEST_PreloadAncientStatueCof();
    return 1;
}

//1.10f: D2Client.0x6FB48180
//  ClientFn 13, the invisible Ancient (row 561), the trigger of the Ancients' fight: while the object is still NEUTRAL, with a player whose quest
//  39 (Rite of Passage) has QFLAG_REWARDGRANTED and not QFLAG_ENTERAREA, and who is closer than 25 (jge, signed, 0x6FB481CF): the player's path is
//  stopped (D2Common #10153: dwPathPoints 0, flag 20h cleared), the player interacts with the object (command 19, UNITMODE_PerformClientCommand's
//  CLTACTION_INTERACT arm, on UNIT_OBJECT and its id), and the object goes to OPENED. It answers 1. The player is fetched twice, the second time
//  for the distance (0x6FB481C0). The command's three stack arguments are pushed at 0x6FB481E1..0x6FB481E4.
uint8_t __fastcall OBJECT_ProcessInvisibleAncient(D2UnitStrc *pUnit)
{
    if (!pUnit || !pUnit->dwAnimMode)
    {
        D2UnitStrc *pPlayer = CUNIT_GetPlayer();
        if (pPlayer && D2COMMON_QUESTRECORD_GetQuestState(NPCMENU_QuestGetQuestRecord(), 39, QFLAG_REWARDGRANTED) && !D2COMMON_QUESTRECORD_GetQuestState(NPCMENU_QuestGetQuestRecord(), 39, QFLAG_ENTERAREA)
            && D2COMMON_UNITS_GetDistanceToOtherUnit(pUnit, CUNIT_GetPlayer()) < 25)
        {
            if (pPlayer->pDynamicPath)
                D2COMMON_10153(pPlayer->pDynamicPath);
            UNITMODE_PerformClientCommand(19, pPlayer, UNIT_OBJECT, (int32_t)pUnit->dwUnitId, 0);
            D2COMMON_UNITS_ChangeAnimMode(pUnit, OBJMODE_OPENED);
            GFX_RebuildUnitAnimForMode(pUnit, 0);
        }
    }
    return 1;
}

//1.10f: D2Client.0x6FB48210
//  ClientFn 8, the Horadric orifice (row 152, where the staff is placed): OBJECT_ProcessAncientsAltar with Duriel's animation preloaded instead
//  (QUEST_PreloadDurielCof). Written out a second time because the image has two bodies.
uint8_t __fastcall OBJECT_ProcessHoradricOrifice(D2UnitStrc *pUnit)
{
    if (pUnit && pUnit->dwAnimMode)
        QUEST_PreloadDurielCof();
    return 1;
}

//1.10f: D2Client.0x6FB48230
//  ClientFn 14, the rogue bonfire (row 39): past dwTickCount (jbe, unsigned, 0x6FB48242) apply the day or night mode (OBJECT_ApplyDayNightMode,
//  the animation rebuilt) and wait 500 ms.
uint8_t __fastcall OBJECT_ProcessRogueBonfire(D2UnitStrc *pUnit)
{
    if (GetTickCount() > pUnit->dwTickCount)
    {
        OBJECT_ApplyDayNightMode(pUnit, 0);
        pUnit->dwTickCount = GetTickCount() + 500;
    }
    return 1;
}

//1.10f: D2Client.0x6FB48260
//  ClientFn 15, frozen Anya (row 558): every 500 ms, overlay 72 (OVERLAY_NPCALERT) is added while the object is NEUTRAL and removed while it is
//  anything else -- added again each time, not toggled.
uint8_t __fastcall OBJECT_ProcessFrozenAnya(D2UnitStrc *pUnit)
{
    if (GetTickCount() > pUnit->dwTickCount)
    {
        if (!pUnit->dwAnimMode)
        {
            GFX_AddOverlay(pUnit, OVERLAY_NPCALERT, OVERLAYTYPE_LOOP, 0, 0, 0, 0, 0);
            pUnit->dwTickCount = GetTickCount() + 500;
            return 1;
        }
        GFX_RemoveOverlay(pUnit, 0, OVERLAY_NPCALERT);
        pUnit->dwTickCount = GetTickCount() + 500;
    }
    return 1;
}

//1.10f: D2Client.0x6FB482C0
//  ClientFn 18, the keeper (row 568, "test data"): past dwTickCount, one time in ten -- a roll's low dword % 100 below 10, jnb, unsigned
//  (0x6FB482FA) -- it plays sound 2505 (Sounds.txt barbarian_grunt_small_1), and waits 0..59 seconds: the second roll's % 60 times 1000, as three
//  lea of five and one of eight (0x6FB48331..0x6FB4833B), added to a fresh GetTickCount taken after the roll. The original inlines
//  SEED_RollRandomNumber here.
uint8_t __fastcall OBJECT_ProcessKeeper(D2UnitStrc *pUnit)
{
    if (GetTickCount() > pUnit->dwTickCount)
    {
        if ((uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) % 100 < 10)
            SOUNDINST_PlaySound(SOUND_BARBARIAN_GRUNT_SMALL_1, pUnit, 0, 0, 0);
        uint32_t nDelay = (uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) % 60;
        pUnit->dwTickCount = GetTickCount() + 1000 * nDelay;
    }
    return 1;
}

//1.10f: D2Client.0x6FB48350
//  ClientFn 16, Baal's portal (rows 563 and 569): in OPERATING, once the counter equals mode 1's start frame shifted into the frame byte -- xor edx,
//  edx / mov dh, [ecx+12Ah], nStart[1] << 8 (0x6FB4835C, 0x6FB48363), the one body here that shifts it -- the object goes to OPENED, stored
//  straight into dwAnimMode, with the counter zeroed, the animation rebuilt and the start frame set. When mode 1's counter meets that value is
//  not established.
uint8_t __fastcall OBJECT_ProcessBaalPortal(D2UnitStrc *pUnit)
{
    if (pUnit->dwAnimMode == OBJMODE_OPERATING && pUnit->nSeqCurrentFramePrecise == (int32_t)pUnit->pObjectData->pObjectTxt->nStart[OBJMODE_OPERATING] << 8)
    {
        pUnit->dwAnimMode = OBJMODE_OPENED;
        pUnit->nSeqCurrentFramePrecise = 0;
        GFX_RebuildUnitAnimForMode(pUnit, 0);
        D2COMMON_UNITS_SetAnimStartFrame(pUnit);
    }
    return 1;
}

//1.10f: D2Client.0x6FB48390
//  ClientFn 17, the zoo (row 567, "test data"): while the zoo is armed and no chicken is left, spawn the chickens at the object's position
//  (QUEST_IsZooArmed, QUEST_ZooChickensPresent, QUEST_SpawnZooChickens). It answers 1. The original inlines UNITS_GetXPosition and
//  UNITS_GetYPosition here.
uint8_t __fastcall OBJECT_ProcessZoo(D2UnitStrc *pUnit)
{
    if (!QUEST_IsZooArmed() || QUEST_ZooChickensPresent())
        return 1;

    int32_t nY = UNITS_GetYPosition(pUnit);
    int32_t nX = UNITS_GetXPosition(pUnit);
    QUEST_SpawnZooChickens(nX, nY);
    return 1;
}


//1.10f: D2Client.0x6FB48430
//  Runs an object's client process function: its objects.txt ClientFn, asserted below 19 (cmp al, 13h / jb, unsigned, 0x6FB48442; Object.cpp:1346),
//  and for a nonzero one a tail jump through sgpfnProcessFunctions (0x6FB48470) with the unit in ecx, so the body's answer is this one's; ClientFn 0
//  answers 1. Callers: OBJECT_AdvanceAnim, which reads the answer, and CUNIT_UpdateAllUnits, three times, which does not.
//  Every object comes here once per update from CUNIT_UpdateAllUnits, after UNITMODE_UpdateUnit; one whose ClientFn is above 3 comes a second time,
//  earlier, from the end of OBJECT_AdvanceAnim. The ripple (ClientFn 2) therefore counts updates; the others keep time with GetTickCount.
//  A nonzero answer tells the caller to update the unit's mode sound, as OBJECT_AdvanceAnim reads it: the drinker, the gesturer and the turner do
//  that themselves and answer 0, the ripple with its sound does it and answers 1 all the same, and the rest answer 1. The answer is a byte
//  (OBJECT_AdvanceAnim tests al alone, 0x6FB46F97).
//  Each body is char __thiscall in the original and is written __fastcall: a function that takes only this has the same ABI. Their random numbers
//  are the original's inlined SEED_RollRandomNumber; the mask or remainder after each is the copy going on with the low dword, unsigned
//  (xor edx, edx / div), written around the call.
uint8_t __fastcall OBJECT_ClientProcess(D2UnitStrc *pUnit)
{
    uint8_t nClientFn = D2COMMON_DATATBLS_GetObjectsTxtRecord((int32_t)pUnit->dwClassId)->nClientFn;
    if (nClientFn >= 19)
    {
        FOG_DisplayAssert("ptTypeData->bClientProcessFunction < sizeof( sgpfnProcessFunctions ) / sizeof( ptClientObjectProcess )", szObjectCpp, 1346);
        exit(-1);
    }
    if (!nClientFn)
        return 1;
    return sgpfnProcessFunctions[nClientFn](pUnit);
}

//  The unit's last three functions, 0x6FB48480..0x6FB4856F: a portal's state from packet 0x60, and the owner name a town portal carries.

static_assert(offsetof(D2GSPacketSrv60, nFlags) == 1
                  && offsetof(D2GSPacketSrv60, nLevel) == 2
                  && offsetof(D2GSPacketSrv60, dwUnitGUID) == 3,
              "OBJECT_ApplyPortalState: `mov al, [edi+1]` (0x6FB4851E), "
              "`mov al, [edi+2]` (0x6FB484E9), `mov ecx, [edi+3]` (0x6FB484A7)");
static_assert(offsetof(D2ObjectDataStrc, szOwner) == 0x28
                  && sizeof(D2ObjectDataStrc::szOwner) == 16,
              "OBJECT_GetPortalOwnerName / SetPortalOwnerName: `add eax, 28h` "
              "(0x6FB48553, 0x6FB48565), `push 10h` (0x6FB48563)");

//1.10f: D2Client.0x6FB48480
//  Applies a portal's state, packet 0x60. SCMD_HandleSrv60_TownPortalState passes its stack copy, so the NULL assert (Object.cpp:1362) cannot fire
//  from there; it is written as the bytes have it. The portal is found by its GUID as UNIT_OBJECT, and a miss does nothing. A hit must be a town
//  portal or the permanent one (cmp eax, 3Bh, then cmp eax, 3Ch, 0x6FB484C1, 0x6FB484C6; the assert at Object.cpp:1372) and takes the packet's
//  level into InteractType, the byte OBJECT_InitClientUnit fills for the same two classes; then the packet's bit 1 and then bit 0 are ORed into
//  its portal flags through D2Common #10427 and #10428. This function sets bits and clears none, and takes no other bit of the packet's byte.
//  The level's assert, ptCmd->bLevelId <= 255 (Object.cpp:1374), is cmp al, 0FFh / jbe on a byte (0x6FB484EC) and cannot fail. It is written as
//  the source shape, and our compiler folds it: a difference in the bytes only, the arm being dead in both.
//  The flags byte lives in the slot push ecx opened (0x6FB48480), whose other three bytes are ptCmd's; mov edx, dword ptr [...] pushes the whole
//  dword (0x6FB48533, 0x6FB48537), and #10428 reads its low byte only (D2Common 0x6FDC07AC), so the upper bytes are inert.
void __fastcall OBJECT_ApplyPortalState(D2GSPacketSrv60 *ptCmd)
{
    if (!ptCmd)
    {
        FOG_DisplayAssert("ptCmd", szObjectCpp, 1362);
        exit(-1);
    }

    D2UnitStrc *pPortal = CUNIT_FindUnitByGuidAndType(ptCmd->dwUnitGUID, UNIT_OBJECT);
    if (!pPortal)
        return;

    const D2Objects nClassId = (D2Objects)pPortal->dwClassId;
    if (nClassId != OBJECT_TOWN_PORTAL && nClassId != OBJECT_PERMANENT_TOWN_PORTAL)
    {
        FOG_DisplayAssert("UnitGetClass(hSrcPortal) == OBJECTCLASS_TOWNPORTAL || UnitGetClass(hSrcPortal) == OBJECTCLASS_PORTALPERMANENT", szObjectCpp, 1372);
        exit(-1);
    }
    const uint8_t nLevel = ptCmd->nLevel;
    if (nLevel > 255)
    {
        FOG_DisplayAssert("ptCmd->bLevelId <= 255", szObjectCpp, 1374);
        exit(-1);
    }
    pPortal->pObjectData->InteractType = nLevel;

    uint8_t nPortalFlag = D2COMMON_UNITS_GetObjectPortalFlags(pPortal);
    const uint8_t nFlags = ptCmd->nFlags;
    if (nFlags & 2)
        nPortalFlag |= 2;
    if (nFlags & 1)
        nPortalFlag |= 1;
    D2COMMON_UNITS_SetObjectPortalFlags(pPortal, nPortalFlag);
}

//1.10f: D2Client.0x6FB48550
//  A town portal's owner name: the object data's szOwner (mov eax, [ecx+14h] / add eax, 28h), with no test of the unit or its type. One caller,
//  UIPANELS_DrawUnitNameLabel, at two call sites (0x6FB2130D for the strlen, 0x6FB21322 for the name).
char *__fastcall OBJECT_GetPortalOwnerName(D2UnitStrc *pObject)
{
    return pObject->pObjectData->szOwner;
}

//1.10f: D2Client.0x6FB48560
//  Sets it: STORM_SStrCopy into szOwner, bounded by 10h, the field's 16 bytes, and the copy's answer returned (call then retn, eax left as Storm's).
//  Two call sites, both in SCMD_HandleSrv82_PortalOwner, one for each portal GUID, and neither reads the answer, so the bytes cannot tell int from
//  void and the int stays.
int32_t __fastcall OBJECT_SetPortalOwnerName(D2UnitStrc *pObject, const char *pszName)
{
    return (int32_t)STORM_SStrCopy(pObject->pObjectData->szOwner, pszName, sizeof(pObject->pObjectData->szOwner));
}

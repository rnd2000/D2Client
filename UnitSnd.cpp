#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "UnitSnd.h"

#include <string.h>

//  UnitSnd.cpp -- the sounds of units in D2Client.dll 1.10: a unit's list of playing sound instances, the weapon sounds of a hit class, the floor
//  under a unit, footsteps, the sounds of object and monster modes, attacks, hits and deaths, the player's voice lines, the server's sound events
//  and the player's line on entering an area.
//  The original source file is UNIT\UnitSnd.cpp (every assert of the unit pushes the path). Address range 0x6FB4F4A0..0x6FB522E7.
//  UNITSND_GetSoundClassId, UNITSND_GetSoundUnitType and UNITSND_GetSoundListHead are small helpers the original also inlines in many of the
//  functions below.

static const char szUnitSndCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\UNIT\\UnitSnd.cpp";

static_assert(offsetof(D2HitClassSoundStrc, nHitFrame) == 4,
              "GetHitClassSoundDelay: `mov eax, (gHitClassSounds+4)[edx*8]` (0x6FB4F569)");
static_assert(NUM_EHITCLASSES == 14 && HITCLASS_HandToHand == 1 && HITCLASS_Claw == 12,
              "the hit class's bound, `cmp edx, 0Eh` (0x6FB4F557), and the weapon switch's 12 cases (`lea ecx, [ebp-1]` / "
              "`cmp ecx, 0Bh`, 0x6FB4F6DE)");
static_assert(HITCLASS_Layer_Double == 0x10 && HITCLASS_Layer_Fire == 0x20 && HITCLASS_Layer_Cold == 0x30
              && HITCLASS_Layer_Lightning == 0x40 && HITCLASS_Layer_Poison == 0x50 && HITCLASS_Layer_Stun == 0x60
              && HITCLASS_Layer_Bash == 0x70 && HITCLASS_Layer_Thorns == 0x80 && HITCLASS_Layer_Goo == 0xB0,
              "the layer switch's cases (`add eax, 0FFFFFFF0h` / `cmp eax, 0A0h`, 0x6FB4F71B)");
static_assert(SOUND_IMPACT_PUNCH_1 == 321 && SOUND_IMPACT_BLADE_SWING_1 == 341 && SOUND_IMPACT_BLADE_THRUST_1 == 335
              && SOUND_IMPACT_BLUNT_1 == 329 && SOUND_IMPACT_ARROW_1 == 347 && SOUND_IMPACT_CLAW_1 == 325,
              "the weapon impacts, `mov dword ptr [edi], 141h` .. `145h` (0x6FB4F6ED..0x6FB4F715)");
static_assert(SOUND_IMPACT_DOUBLE_1 == 379 && SOUND_IMPACT_FIRE_1 == 353 && SOUND_IMPACT_FIRE_LAYER_1 == 356
              && SOUND_IMPACT_COLD_1 == 359 && SOUND_IMPACT_COLD_LAYER_1 == 362 && SOUND_IMPACT_LIGHTNING_1 == 368
              && SOUND_IMPACT_LIGHTNING_LAYER_1 == 372 && SOUND_IMPACT_POISON_1 == 376 && SOUND_IMPACT_STUN_1 == 382
              && SOUND_IMPACT_BASH == 395 && SOUND_PALADIN_THORNS_HIT == 2390 && SOUND_IMPACT_GOO_1 == 391,
              "the layers, `mov dword ptr [edx], 17Bh` (0x6FB4F7E6), `add eax, 161h` (0x6FB4F744) .. `187h` (0x6FB4F823)");
static_assert(TILE_FLAGS_DIRT == 0x20 && TILE_FLAGS_ISTONE == 8 && TILE_FLAGS_OSTONE == 0x10 && TILE_FLAGS_SAND == 0x40
              && TILE_FLAGS_WOOD == 0x80 && TILE_FLAGS_SNOW == 0x400,
              "GetSurfaceMaterialUnderUnit: `test al, 20h` (0x6FB4FC6A), `test al, 8` (0x6FB4FC7B), `test al, 10h` "
              "(0x6FB4FC8C), `test al, 40h` (0x6FB4FC9D), `test al, al` / `jns` (0x6FB4FCAE), `test ah, 4` (0x6FB4FCBF)");
static_assert(offsetof(D2MonSoundsTxt, dwFootstep) == 0x54 && offsetof(D2MonSoundsTxt, dwFootstepLayer) == 0x58
              && offsetof(D2MonSoundsTxt, dwFsCnt) == 0x5C && offsetof(D2MonSoundsTxt, dwFsOff) == 0x60
              && offsetof(D2MonSoundsTxt, dwFsPrb) == 0x64,
              "GetFootstepSoundId `mov edi, [eax+54h]` (0x6FB4FDE4); PlayFootstep `mov esi, [eax+60h]` / `mov ebx, [eax+5Ch]` "
              "/ `mov edx, [eax+58h]` / `mov eax, [eax+64h]` (0x6FB5011A..0x6FB50123)");
static_assert(offsetof(D2MonStatsTxt, wUMonSound) == 0x16 && offsetof(D2SuperUniquesTxt, dwMonSound) == 0x18,
              "GetMonSoundsRecordForUnit `mov ax, [eax+16h]` (0x6FB4FF4E), `mov eax, [eax+18h]` (0x6FB4FEED)");
static_assert((decltype(((D2MonStatsTxt *)0)->wUMonSound))-1 < 0 && (decltype(((D2MonStatsTxt *)0)->wMonSound))-1 < 0
              && (decltype(((D2SuperUniquesTxt *)0)->dwMonSound))-1 < 0,
              "the three read signed: `test ax, ax` / `jle` and `movsx` (0x6FB4FF52..0x6FB4FF57), `movsx eax, word ptr "
              "[esi+14h]` (Monster.cpp, 0x6FB40A1B), `test eax, eax` / `jle` (0x6FB4FEF0..0x6FB4FEF2)");
static_assert(MONSTER_FOULCROW1 == 15 && MONMODE_NEUTRAL == 1,
              "PlayFootstep `cmp edx, 0Fh` (0x6FB500B8) and `cmp ecx, 1` (0x6FB500BD)");
static_assert(sizeof(((D2UnitStrc *)0)->bUnitSfx) == 1 && (decltype(((D2UnitStrc *)0)->dwSfxMode))-1 < 0,
              "bUnitSfx a byte: `mov [ecx+70h], dl` (0x6FB50962), `mov al, [esi+70h]` (0x6FB50501, 0x6FB50667); dwSfxMode "
              "read signed: `jg` (0x6FB50699)");
static_assert(offsetof(D2ClientObjectModeSoundStrc, nAltSound1) == 0x20 && offsetof(D2ClientObjectModeSoundStrc, nAltMode1) == 0x24
              && offsetof(D2ClientObjectModeSoundStrc, nAltSound2) == 0x28 && offsetof(D2ClientObjectModeSoundStrc, nAltMode2) == 0x2C,
              "UpdateUnitModeSound `mov ebp, [ecx+20h]` (0x6FB50549), `mov eax, [ecx+28h]` (0x6FB50550), `mov eax, [ecx+24h]` "
              "(0x6FB5055B), `mov eax, [ecx+2Ch]` (0x6FB50567)");
static_assert(OBJECT_CAINGIBBET == 26 && OBJECT_INVISIBLE_OBJECT == 61 && OBJECT_DUMMY_DOOR_BLOCKER_572 + 1 == 573
              && SOUND_CAIN_ACT1_HELP == 3671,
              "UpdateUnitModeSound `cmp [esp+28h+var_C], 1Ah` (0x6FB5047A), `cmp eax, 3Dh` (0x6FB5053B), the bound `cmp eax, "
              "23Dh` (0x6FB50518) one past D2Objects' last, `mov ecx, 0E57h` (0x6FB504CA)");
static_assert(offsetof(D2MonSoundsTxt, dwNeutral) == 0x68 && offsetof(D2MonSoundsTxt, dwNeuTime) == 0x6C,
              "UpdateUnitModeSound `mov ebp, [eax+68h]` (0x6FB50792), `mov ebx, [ebx+6Ch]` (0x6FB5079A)");
static_assert(offsetof(D2MonSoundsTxt, dwAttack1) == 0x04 && offsetof(D2MonSoundsTxt, dwAtt1Del) == 0x08
              && offsetof(D2MonSoundsTxt, dwAtt1Prb) == 0x0C && offsetof(D2MonSoundsTxt, dwWeapon1) == 0x10
              && offsetof(D2MonSoundsTxt, dwWea1Del) == 0x14 && offsetof(D2MonSoundsTxt, dwWea1Vol) == 0x18
              && offsetof(D2MonSoundsTxt, dwAttack2) == 0x1C && offsetof(D2MonSoundsTxt, dwAtt2Del) == 0x20
              && offsetof(D2MonSoundsTxt, dwAtt2Prb) == 0x24 && offsetof(D2MonSoundsTxt, dwWeapon2) == 0x28
              && offsetof(D2MonSoundsTxt, dwWea2Del) == 0x2C && offsetof(D2MonSoundsTxt, dwWea2Vol) == 0x30,
              "PlayAttackSound: Attack1 then Weapon1, not D2MOO's Attack1 then Attack2 (D2MOO_BUGS.md) -- `[ebx+4]` "
              "(0x6FB50B89), `[ebx+8]` (0x6FB50B8C), `[ebx+0Ch]` (0x6FB50BBC), `[ebx+10h]` (0x6FB50CCD), `[ebx+14h]` "
              "(0x6FB50CD0), `[ebx+18h]` (0x6FB50CEC), `[ebx+1Ch]` (0x6FB50B99), `[ebx+20h]` (0x6FB50B9C), `[ebx+24h]` "
              "(0x6FB50BC1), `[ebx+28h]` (0x6FB50CD5), `[ebx+2Ch]` (0x6FB50CD8), `[ebx+30h]` (0x6FB50CF1)");
static_assert(offsetof(D2MonSoundsTxt, dwHitSound) == 0x34 && offsetof(D2MonSoundsTxt, dwHitDelay) == 0x38
              && offsetof(D2MonSoundsTxt, dwDeathSound) == 0x3C && offsetof(D2MonSoundsTxt, dwDeaDelay) == 0x40
              && offsetof(D2MonSoundsTxt, dwInit) == 0x70 && offsetof(D2MonSoundsTxt, dwTaunt) == 0x74
              && offsetof(D2MonSoundsTxt, dwFlee) == 0x78,
              "PlayHitOrDeathSound `[edi+34h]` (0x6FB5128C), `[edi+38h]` (0x6FB5128F), `[edi+3Ch]` (0x6FB51275), "
              "`[edi+40h]` (0x6FB51278); StopMonsterVoices `[esi+70h]` (0x6FB51109), `[esi+74h]` (0x6FB5110E), "
              "`[esi+78h]` (0x6FB51113)");
static_assert(offsetof(D2MonSoundsTxt, nCvtMo1) == 0x7C && offsetof(D2MonSoundsTxt, nCvtTgt1) == 0x7D
              && offsetof(D2MonSoundsTxt, dwCvtSk1) == 0x80 && offsetof(D2MonSoundsTxt, nCvtMo2) == 0x84
              && offsetof(D2MonSoundsTxt, nCvtTgt2) == 0x85 && offsetof(D2MonSoundsTxt, dwCvtSk2) == 0x88
              && offsetof(D2MonSoundsTxt, nCvtMo3) == 0x8C && offsetof(D2MonSoundsTxt, nCvtTgt3) == 0x8D
              && offsetof(D2MonSoundsTxt, dwCvtSk3) == 0x90
              && (decltype(((D2MonSoundsTxt *)0)->dwCvtSk1))-1 < 0 && (decltype(((D2MonSoundsTxt *)0)->dwCvtSk2))-1 < 0
              && (decltype(((D2MonSoundsTxt *)0)->dwCvtSk3))-1 < 0,
              "PlayModeSound's Cvt walk: `add eax, 80h` (0x6FB50F4C), `mov dl, [eax-4]` (0x6FB50F58), `cmp dword ptr "
              "[eax], 0` / `jge` (0x6FB50F5F, 0x6FB50F62, signed), `mov dl, [eax-3]` (0x6FB50F66), `add eax, 8` (0x6FB50F6B)");
static_assert(SOUND_WEAPON_KICK_1 == 255 && SOUND_BLOCK_WEAPON_1 == 398 && SOUND_BLOCK_ARROW_1 == 401
              && HITCLASS_Staff == 9 && HITCLASS_Bow == 10 && HITCLASS_Crossbow == 11,
              "PlayModeSound `mov ecx, 0FFh` (0x6FB50F11), `mov ecx, 18Eh` (0x6FB50ED4) for the hit classes 1..9 and "
              "`mov ecx, 191h` (0x6FB50EDB) for 10..11 (the byte table 0x6FB50FF4)");
static_assert(SOUND_DIABLO_DEATH == 745 && SOUND_DIABLO_DEATH_SCHWING == 746 && SOUND_DIABLO_DEATH_HIT == 747
              && HITCLASS_Layer_SilentVoice == 0xA0 && SOUND_CREATURE_CHICKEN_1 == 2692,
              "PlayHitOrDeathSound `cmp ebx, 2E9h` (0x6FB512A4), `mov ecx, 2EAh` (0x6FB512B8), `mov ecx, 2EBh` "
              "(0x6FB512C2), `cmp dl, 0A0h` (0x6FB51248); PlayMonsterSkillSound `cmp ecx, 0A84h` (0x6FB51451)");
static_assert(offsetof(D2AreaEntryQuestLineStrc, nQuestId) == 0x28 && offsetof(D2AreaEntryQuestLineStrc, nSoundEvent) == 0x2C
              && sizeof(((D2AreaEntryQuestLineStrc *)0)->nSoundEvent) == 2
              && (decltype(((D2SkillsTxt *)0)->wStSound))-1 < 0 && STATE_EVADE == 68,
              "PlayAreaEntryQuestLine `mov ecx, [esi+28h]` (0x6FB5212A), `mov dx, [esi+2Ch]` (0x6FB52174); "
              "PlayUnitSoundEvent `test ax, ax` / `jle` on wStSound (0x6FB51A26), `push 44h` (0x6FB519BA)");
static_assert(SOUND_ITEM_PICKUP == 235 && SOUND_CURSOR_LEVEL_UP == 7 && SOUND_CURSOR_CONVERT_READY == 10
              && SOUND_CURSOR_CONVERT_ITEM == 11 && SOUND_CURSOR_IDENTIFY_READY == 12 && SOUND_CURSOR_IDENTIFY_ITEM == 13
              && SOUND_PLAYER_TOWNPORTAL_CAST == 2230 && SOUND_PLAYER_TOWNPORTAL_ENTER == 2231
              && SOUND_CURSOR_DURABILITY_BREAK == 9 && SOUND_ITEM_KEY_USED == 228,
              "PlayPlayerSoundEvent's fixed sounds, `mov esi, 0EBh` (0x6FB51820) .. `mov esi, 0E4h` (0x6FB51899)");
static_assert(SOUND_MUSIC_QUEST_ANDARIEL == 4685 && SOUND_MUSIC_QUEST_BLOODRAVEN == 4686 && SOUND_MUSIC_QUEST_DEN == 4688
              && SOUND_MUSIC_QUEST_MEPHISTO == 4692 && SOUND_MUSIC_QUEST_RADAMENT == 4693 && SOUND_MUSIC_QUEST_TAINTED == 4694
              && SOUND_MUSIC_QUEST_DIABLO == 4695 && SOUND_MUSIC_QUEST_BAAL == 4696 && SOUND_MUSIC_QUEST_NIHLATHAK == 4697
              && SOUND_MUSIC_QUEST_SHENK == 4698,
              "PlayPlayerSoundEvent's stings, `mov ecx, 1250h` (0x6FB5158D) .. `mov ecx, 1258h` (0x6FB5171D)");
static_assert(SOUND_SHRINE_REFILL == 2673 && SOUND_OBJECT_TRAP_TRIGGER == 2634 && SOUND_OBJECT_TRAP_RELEASE == 2635
              && SOUND_ROGUE_CONFIRM_A_1 == 4290 && SOUND_HIREABLE_FEMALE_THANK_YOU == 4615 && SOUND_HIREABLE_MALE_THANKS_1 == 4625
              && SOUND_HIREABLE_FEM_CANT_USE_THAT_EVER == 4612 && SOUND_HIRE_MALE_CANT_USE_THAT_EVER_1 == 4616
              && SOUND_HIREABLE_FEM_CANT_USE_THAT_YET == 4613 && SOUND_HIRE_MALE_CANT_USE_THAT_YET_1 == 4619
              && SOUND_HIREABLE_FEMALE_I_WILL_USE_THAT == 4614 && SOUND_HIREABLE_MALE_I_WILL_USE_THAT_1 == 4622
              && SOUND_ANCIENT_ACT5_REWARD_SPIEL == 4379 && SOUND_CURSOR_LEVEL_UP_HIRELING == 8 && SOUND_OBJECT_CORPSE_LOOT == 2553,
              "PlayUnitSoundEvent `mov ecx, 0A71h` (0x6FB51982) .. `mov ecx, 9F9h` (0x6FB519A8), the hireling pairs "
              "`mov ecx, 1207h` (0x6FB51A98) .. `mov ecx, 120Eh` (0x6FB51C2C)");


//1.10f: D2Client.0x6FB4F4A0
//  Clears the level sound state: the 1024 one-shot flags of gbLevelSoundPlayed and the five dwords after them are set to 0, except
//  gnNeutralSoundReArmDelay, which starts at 90, the upper bound of the roll that re-arms it (UNITSND_UpdateUnitModeSound waits that many frames
//  before a monster's next idle noise).
void __cdecl UNITSND_ResetLevelSoundState()
{
    memset(gbLevelSoundPlayed, 0, sizeof(gbLevelSoundPlayed));
    gnLastNeutralSoundFrame = 0;
    gnLastMonsterVoiceFrame = 0;
    gnNeutralSoundReArmDelay = 90;
    gnLastNagSoundFrame = 0;
    gnLastNagSoundId = 0;
}


//1.10f: D2Client.0x6FB4F4D0
//  Returns the weapon sound of a hit class, gHitClassSounds[nHitClass & 0xF].nSoundId (SOUND_NONE for the classes 0 and 10..13). The unit is only
//  asserted. The bound test is signed and its `>= 0` half is dead after the mask.
D2C_Sounds __fastcall UNITSND_GetHitClassSound(D2UnitStrc *pUnit, int32_t nHitClass)
{
    int32_t nHitClassBase;

    if (!pUnit)
    {
        FOG_DisplayAssert("ptUnit", szUnitSndCpp, 2908);
        exit(-1);
    }

    nHitClassBase = nHitClass & 0xF;
    if (!(nHitClassBase >= (int32_t)HITCLASS_None && nHitClassBase < (int32_t)NUM_EHITCLASSES))
    {
        FOG_DisplayAssert("eHitClassBase >= EHITCLASS_NONE && eHitClassBase < NUM_EHITCLASSES", szUnitSndCpp, 2912);
        exit(-1);
    }
    return gHitClassSounds[nHitClassBase].nSoundId;
}


//1.10f: D2Client.0x6FB4F530
//  Returns the delay of a hit class's sound: (nHitFrame * 256 + 128) / the unit's wAnimSpeed, unsigned, the speed sign-extended first and taken as
//  1 when it is 0. nHitFrame is 6 for the classes 1, 2, 3, 6, 8 and 9, 8 for 4, 5 and 7, 0 for the rest.
//  BUG: wAnimSpeed is a signed 16-bit member that is sign-extended and then divided unsigned, so a negative speed divides as 2^32 less its
//  magnitude and the delay is 0.
//  UNREACHABLE: a unit's animation speed is never negative.
uint32_t __fastcall UNITSND_GetHitClassSoundDelay(D2UnitStrc *pUnit, int32_t nHitClass)
{
    int32_t nHitClassBase;
    uint32_t nDivisor;

    if (!pUnit)
    {
        FOG_DisplayAssert("ptUnit", szUnitSndCpp, 2923);
        exit(-1);
    }

    nHitClassBase = nHitClass & 0xF;
    if (!(nHitClassBase >= (int32_t)HITCLASS_None && nHitClassBase < (int32_t)NUM_EHITCLASSES))
    {
        FOG_DisplayAssert("eHitClassBase >= EHITCLASS_NONE && eHitClassBase < NUM_EHITCLASSES", szUnitSndCpp, 2927);
        exit(-1);
    }

    nDivisor = (uint32_t)(int32_t)pUnit->wAnimSpeed;
    if (!nDivisor)
        nDivisor = 1;
    return (((uint32_t)gHitClassSounds[nHitClassBase].nHitFrame << 8) + 128) / nDivisor;
}


//1.10f: D2Client.0x6FB4F5A0
//  Fills *pnWeaponSound and *pnLayerSound (both 0 first) with the two impact sounds of a hit class. The low nibble picks the weapon's impact: punch
//  for HandToHand, blade_swing for the four swings, blade_thrust for the two thrusts, blunt for Club and Staff, arrow for Bow and Crossbow, claw
//  for Claw, none for None and Overlay.
//  The high nibble picks the layer: impact_double only over a weapon sound; fire and lightning their `_layer_1` over a weapon sound and their bare
//  `_1` without one; stun, bash, paladin_thorns_hit and goo always; Sanctuary and SilentVoice nothing. Cold and poison: without a weapon sound
//  their bare `_1` always; over one, only when the struck unit is not a player or monster or nMode is not 0. Poison's two arms name one row,
//  impact_poison_1, because sounds.txt has no layered poison.
//  A monster whose monstats2.txt row has the critter bit (MONSTATS2FLAGINDEX_CRITTER) returns with both sounds 0. nMode is
//  UNITSND_PlayHitOrDeathSound's own nMode, handed down unchanged and tested only against 0.
//  The original inlines UNITSND_GetSoundUnitType, UNITSND_GetSoundClassId and DATATBLS_GetMonStats2TxtRecord here.
void __fastcall UNITSND_GetImpactSoundsForHitClass(D2UnitStrc *pUnit, int32_t nHitClass, int32_t nMode, D2C_Sounds *pnWeaponSound, D2C_Sounds *pnLayerSound)
{
    int32_t nUnitType;
    int32_t nClass;
    D2MonStats2Txt *pMonStats2Txt;

    if (!pUnit)
    {
        FOG_DisplayAssert("ptUnit", szUnitSndCpp, 2946);
        exit(-1);
    }

    nUnitType = UNITSND_GetSoundUnitType(pUnit);
    *pnWeaponSound = SOUND_NONE;
    *pnLayerSound = SOUND_NONE;

    if (nUnitType == UNIT_MONSTER)
    {
        nClass = UNITSND_GetSoundClassId(pUnit);

        pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord(nClass);
        if (pMonStats2Txt && (pMonStats2Txt->nFlags[MONSTATS2FLAGINDEX_CRITTER >> 3] & gdwBitMasks[MONSTATS2FLAGINDEX_CRITTER & 7]))
            return;
    }

    switch (nHitClass & 0xF)
    {
    case HITCLASS_HandToHand:
        *pnWeaponSound = SOUND_IMPACT_PUNCH_1;
        break;
    case HITCLASS_OneHandSwingVsSmall:
    case HITCLASS_OneHandSwingVsLarge:
    case HITCLASS_TwoHandSwingVsSmall:
    case HITCLASS_TwoHandSwingVsLarge:
        *pnWeaponSound = SOUND_IMPACT_BLADE_SWING_1;
        break;
    case HITCLASS_OneHandThrust:
    case HITCLASS_TwoHandThrust:
        *pnWeaponSound = SOUND_IMPACT_BLADE_THRUST_1;
        break;
    case HITCLASS_Club:
    case HITCLASS_Staff:
        *pnWeaponSound = SOUND_IMPACT_BLUNT_1;
        break;
    case HITCLASS_Bow:
    case HITCLASS_Crossbow:
        *pnWeaponSound = SOUND_IMPACT_ARROW_1;
        break;
    case HITCLASS_Claw:
        *pnWeaponSound = SOUND_IMPACT_CLAW_1;
        break;
    default:
        break;
    }

    switch (nHitClass & 0xF0)
    {
    case HITCLASS_Layer_Double:
        if (*pnWeaponSound)
            *pnLayerSound = SOUND_IMPACT_DOUBLE_1;
        break;
    case HITCLASS_Layer_Fire:
        *pnLayerSound = *pnWeaponSound ? SOUND_IMPACT_FIRE_LAYER_1 : SOUND_IMPACT_FIRE_1;
        break;
    case HITCLASS_Layer_Cold:
        if (!*pnWeaponSound)
            *pnLayerSound = SOUND_IMPACT_COLD_1;
        else if ((nUnitType != UNIT_PLAYER && nUnitType != UNIT_MONSTER) || nMode)
            *pnLayerSound = SOUND_IMPACT_COLD_LAYER_1;
        break;
    case HITCLASS_Layer_Lightning:
        *pnLayerSound = *pnWeaponSound ? SOUND_IMPACT_LIGHTNING_LAYER_1 : SOUND_IMPACT_LIGHTNING_1;
        break;
    case HITCLASS_Layer_Poison:
        if (!*pnWeaponSound)
            *pnLayerSound = SOUND_IMPACT_POISON_1;
        else if ((nUnitType != UNIT_PLAYER && nUnitType != UNIT_MONSTER) || nMode)
            *pnLayerSound = SOUND_IMPACT_POISON_1;
        break;
    case HITCLASS_Layer_Stun:
        *pnLayerSound = SOUND_IMPACT_STUN_1;
        break;
    case HITCLASS_Layer_Bash:
        *pnLayerSound = SOUND_IMPACT_BASH;
        break;
    case HITCLASS_Layer_Thorns:
        *pnLayerSound = SOUND_PALADIN_THORNS_HIT;
        break;
    case HITCLASS_Layer_Goo:
        *pnLayerSound = SOUND_IMPACT_GOO_1;
        break;
    default:
        break;
    }
}


//1.10f: D2Client.0x6FB4F930
//  Pushes a sound instance's handle on the unit's own sound list (pUnitSfxData) through D2Sound #10058, a tail call that hands edx on untouched.
//  The original inlines UNITSND_GetSoundListHead here.
void __fastcall UNITSND_LinkSoundToUnit(D2UnitStrc *pUnit, int32_t hInstance)
{
    D2SOUND_10058_ListPushFront(UNITSND_GetSoundListHead(pUnit), hInstance);
}


//1.10f: D2Client.0x6FB4F960
//  Removes a sound instance from the unit's sound list through D2Sound #10059, the same tail call. The original inlines UNITSND_GetSoundListHead
//  here.
void __fastcall UNITSND_UnlinkSoundFromUnit(D2UnitStrc *pUnit, int32_t hInstance)
{
    D2SOUND_10059_ListRemove(UNITSND_GetSoundListHead(pUnit), hInstance);
}


//1.10f: D2Client.0x6FB4F990
//  Returns the handle of the first instance on the unit's sound list whose sound id lies in nSoundId's group, from the group's first id
//  (SOUNDHDR_GetGroupBaseIndex) for the group's size (SOUNDHDR_GetGroupSize, a byte); 0 when none, or when the sound system is not up.
//  Both bounds are signed compares, so the group's first id is held as an int and cast to D2C_Sounds for GetGroupSize.
int32_t __fastcall UNITSND_FindPlayingSoundInGroup(D2UnitStrc *pUnit, D2C_Sounds nSoundId)
{
    D2C_Sounds nGroupBase;
    int32_t nGroupCount;
    D2SoundListNodeStrc *pNode;
    int32_t hInstance;
    int32_t nPlayingSoundId;

    if (!pUnit)
    {
        FOG_DisplayAssert("ptUnit", szUnitSndCpp, 3143);
        exit(-1);
    }

    if (!SOUNDCHAN_IsSystemReady_2())
        return 0;

    nGroupBase = SOUNDHDR_GetGroupBaseIndex(nSoundId);
    nGroupCount = SOUNDHDR_GetGroupSize(nGroupBase);
    for (pNode = pUnit->pUnitSfxData; pNode; pNode = pNode->pNext)
    {
        hInstance = pNode->nValue;
        nPlayingSoundId = SOUNDINST_GetSoundIdFromHandle(hInstance);
        if (nPlayingSoundId >= nGroupBase && nPlayingSoundId < nGroupBase + nGroupCount)
            return hInstance;
    }
    return 0;
}


//1.10f: D2Client.0x6FB4FA10
//  Returns the handle at the head of the unit's sound list, 0 for an empty list.
int32_t __fastcall UNITSND_GetFirstSoundOnUnit(D2UnitStrc *pUnit)
{
    D2SoundListNodeStrc *pNode;

    if (!pUnit)
    {
        FOG_DisplayAssert("ptUnit", szUnitSndCpp, 3171);
        exit(-1);
    }

    pNode = pUnit->pUnitSfxData;
    if (pNode)
        return pNode->nValue;
    return 0;
}


//1.10f: D2Client.0x6FB4FA40
//  Detaches every sound instance on the unit's list with SOUNDINST_DetachFromUnit (passing bForce); the next node is read first because the call
//  unlinks the one it is given. The original inlines UNITSND_GetSoundListHead here.
void __fastcall UNITSND_StopAllSoundsOnUnit(D2UnitStrc *pUnit, int32_t bForce)
{
    D2SoundListNodeStrc *pNode;
    D2SoundListNodeStrc *pNext;

    for (pNode = *UNITSND_GetSoundListHead(pUnit); pNode; pNode = pNext)
    {
        pNext = pNode->pNext;
        SOUNDINST_DetachFromUnit(pNode->nValue, pUnit, bForce);
    }
}


//1.10f: D2Client.0x6FB4FA90
//  Returns the floor under a unit as a footstep material: 1 dirt, 2 indoor stone, 3 outdoor stone, 4 sand, 5 snow, 6 wood. The default is the
//  current sound environment's Material 1 (SOUNDHDR_GetEnvRecord) when it is 1..6, else 1.
//  Then the unit's subtile, made a tile of its room ((subtile - the room's) / 5, signed), is looked for in the room's floor tiles (tile counts
//  compared unsigned) and, if it is not there, in each adjacent room's (room counts signed). The tile's library entry decides by its material
//  flags, in this order: DIRT, ISTONE, OSTONE, SAND, WOOD, SNOW; a tile with no entry, no material flag, or none found keeps the default.
//  BUG: a unit with no room answers 0 (no material), not the default, because D2COMMON_UNITS_GetRoom's NULL is returned as it is;
//  UNITSND_GetFootstepSoundId then keeps the dirt take.
//  NOT ESTABLISHED: whether a footstep sound is ever asked for a unit with no room.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall UNITSND_GetSurfaceMaterialUnderUnit(D2UnitStrc *pUnit)
{
    D2SoundEnvironTxt *pEnvRecord;
    int32_t nDefaultMaterial;
    int32_t nSubtileX;
    int32_t nSubtileY;
    D2ActiveRoomStrc *pRoom;
    D2DrlgCoordsStrc tCoords;
    D2DrlgTileDataStrc *pFloorTiles;
    D2DrlgTileDataStrc *pTileData;
    D2ActiveRoomStrc **ppAdjRooms;
    D2TileLibraryEntryStrc *pTileEntry;
    int32_t nFloorCount;
    int32_t nAdjRoomCount;
    int32_t nTileX;
    int32_t nTileY;
    int32_t iAdjRoom;
    uint32_t iTile;
    uint16_t wMaterialFlags;

    pEnvRecord = SOUNDHDR_GetEnvRecord(SOUNDHDR_GetCurrentSoundEnvironId());
    nDefaultMaterial = 1;
    if (pEnvRecord && pEnvRecord->nMaterial1 > 0 && pEnvRecord->nMaterial1 < 7)
        nDefaultMaterial = pEnvRecord->nMaterial1;

    nSubtileX = UNITS_GetXPosition(pUnit);
    nSubtileY = UNITS_GetYPosition(pUnit);

    pRoom = D2COMMON_UNITS_GetRoom(pUnit);
    if (!pRoom)
        return 0;

    D2COMMON_DUNGEON_GetRoomCoordinates(pRoom, &tCoords);
    nTileX = (nSubtileX - tCoords.nSubtileX) / 5;
    nTileY = (nSubtileY - tCoords.nSubtileY) / 5;
    pFloorTiles = D2COMMON_DUNGEON_GetFloorTilesFromRoom(pRoom, &nFloorCount);
    pTileData = NULL;
    for (iTile = 0; iTile < (uint32_t)nFloorCount; ++iTile)
    {
        if (pFloorTiles[iTile].nPosX == nTileX && pFloorTiles[iTile].nPosY == nTileY)
        {
            pTileData = &pFloorTiles[iTile];
            break;
        }
    }

    if (!pTileData)
    {
        D2COMMON_DUNGEON_GetAdjacentRoomsListFromRoom(pRoom, &ppAdjRooms, &nAdjRoomCount);
        for (iAdjRoom = 0; !pTileData && iAdjRoom < nAdjRoomCount; ++iAdjRoom)
        {
            pRoom = ppAdjRooms[iAdjRoom];
            D2COMMON_DUNGEON_GetRoomCoordinates(pRoom, &tCoords);
            nTileX = (nSubtileX - tCoords.nSubtileX) / 5;
            nTileY = (nSubtileY - tCoords.nSubtileY) / 5;
            pFloorTiles = D2COMMON_DUNGEON_GetFloorTilesFromRoom(pRoom, &nFloorCount);
            for (iTile = 0; iTile < (uint32_t)nFloorCount; ++iTile)
            {
                if (pFloorTiles[iTile].nPosX == nTileX && pFloorTiles[iTile].nPosY == nTileY)
                {
                    pTileData = &pFloorTiles[iTile];
                    break;
                }
            }
        }
        if (!pTileData)
            return nDefaultMaterial;
    }

    pTileEntry = pTileData->pTile;
    if (!pTileEntry)
        return nDefaultMaterial;

    wMaterialFlags = pTileEntry->nFlags;
    if (wMaterialFlags & TILE_FLAGS_DIRT)
        return 1;
    if (wMaterialFlags & TILE_FLAGS_ISTONE)
        return 2;
    if (wMaterialFlags & TILE_FLAGS_OSTONE)
        return 3;
    if (wMaterialFlags & TILE_FLAGS_SAND)
        return 4;
    if (wMaterialFlags & TILE_FLAGS_WOOD)
        return 6;
    if (wMaterialFlags & TILE_FLAGS_SNOW)
        return 5;
    return nDefaultMaterial;
}


//1.10f: D2Client.0x6FB4FCE0
//  Returns the footstep sound of a unit: the walk sound of its class or of its monsounds.txt row, moved to the take of the floor under it and to
//  the run take when bRunning. A player's is gpPlayerClassSounds[class][10]: light_walk_dirt_1 (2720) for the sorceress, necromancer, druid and
//  assassin, medium_walk_dirt_1 (2768) for the amazon and paladin, heavy_walk_dirt_1 (2816) for the barbarian. A monster's is its row's dwFootstep.
//  Any other type, a NULL record or a 0 id answers 0; the foul crow's and the vulture's rows have no Footstep, their step being their layer
//  (UNITSND_PlayFootstep).
//  The floor adds 4 per material past dirt, the four takes of each group, for the materials 2..6; material 1, and the 0 that
//  UNITSND_GetSurfaceMaterialUnderUnit answers for a unit with no room, keep the dirt take. Running adds 24 (light_walk_dirt_1 to light_run_dirt_1,
//  2720 to 2744).
//  The class bound "eUnitClass < NUM_PLAYERCLASS" is an assert of its own, compared unsigned. The id is built by arithmetic on the base take, so
//  the local stays int32_t and the one cast to D2C_Sounds is at the return.
//  The original inlines UNITSND_GetSoundUnitType, UNITSND_GetSoundClassId and that player class bound assert here.
D2C_Sounds __fastcall UNITSND_GetFootstepSoundId(D2UnitStrc *pUnit, int32_t bRunning)
{
    int32_t nType;
    int32_t nClass;
    int32_t nSoundId;
    D2MonSoundsTxt *pMonSoundsTxt;

    nType = UNITSND_GetSoundUnitType(pUnit);
    if (nType == UNIT_PLAYER)
    {
        nClass = UNITSND_GetSoundClassId(pUnit);
        if ((uint32_t)nClass >= NUMBER_OF_PLAYERCLASSES)
        {
            FOG_DisplayAssert("eUnitClass < NUM_PLAYERCLASS", szUnitSndCpp, 2850);
            exit(-1);
        }
        nSoundId = gpPlayerClassSounds[nClass][10];
    }
    else if (nType == UNIT_MONSTER)
    {
        pMonSoundsTxt = UNITSND_GetMonSoundsRecordForUnit(pUnit);
        if (!pMonSoundsTxt)
            return SOUND_NONE;
        nSoundId = pMonSoundsTxt->dwFootstep;
    }
    else
        return SOUND_NONE;

    if (!nSoundId)
        return SOUND_NONE;

    switch (UNITSND_GetSurfaceMaterialUnderUnit(pUnit))
    {
    case 2:
        nSoundId += 4;
        break;
    case 3:
        nSoundId += 8;
        break;
    case 4:
        nSoundId += 12;
        break;
    case 5:
        nSoundId += 16;
        break;
    case 6:
        nSoundId += 20;
        break;
    default:
        break;
    }
    if (bRunning)
        nSoundId += 24;
    return (D2C_Sounds)nSoundId;
}


//1.10f: D2Client.0x6FB4FE50
//  Returns the monsounds.txt record of a unit's voice. Record 0 for a NULL unit and for a class past monstats.txt's count (signed compare); for a
//  unit that is not a monster (dwUnitType read raw, not the type ConvertMode makes) its class's own row (D2Common's FromMonsterId). For a monster:
//  a superunique's superuniques.txt MonSound when it is above 0; then, for a unique or a minion (MONSTER_IsUnique, MONSTER_IsMinion), its
//  monstats.txt UMonSound when it is above 0; otherwise, and for a unique with no UMonSound, the class's own MonSound row.
//  The three sound fields are read signed. The answer can be NULL (FromMonsterId answers NULL for a class whose MonSound is past monsounds.txt),
//  and the callers test it.
//  The original inlines UNITSND_GetSoundClassId and DATATBLS_GetMonStatsTxtRecord here.
D2MonSoundsTxt *__fastcall UNITSND_GetMonSoundsRecordForUnit(D2UnitStrc *pUnit)
{
    int32_t nClass;
    D2SuperUniquesTxt *pSuperUniquesTxt;
    D2MonStatsTxt *pMonStatsTxt;

    if (!pUnit)
        return D2COMMON_DATATBLS_GetMonSoundsTxtRecordFromSoundId(0);

    nClass = UNITSND_GetSoundClassId(pUnit);
    if (nClass >= sgptDataTables->nMonStatsTxtRecordCount)
        return D2COMMON_DATATBLS_GetMonSoundsTxtRecordFromSoundId(0);
    if (pUnit->dwUnitType != UNIT_MONSTER)
        return D2COMMON_DATATBLS_GetMonSoundsTxtRecordFromMonsterId(nClass);

    if (MONSTER_IsSuperUnique(pUnit))
    {
        pSuperUniquesTxt = D2COMMON_DATATBLS_GetSuperUniquesTxtRecord(MONSTER_GetBossHcIdx(pUnit));
        if (pSuperUniquesTxt && pSuperUniquesTxt->dwMonSound > 0)
            return D2COMMON_DATATBLS_GetMonSoundsTxtRecordFromSoundId(pSuperUniquesTxt->dwMonSound);
    }

    if (!MONSTER_IsUnique(pUnit) && !MONSTER_IsMinion(pUnit))
        return D2COMMON_DATATBLS_GetMonSoundsTxtRecordFromMonsterId(nClass);

    pMonStatsTxt = DATATBLS_GetMonStatsTxtRecord(nClass);
    if (!pMonStatsTxt)
        return D2COMMON_DATATBLS_GetMonSoundsTxtRecordFromSoundId(0);
    if (pMonStatsTxt->wUMonSound > 0)
        return D2COMMON_DATATBLS_GetMonSoundsTxtRecordFromSoundId(pMonStatsTxt->wUMonSound);
    return D2COMMON_DATATBLS_GetMonSoundsTxtRecordFromMonsterId(nClass);
}


//1.10f: D2Client.0x6FB4FF80
//  Plays a footstep when this frame is one. A monster steps only in a walking mode (MONMODE_WALK or MONMODE_RUN), but the foul crow (class 15) also
//  at MONMODE_NEUTRAL and the vulture (110) only at MONMODE_SKILL1: the two birds, whose monsounds.txt rows have no Footstep and a wing beat for
//  the layer (hawk_wing_1, vulturedemon_wing_1). The class is held -1 outside monstats.txt's count first. A player steps in any mode its callers
//  call it for (the moving modes), and a unit with no animation speed never.
//  The step: the unit's animation cycle (dwFrameCountPrecise, 1/256 frames) is split into FsCnt steps (2 for a player), and a step plays on the
//  frame whose ring distance to the phase FsOff << 8 is less than both its neighbours' one speed step ahead and behind
//  (SOUNDSONG_GetCircularDistance, modulus cycle / FsCnt). With FsCnt above 1 it waits two thirds of a step's frames since the last
//  (dwSfxStepTicks).
//  The divisions, the FsCnt test and the frame compares are unsigned; the ring distances and the class bound are signed. The sound is
//  UNITSND_GetFootstepSoundId's, running in PLRMODE_RUN or MONMODE_RUN, at 255 for the player and 200 for anyone else, and 160/255 of that (a
//  signed /255) when the step comes more than one and a half step periods late. Then the layer, FsPrb percent of the time
//  (SOUNDSONG_RollRandom(100) below it, unsigned); once the step has fired, the frame is stored as the last step's whether or not a sound played.
//  BUG: the wing-beat tests name the classes 15 and 110 alone, though the stock monstats.txt gives the foulcrow row's MonSound and UMonSound to
//  eight more classes (foulcrow2..4 16..18, the eagle 429, foulcrow5..8 590..593) and the vulture row's to four (vulture2..4 111..113, vulture5
//  608), so those twelve beat their wings only walking or running.
//  REACHABLE: the stock monstats.txt has those classes.
//  The original inlines UNITSND_GetSoundUnitType, UNITSND_GetSoundClassId and the sound code's mode read (the type, class and mode put through
//  ConvertMode for a player, the mode kept) here.
void __fastcall UNITSND_PlayFootstep(D2UnitStrc *pUnit)
{
    int32_t nType;
    int32_t nModeType;
    int32_t nModeClass;
    int32_t nMode;
    int32_t nClass;
    int32_t nMonClass;
    BOOL bModeHasFootsteps;
    int32_t nAnimSpeed;
    int32_t nFrame;
    uint32_t nCycle;
    uint32_t nPhase;
    uint32_t nStepsPerCycle;
    D2C_Sounds nLayerSoundId;
    uint32_t nLayerProbability;
    D2MonSoundsTxt *pMonSoundsTxt;
    int32_t nNow;
    uint32_t nStepPeriod;
    uint32_t nFramesSinceLastStep;
    uint32_t nStepLength;
    int32_t nDistance;
    int32_t nDistanceAhead;
    int32_t nDistanceBack;
    int32_t bRunning;
    int32_t hInstance;
    int32_t nVolume;

    nType = UNITSND_GetSoundUnitType(pUnit);

    nModeType = pUnit ? (int32_t)pUnit->dwUnitType : (int32_t)UNIT_TYPES_COUNT;
    nModeClass = pUnit ? (int32_t)pUnit->dwClassId : -1;
    nMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
    if (!nModeType)
        D2COMMON_11013_ConvertMode(pUnit, &nModeType, &nModeClass, &nMode, szUnitSndCpp, 2841);

    if (nType == UNIT_MONSTER)
    {
        nClass = UNITSND_GetSoundClassId(pUnit);
        nMonClass = (nClass >= 0 && nClass < sgptDataTables->nMonStatsTxtRecordCount) ? nClass : -1;
        bModeHasFootsteps = nMode == MONMODE_WALK || nMode == MONMODE_RUN;
        if (nMonClass != MONSTER_FOULCROW1 || nMode != MONMODE_NEUTRAL)
        {
            if (nMonClass == MONSTER_VULTURE1)
                bModeHasFootsteps = nMode == MONMODE_SKILL1;
            if (!bModeHasFootsteps)
                return;
        }
    }

    nAnimSpeed = pUnit->wAnimSpeed;
    nFrame = pUnit->nSeqCurrentFramePrecise;
    nCycle = pUnit->dwFrameCountPrecise;
    if (!nAnimSpeed)
        return;

    nPhase = 0;
    nStepsPerCycle = 2;
    nLayerSoundId = SOUND_NONE;
    nLayerProbability = 100;
    if (nType == UNIT_MONSTER)
    {
        pMonSoundsTxt = UNITSND_GetMonSoundsRecordForUnit(pUnit);
        if (pMonSoundsTxt)
        {
            nPhase = pMonSoundsTxt->dwFsOff << 8;
            nStepsPerCycle = pMonSoundsTxt->dwFsCnt;
            nLayerSoundId = pMonSoundsTxt->dwFootstepLayer;
            nLayerProbability = pMonSoundsTxt->dwFsPrb;
            if (!nStepsPerCycle)
                return;
        }
    }

    nNow = GAME_GetFrameCount();
    nStepPeriod = nCycle / (nStepsPerCycle * nAnimSpeed);
    nFramesSinceLastStep = nNow - pUnit->dwSfxStepTicks;
    if (nStepsPerCycle > 1 && nFramesSinceLastStep < 2 * nStepPeriod / 3)
        return;

    nStepLength = nCycle / nStepsPerCycle;
    nDistance = SOUNDSONG_GetCircularDistance(nFrame, nPhase, nStepLength);
    nDistanceAhead = SOUNDSONG_GetCircularDistance(nFrame + nAnimSpeed, nPhase, nStepLength);
    nDistanceBack = SOUNDSONG_GetCircularDistance(nFrame - nAnimSpeed, nPhase, nStepLength);
    if (nDistance >= nDistanceAhead || nDistance >= nDistanceBack)
        return;

    bRunning = 0;
    if (nType == UNIT_PLAYER)
    {
        if (nMode == PLRMODE_RUN)
            bRunning = 1;
    }
    else if (nType == UNIT_MONSTER)
    {
        if (nMode == MONMODE_RUN)
            bRunning = 1;
    }

    hInstance = SOUNDINST_PlaySound(UNITSND_GetFootstepSoundId(pUnit, bRunning), pUnit, 0, 0, 0);
    if (hInstance)
    {
        nVolume = 255;
        if (pUnit != CUNIT_GetPlayer())
            nVolume = 200;
        if (nFramesSinceLastStep > (3 * nStepPeriod) >> 1)
            nVolume = 160 * nVolume / 255;
        SOUNDINST_SetVolumeByHandle(hInstance, nVolume);
    }

    if (nLayerSoundId && SOUNDSONG_RollRandom(100) < nLayerProbability)
        SOUNDINST_PlaySound(nLayerSoundId, pUnit, 0, 0, 0);

    pUnit->dwSfxStepTicks = nNow;
}


//1.10f: D2Client.0x6FB50290
//  Stops a monster's skill sounds: every instance on its list whose sound's group is one of its monsounds.txt row's four Skill sounds
//  (dwSkill[0..3], compared for equality) is detached with bForce 1, the next node read first. Only for a monster (the type after ConvertMode) with
//  a record.
//  The original inlines UNITSND_GetSoundUnitType and UNITSND_GetSoundListHead here.
void __fastcall UNITSND_StopMonsterSkillSounds(D2UnitStrc *pUnit)
{
    D2MonSoundsTxt *pMonSoundsTxt;
    D2SoundListNodeStrc *pNode;
    D2SoundListNodeStrc *pNext;
    int32_t hInstance;
    D2C_Sounds nGroupBase;

    if (UNITSND_GetSoundUnitType(pUnit) != UNIT_MONSTER)
        return;

    pMonSoundsTxt = UNITSND_GetMonSoundsRecordForUnit(pUnit);
    if (!pMonSoundsTxt)
        return;

    for (pNode = *UNITSND_GetSoundListHead(pUnit); pNode; pNode = pNext)
    {
        hInstance = pNode->nValue;
        pNext = pNode->pNext;
        nGroupBase = SOUNDHDR_GetGroupBaseIndex(SOUNDINST_GetSoundIdFromHandle(hInstance));
        if (nGroupBase == pMonSoundsTxt->dwSkill[0] || nGroupBase == pMonSoundsTxt->dwSkill[1] || nGroupBase == pMonSoundsTxt->dwSkill[2] || nGroupBase == pMonSoundsTxt->dwSkill[3])
            SOUNDINST_DetachFromUnit(hInstance, pUnit, TRUE);
    }
}


//1.10f: D2Client.0x6FB50370
//  One frame of a unit's mode sound: an object's sound for its mode, or a monster's idle voice. The mode is read the sound code's way (type, class
//  and mode put through ConvertMode for a player, the mode kept).
//  An object: the cain gibbet (OBJECT_CAINGIBBET) in OBJMODE_NEUTRAL plays cain_act1_help once, while the unit's dwSfxAsyncTicks is still 0 and the
//  player is nearer than 20 (signed compare), stamping the frame into that member. Then, only when the mode changed since the last pass (bUnitSfx
//  still 0, or dwSfxMode not the mode), the class and the mode are asserted and the class's record in gpObjectModeSounds is read:
//    * the invisible object (OBJECT_INVISIBLE_OBJECT) takes the sound for the mode from gObjectModeSoundTable[21]: cairn_stone_1..5 for modes 1..5
//    (413..417), the Cairn Stones' tones;
//    * any other takes its record's first alternate sound when the first alternate mode is the mode or 8, else the second sound when the second
//    mode is; 8 is one past the modes, any mode. A record with neither alternate sound, or neither mode matching, gives none.
//  A sound found plays unless an instance of its group already plays on the unit, and at once when the sound system is down. No sound, or a class
//  with no record: every looping sound on the unit is detached (SOUNDHDR_IsLooping, bForce 0), which is how a loop stops when an object leaves its
//  mode.
//  Then the mode's own sound, nModeSounds[mode] of the class's record, plays on every change of mode after the first (bUnitSfx set), and on the
//  first too for a town portal (OBJECT_TOWN_PORTAL) -- but for a well (gObjectModeSoundTable[106], the record of the thirteen well classes of
//  objects.txt) not when the mode went down (the old mode is compared signed). Last bUnitSfx is set and the mode stored.
//  A monster: its idle voice, monsounds.txt's Neutral, in MONMODE_NEUTRAL, WALK and RUN -- and in SKILL1 for a vulture and SKILL4 for a bat demon,
//  by the base monster (D2CLIENT_MONSTER_GetBaseMonsterId, clamped to -1 outside monstats.txt), where UNITSND_PlayFootstep tests the class. It
//  plays when no instance of its group plays on the unit, gnNeutralSoundReArmDelay frames have passed since any monster's last, NeuTime frames
//  since the unit's dwSfxTicks and since its dwSfxAsyncTicks (the three compares unsigned), the unit is not in a town room, it is in earshot and
//  SOUNDSONG_RollRandom(15) rolls 0; then the frame is stamped globally and on the unit and the delay is re-rolled to 30..90. The record is fetched
//  twice, the second time for Neutral with a NULL test of its own, as in the original.
//  The two asserts' text is one string, "eUnitClass < NUM_OBJECTS && nMode < NUM_OBJECTMODES"; the second tests the class alone. NUM_OBJECTS is 573
//  (0x23D), objects.txt's rows with an Id.
//  The original inlines UNITSND_GetSoundUnitType, UNITSND_GetSoundClassId, UNITSND_GetSfxAsyncTicks, the sound code's mode read and the read of a
//  unit's bUnitSfx byte here.
void __fastcall UNITSND_UpdateUnitModeSound(D2UnitStrc *pUnit)
{
    int32_t nSoundType;
    int32_t nSoundClass;
    int32_t nUnitMode;
    uint32_t nObjectClass;
    D2ClientObjectModeSoundStrc *pModeSounds;
    D2C_Sounds nModeSound;
    BOOL bGroupPlaying;
    D2C_Sounds nGroupBase;
    int32_t nGroupSize;
    D2SoundListNodeStrc *pNode;
    int32_t hInstance;
    int32_t nPlayingSoundId;
    D2SoundListNodeStrc *pLoopNode;
    D2SoundListNodeStrc *pNext;
    int32_t hLoopInstance;
    BOOL bTownPortal;
    int32_t nOldSfxMode;
    D2ClientObjectModeSoundStrc *pObjectModeSound;
    D2C_Sounds nModeSoundId;
    int32_t nBaseMonsterId;
    BOOL bVoiceMode;
    D2MonSoundsTxt *pMonSoundsTxt;
    D2MonSoundsTxt *pMonSoundsTxt2;
    D2C_Sounds nNeutralSound;
    uint32_t nFrame;
    uint32_t nNeuTime;
    D2ActiveRoomStrc *pRoom;

    nSoundType = pUnit ? (int32_t)pUnit->dwUnitType : (int32_t)UNIT_TYPES_COUNT;
    nSoundClass = pUnit ? (int32_t)pUnit->dwClassId : -1;
    nUnitMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
    if (!nSoundType)
        D2COMMON_11013_ConvertMode(pUnit, &nSoundType, &nSoundClass, &nUnitMode, szUnitSndCpp, 2841);

    if (UNITSND_GetSoundUnitType(pUnit) != UNIT_OBJECT)
    {
        if (UNITSND_GetSoundUnitType(pUnit) != UNIT_MONSTER)
            return;

        nBaseMonsterId = MONSTER_GetBaseMonsterId(pUnit);
        if (nBaseMonsterId < 0 || nBaseMonsterId >= sgptDataTables->nMonStatsTxtRecordCount)
            nBaseMonsterId = -1;
        bVoiceMode = nUnitMode == MONMODE_NEUTRAL || nUnitMode == MONMODE_WALK || nUnitMode == MONMODE_RUN;
        if (nBaseMonsterId == MONSTER_VULTURE1)
        {
            if (nUnitMode != MONMODE_SKILL1 && !bVoiceMode)
                return;
        }
        else if (!(nBaseMonsterId == MONSTER_BATDEMON1 && nUnitMode == MONMODE_SKILL4) && !bVoiceMode)
            return;

        pMonSoundsTxt = UNITSND_GetMonSoundsRecordForUnit(pUnit);
        if (!pMonSoundsTxt)
            return;
        pMonSoundsTxt2 = UNITSND_GetMonSoundsRecordForUnit(pUnit);
        nNeutralSound = pMonSoundsTxt2 ? pMonSoundsTxt2->dwNeutral : SOUND_NONE;
        nFrame = GAME_GetFrameCount();
        nNeuTime = pMonSoundsTxt->dwNeuTime;
        if (!nNeutralSound)
            return;
        if (UNITSND_FindPlayingSoundInGroup(pUnit, nNeutralSound))
            return;
        if (nFrame - gnLastNeutralSoundFrame < (uint32_t)gnNeutralSoundReArmDelay)
            return;
        if (nFrame - UNITSND_GetSfxTicks(pUnit) < nNeuTime)
            return;
        if (nFrame - UNITSND_GetSfxAsyncTicks(pUnit) < nNeuTime)
            return;
        pRoom = D2COMMON_UNITS_GetRoom(pUnit);
        if (pRoom && D2COMMON_DUNGEON_IsRoomInTown(pRoom))
            return;
        if (!SOUNDINST_IsUnitInEarshot(pUnit))
            return;
        if (SOUNDSONG_RollRandom(15))
            return;

        SOUNDINST_PlaySound(nNeutralSound, pUnit, 0, 0, 0);
        gnLastNeutralSoundFrame = nFrame;
        UNITSND_SetSfxAsyncTicks(pUnit, nFrame);
        gnNeutralSoundReArmDelay = SOUNDSONG_RollRandomInRange(30, 90);
        return;
    }

    if (UNITSND_GetSoundClassId(pUnit) == OBJECT_CAINGIBBET && nUnitMode == OBJMODE_NEUTRAL)
    {
        if (!UNITSND_GetSfxAsyncTicks(pUnit) && D2COMMON_UNITS_GetDistanceToOtherUnit(pUnit, CUNIT_GetPlayer()) < 20)
        {
            SOUNDINST_PlaySound(SOUND_CAIN_ACT1_HELP, pUnit, 0, 0, 0);
            pUnit->dwSfxAsyncTicks = GAME_GetFrameCount();
        }
    }

    if (!pUnit)
    {
        FOG_DisplayAssert("ptUnit", szUnitSndCpp, 3215);
        exit(-1);
    }
    if (pUnit->bUnitSfx && nUnitMode == pUnit->dwSfxMode)
        return;

    nObjectClass = UNITSND_GetSoundClassId(pUnit);
    if (!(nObjectClass < 573 && nUnitMode < (int32_t)NUM_OBJECTMODES))
    {
        FOG_DisplayAssert("eUnitClass < NUM_OBJECTS && nMode < NUM_OBJECTMODES", szUnitSndCpp, 3718);
        exit(-1);
    }

    nModeSound = SOUND_NONE;
    pModeSounds = gpObjectModeSounds[nObjectClass];
    if (pModeSounds)
    {
        if (nObjectClass == OBJECT_INVISIBLE_OBJECT)
            //  Record [21] of gObjectModeSoundTable, the invisible object's, read by its address.
            nModeSound = gObjectModeSoundTable[21].nModeSounds[nUnitMode];
        else if (pModeSounds->nAltSound1 || pModeSounds->nAltSound2)
        {
            if (pModeSounds->nAltMode1 == nUnitMode || pModeSounds->nAltMode1 == 8)
                nModeSound = pModeSounds->nAltSound1;
            else if (pModeSounds->nAltMode2 == nUnitMode || pModeSounds->nAltMode2 == 8)
                nModeSound = pModeSounds->nAltSound2;
        }
    }

    if (nModeSound)
    {
        bGroupPlaying = FALSE;
        if (SOUNDCHAN_IsSystemReady_2())
        {
            nGroupBase = SOUNDHDR_GetGroupBaseIndex(nModeSound);
            nGroupSize = SOUNDHDR_GetGroupSize(nGroupBase);
            hInstance = 0;
            for (pNode = *UNITSND_GetSoundListHead(pUnit); pNode; pNode = pNode->pNext)
            {
                hInstance = pNode->nValue;
                nPlayingSoundId = SOUNDINST_GetSoundIdFromHandle(hInstance);
                if (nPlayingSoundId >= nGroupBase && nPlayingSoundId < nGroupBase + nGroupSize)
                    break;
            }
            bGroupPlaying = pNode && hInstance;
        }
        if (!bGroupPlaying)
            SOUNDINST_PlaySound(nModeSound, pUnit, 0, 0, 0);
    }
    else
    {
        for (pLoopNode = *UNITSND_GetSoundListHead(pUnit); pLoopNode; pLoopNode = pNext)
        {
            hLoopInstance = pLoopNode->nValue;
            pNext = pLoopNode->pNext;
            if (SOUNDHDR_IsLooping(SOUNDINST_GetSoundIdFromHandle(hLoopInstance)))
                SOUNDINST_DetachFromUnit(hLoopInstance, pUnit, FALSE);
        }
    }

    bTownPortal = FALSE;
    if (UNITSND_GetSoundClassId(pUnit) == OBJECT_TOWN_PORTAL)
        bTownPortal = TRUE;
    if (pUnit->bUnitSfx || bTownPortal)
    {
        nOldSfxMode = pUnit->dwSfxMode;
        nObjectClass = UNITSND_GetSoundClassId(pUnit);
        if (!(nObjectClass < 573 && nUnitMode < (int32_t)NUM_OBJECTMODES))
        {
            FOG_DisplayAssert("eUnitClass < NUM_OBJECTS && nMode < NUM_OBJECTMODES", szUnitSndCpp, 3698);
            exit(-1);
        }
        pObjectModeSound = gpObjectModeSounds[nObjectClass];
        //  Record [106] of gObjectModeSoundTable, the wells', compared by its address.
        if (pObjectModeSound && (pObjectModeSound != &gObjectModeSoundTable[106] || nOldSfxMode <= nUnitMode))
        {
            nModeSoundId = pObjectModeSound->nModeSounds[nUnitMode];
            if (nModeSoundId)
                SOUNDINST_PlaySound(nModeSoundId, pUnit, 0, 0, 0);
        }
    }
    UNITSND_SetSfxStarted(pUnit, 1);
    UNITSND_SetSfxMode(pUnit, nUnitMode);
}


//1.10f: D2Client.0x6FB50850
//  Reads a unit's type, class and mode the way the sound code wants them -- type UNIT_TYPES_COUNT (6) and class -1 for a NULL unit; for a player
//  the three are put through D2COMMON_11013_ConvertMode -- and returns the class. A NULL unit is handed to D2COMMON_UNITS_GetAnimOrSeqMode, which
//  answers 0 for it, and never to ConvertMode. The original inlines it in other UNITSND_ functions.
int32_t __fastcall UNITSND_GetSoundClassId(D2UnitStrc *pUnit)
{
    int32_t nType;
    int32_t nClass;
    int32_t nMode;

    nType = pUnit ? (int32_t)pUnit->dwUnitType : (int32_t)UNIT_TYPES_COUNT;
    nClass = pUnit ? (int32_t)pUnit->dwClassId : -1;
    nMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
    if (!nType)
        D2COMMON_11013_ConvertMode(pUnit, &nType, &nClass, &nMode, szUnitSndCpp, 2817);
    return nClass;
}


//1.10f: D2Client.0x6FB508B0
//  GetSoundClassId's twin: returns the type the sound code uses, after ConvertMode for a player. The original inlines it in other UNITSND_
//  functions.
int32_t __fastcall UNITSND_GetSoundUnitType(D2UnitStrc *pUnit)
{
    int32_t nType;
    int32_t nClass;
    int32_t nMode;

    nType = pUnit ? (int32_t)pUnit->dwUnitType : (int32_t)UNIT_TYPES_COUNT;
    nClass = pUnit ? (int32_t)pUnit->dwClassId : -1;
    nMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
    if (!nType)
        D2COMMON_11013_ConvertMode(pUnit, &nType, &nClass, &nMode, szUnitSndCpp, 2829);
    return nType;
}


//1.10f: D2Client.0x6FB50910
//  Returns the address of a unit's sound list head, &pUnit->pUnitSfxData; the unit is asserted. The original inlines it in other UNITSND_
//  functions: a copy that hands the list on takes the address, one that walks it folds the dereference into the load.
D2SoundListNodeStrc **__fastcall UNITSND_GetSoundListHead(D2UnitStrc *pUnit)
{
    if (!pUnit)
    {
        FOG_DisplayAssert("ptUnit", szUnitSndCpp, 3116);
        exit(-1);
    }
    return &pUnit->pUnitSfxData;
}


//1.10f: D2Client.0x6FB50940
//  Sets the unit's bUnitSfx byte (+70h); the unit is asserted.
void __fastcall UNITSND_SetSfxStarted(D2UnitStrc *pUnit, uint8_t bStarted)
{
    if (!pUnit)
    {
        FOG_DisplayAssert("ptUnit", szUnitSndCpp, 3226);
        exit(-1);
    }
    pUnit->bUnitSfx = bStarted;
}


//1.10f: D2Client.0x6FB50970
//  Sets the unit's dwSfxMode (+74h), the mode a sound was last started for. It is written from an int and read signed.
void __fastcall UNITSND_SetSfxMode(D2UnitStrc *pUnit, int32_t nMode)
{
    if (!pUnit)
    {
        FOG_DisplayAssert("ptUnit", szUnitSndCpp, 3248);
        exit(-1);
    }
    pUnit->dwSfxMode = nMode;
}


//1.10f: D2Client.0x6FB509A0
//  Returns the unit's dwSfxTicks (+7Ch); the unit is asserted. The member and the answer are unsigned: the caller subtracts it from the frame count
//  and compares the difference unsigned. The original inlines it in UNITSND_PlayMonsterSkillSound and UNITSND_PlayAreaEntryQuestLine.
uint32_t __fastcall UNITSND_GetSfxTicks(D2UnitStrc *pUnit)
{
    if (!pUnit)
    {
        FOG_DisplayAssert("ptUnit", szUnitSndCpp, 3259);
        exit(-1);
    }
    return pUnit->dwSfxTicks;
}


//1.10f: D2Client.0x6FB509D0
//  Returns the unit's dwSfxAsyncTicks (+80h); the unit is asserted. Unsigned, like the member: the caller's compare is unsigned. The original
//  inlines it in UNITSND_UpdateUnitModeSound and UNITSND_PlayMonsterInitSound.
uint32_t __fastcall UNITSND_GetSfxAsyncTicks(D2UnitStrc *pUnit)
{
    if (!pUnit)
    {
        FOG_DisplayAssert("ptUnit", szUnitSndCpp, 3281);
        exit(-1);
    }
    return pUnit->dwSfxAsyncTicks;
}


//1.10f: D2Client.0x6FB50A00
//  Sets the unit's dwSfxAsyncTicks (+80h) to the frame its idle voice played on (unsigned, like the member).
void __fastcall UNITSND_SetSfxAsyncTicks(D2UnitStrc *pUnit, uint32_t nFrame)
{
    if (!pUnit)
    {
        FOG_DisplayAssert("ptUnit", szUnitSndCpp, 3292);
        exit(-1);
    }
    pUnit->dwSfxAsyncTicks = nFrame;
}


//1.10f: D2Client.0x6FB50A30
//  Plays a unit's attack sounds for nMode. A player: its swing, the hit class's sound (UNITSND_GetHitClassSound, the hit class being D2Common's
//  UNITS_GetHitClass) at the hit class's delay (UNITSND_GetHitClassSoundDelay), and the frame stamped into dwSfxTicks; a bDelayed of 0 zeroes the
//  delay after it is computed.
//  A monster, from its monsounds.txt row: the 1 columns for MONMODE_ATTACK1 and the 2 columns for any other mode. First the voice, Attack<n> at
//  Att<n>Del frames as they stand (0 when bDelayed is 0), which plays when Attack<n> is not 0 and SOUNDSONG_RollRandom(100) rolls below Att<n>Prb.
//  A probability below 100 also asks for 15 frames since any monster's last voice (gnLastMonsterVoiceFrame) and no speech playing on the unit,
//  where speech skips the voice outright; at 100 neither is asked.
//  Before the voice plays, every instance on the unit's list whose sound's group is the row's Neutral is let go: DetachFromUnit with bForce when
//  SOUNDINST_HasActiveChannel answers 1 (two or more units hold the instance), otherwise UpdateForUnit to volume 0 with 6 fade ticks. dwSfxTicks
//  and gnLastMonsterVoiceFrame are stamped after the voice. The row is fetched again for every node, Neutral read with a NULL test of its own.
//  Then, voice or not, the weapon: Weapon<n> at Wea<n>Del animation frames, (Wea<n>Del * 256 + 128) / wAnimSpeed as UNITSND_GetHitClassSoundDelay
//  divides, not zeroed by bDelayed, with Wea<n>Vol set on its instance.
//  The probability is compared with two signs: against 100 unsigned, against the roll signed; the member is unsigned int, so both sides of the
//  second compare are cast. The 15-frame test is unsigned. The two dwSfxTicks stores are plain stores, with no assert.
//  The original inlines UNITSND_GetSoundUnitType, UNITSND_GetHitClassSound, UNITSND_GetHitClassSoundDelay and UNITSND_GetSoundListHead here.
void __fastcall UNITSND_PlayAttackSound(D2UnitStrc *pUnit, int32_t nMode, int32_t bDelayed)
{
    int32_t nType;
    int32_t nHitClass;
    D2C_Sounds nSwingSound;
    uint32_t nSwingDelay;
    D2MonSoundsTxt *pMonSoundsTxt;
    D2MonSoundsTxt *pMonSoundsTxt2;
    D2C_Sounds nVoiceSound;
    int32_t nVoiceDelay;
    uint32_t nVoiceProbability;
    BOOL bVoice;
    D2SoundListNodeStrc *pNode;
    D2SoundListNodeStrc *pNext;
    int32_t hInstance;
    uint32_t nNeutralSound;
    uint32_t nWeaponAnimSpeed;
    D2C_Sounds nWeaponSound;
    uint32_t nWeaponDel;
    int32_t nWeaponVolume;
    int32_t hWeaponInstance;

    nType = UNITSND_GetSoundUnitType(pUnit);
    if (nType == UNIT_PLAYER)
    {
        nHitClass = D2COMMON_UNITS_GetHitClass(pUnit);
        nSwingSound = UNITSND_GetHitClassSound(pUnit, nHitClass);
        nSwingDelay = UNITSND_GetHitClassSoundDelay(pUnit, nHitClass);
        if (!bDelayed)
            nSwingDelay = 0;
        SOUNDINST_PlaySound(nSwingSound, pUnit, nSwingDelay, 0, 0);
        pUnit->dwSfxTicks = GAME_GetFrameCount();
        return;
    }
    if (nType != UNIT_MONSTER)
        return;

    pMonSoundsTxt = UNITSND_GetMonSoundsRecordForUnit(pUnit);
    if (!pMonSoundsTxt)
        return;

    if (nMode == MONMODE_ATTACK1)
    {
        nVoiceSound = pMonSoundsTxt->dwAttack1;
        nVoiceDelay = pMonSoundsTxt->dwAtt1Del;
    }
    else
    {
        nVoiceSound = pMonSoundsTxt->dwAttack2;
        nVoiceDelay = pMonSoundsTxt->dwAtt2Del;
    }
    if (!bDelayed)
        nVoiceDelay = 0;
    nVoiceProbability = nMode == MONMODE_ATTACK1 ? pMonSoundsTxt->dwAtt1Prb : pMonSoundsTxt->dwAtt2Prb;

    bVoice = nVoiceSound != 0;
    if (nVoiceProbability < 100)
    {
        if (GAME_GetFrameCount() - gnLastMonsterVoiceFrame < 15)
            bVoice = FALSE;
        //  Speech playing on the unit jumps past the roll to the weapon; a cleared bVoice skips the roll the same way. SOUNDSONG_RollRandom,
        //  which steps its seed, is not called on either path.
        if (SOUNDINST_IsSpeechPlayingForUnit(pUnit))
            bVoice = FALSE;
    }
    if (bVoice && (int32_t)SOUNDSONG_RollRandom(100) < (int32_t)nVoiceProbability)
    {
        for (pNode = *UNITSND_GetSoundListHead(pUnit); pNode; pNode = pNext)
        {
            pNext = pNode->pNext;
            hInstance = pNode->nValue;
            pMonSoundsTxt2 = UNITSND_GetMonSoundsRecordForUnit(pUnit);
            nNeutralSound = pMonSoundsTxt2 ? pMonSoundsTxt2->dwNeutral : 0;
            if (SOUNDHDR_GetGroupBaseIndex(SOUNDINST_GetSoundIdFromHandle(hInstance)) == nNeutralSound)
            {
                //  The fade's delay is the 0 that SOUNDINST_HasActiveChannel left in eax.
                if (SOUNDINST_HasActiveChannel(hInstance))
                    SOUNDINST_DetachFromUnit(hInstance, pUnit, TRUE);
                else
                    SOUNDINST_UpdateForUnit(hInstance, 0, 0, 6);
            }
        }
        SOUNDINST_PlaySound(nVoiceSound, pUnit, nVoiceDelay, 0, 0);
        pUnit->dwSfxTicks = GAME_GetFrameCount();
        gnLastMonsterVoiceFrame = GAME_GetFrameCount();
    }

    nWeaponAnimSpeed = (uint32_t)(int32_t)pUnit->wAnimSpeed;
    if (!nWeaponAnimSpeed)
        nWeaponAnimSpeed = 1;
    if (nMode == MONMODE_ATTACK1)
    {
        nWeaponSound = pMonSoundsTxt->dwWeapon1;
        nWeaponDel = pMonSoundsTxt->dwWea1Del;
    }
    else
    {
        nWeaponSound = pMonSoundsTxt->dwWeapon2;
        nWeaponDel = pMonSoundsTxt->dwWea2Del;
    }
    nWeaponVolume = nMode == MONMODE_ATTACK1 ? pMonSoundsTxt->dwWea1Vol : pMonSoundsTxt->dwWea2Vol;
    hWeaponInstance = SOUNDINST_PlaySound(nWeaponSound, pUnit, ((nWeaponDel << 8) + 128) / nWeaponAnimSpeed, 0, 0);
    if (hWeaponInstance)
        SOUNDINST_SetVolumeByHandle(hWeaponInstance, nWeaponVolume);
}


//1.10f: D2Client.0x6FB50D20
//  Plays a unit's sound for the mode it enters -- the dispatcher. nMode is that mode when bModeGiven is not 0; when it is 0, nMode is not read and
//  the unit's own mode is taken, put through ConvertMode for a player. Three switches: the player's modes, BLOCK's hit class and the monster's
//  modes.
//  A player: PLRMODE_DEATH, GETHIT and KNOCKBACK -> UNITSND_PlayHitOrDeathSound; ATTACK1 and ATTACK2 -> UNITSND_PlayAttackSound with bDelayed;
//  SPECIAL3 for a barbarian and SPECIAL4 for an assassin -> UNITSND_PlayAttackSound with bDelayed; KICK -> weapon_kick_1, 4 frames late for an
//  assassin and at once for the rest; BLOCK -> the block sound of dwLastHitClass's low nibble: block_weapon_1 for the hit classes 1..9 (hand to
//  hand through staff), block_arrow_1 for 10 and 11 (bow, crossbow), 0 for the rest (which SOUNDINST_PlaySound refuses), and both frames stamped,
//  dwSfxTicks and gnLastMonsterVoiceFrame, whatever was played. Any other mode, nothing.
//  A monster: no monsounds.txt row, nothing. Then the row's three conversions Cvt1..3, each on the mode the one before may have changed: a mode
//  equal to CvtMo<n> becomes CvtTgt<n> when CvtSk<n> is below 0 (an empty cell, which the loader stores as -1; CvtSk is read signed). Then
//  MONMODE_DEATH, GETHIT and KNOCKBACK -> UNITSND_PlayHitOrDeathSound; ATTACK1 and ATTACK2 -> UNITSND_PlayAttackSound with bDelayed; SKILL1..SKILL4
//  -> UNITSND_PlayMonsterSkillSound; DEAD -> UNITSND_StopMonsterVoices; any other, nothing.
//  The conversions are a loop over three 8-byte triples in the original; D2MonSoundsTxt declares them as nine members, not an array, so the code
//  writes the three tests in their order.
//  The original inlines UNITSND_GetSoundUnitType, UNITSND_GetSoundClassId and the sound code's mode read here.
void __fastcall UNITSND_PlayModeSound(D2UnitStrc *pUnit, int32_t nMode, int32_t bModeGiven)
{
    int32_t nType;
    int32_t nClass;
    int32_t nModeType;
    int32_t nModeClass;
    int32_t nUnitMode;
    D2C_Sounds nBlockSound;
    D2MonSoundsTxt *pMonSoundsTxt;

    nType = UNITSND_GetSoundUnitType(pUnit);
    nClass = UNITSND_GetSoundClassId(pUnit);
    if (!bModeGiven)
    {
        nModeType = pUnit ? (int32_t)pUnit->dwUnitType : (int32_t)UNIT_TYPES_COUNT;
        nModeClass = pUnit ? (int32_t)pUnit->dwClassId : -1;
        nUnitMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
        if (!nModeType)
            D2COMMON_11013_ConvertMode(pUnit, &nModeType, &nModeClass, &nUnitMode, szUnitSndCpp, 2841);
        nMode = nUnitMode;
    }

    if (nType == UNIT_PLAYER)
    {
        switch (nMode)
        {
        case PLRMODE_DEATH:
        case PLRMODE_GETHIT:
        case PLRMODE_KNOCKBACK:
            UNITSND_PlayHitOrDeathSound(pUnit, nMode);
            break;
        case PLRMODE_ATTACK1:
        case PLRMODE_ATTACK2:
            UNITSND_PlayAttackSound(pUnit, nMode, TRUE);
            break;
        case PLRMODE_BLOCK:
            nBlockSound = SOUND_NONE;
            switch (pUnit->dwLastHitClass & 0xF)
            {
            case HITCLASS_HandToHand:
            case HITCLASS_OneHandSwingVsSmall:
            case HITCLASS_OneHandSwingVsLarge:
            case HITCLASS_TwoHandSwingVsSmall:
            case HITCLASS_TwoHandSwingVsLarge:
            case HITCLASS_OneHandThrust:
            case HITCLASS_TwoHandThrust:
            case HITCLASS_Club:
            case HITCLASS_Staff:
                nBlockSound = SOUND_BLOCK_WEAPON_1;
                break;
            case HITCLASS_Bow:
            case HITCLASS_Crossbow:
                nBlockSound = SOUND_BLOCK_ARROW_1;
                break;
            default:
                break;
            }
            SOUNDINST_PlaySound(nBlockSound, pUnit, 0, 0, 0);
            pUnit->dwSfxTicks = GAME_GetFrameCount();
            gnLastMonsterVoiceFrame = GAME_GetFrameCount();
            break;
        case PLRMODE_KICK:
            if (nClass == PCLASS_ASSASSIN)
                SOUNDINST_PlaySound(SOUND_WEAPON_KICK_1, pUnit, 4, 0, 0);
            else
                SOUNDINST_PlaySound(SOUND_WEAPON_KICK_1, pUnit, 0, 0, 0);
            break;
        case PLRMODE_SPECIAL3:
            if (nClass == PCLASS_BARBARIAN)
                UNITSND_PlayAttackSound(pUnit, PLRMODE_SPECIAL3, TRUE);
            break;
        case PLRMODE_SPECIAL4:
            if (nClass == PCLASS_ASSASSIN)
                UNITSND_PlayAttackSound(pUnit, PLRMODE_SPECIAL4, TRUE);
            break;
        default:
            break;
        }
        return;
    }
    if (nType != UNIT_MONSTER)
        return;

    pMonSoundsTxt = UNITSND_GetMonSoundsRecordForUnit(pUnit);
    if (!pMonSoundsTxt)
        return;
    if (nMode == pMonSoundsTxt->nCvtMo1 && pMonSoundsTxt->dwCvtSk1 < 0)
        nMode = pMonSoundsTxt->nCvtTgt1;
    if (nMode == pMonSoundsTxt->nCvtMo2 && pMonSoundsTxt->dwCvtSk2 < 0)
        nMode = pMonSoundsTxt->nCvtTgt2;
    if (nMode == pMonSoundsTxt->nCvtMo3 && pMonSoundsTxt->dwCvtSk3 < 0)
        nMode = pMonSoundsTxt->nCvtTgt3;

    switch (nMode)
    {
    case MONMODE_DEATH:
    case MONMODE_GETHIT:
    case MONMODE_KNOCKBACK:
        UNITSND_PlayHitOrDeathSound(pUnit, nMode);
        break;
    case MONMODE_ATTACK1:
    case MONMODE_ATTACK2:
        UNITSND_PlayAttackSound(pUnit, nMode, TRUE);
        break;
    case MONMODE_SKILL1:
    case MONMODE_SKILL2:
    case MONMODE_SKILL3:
    case MONMODE_SKILL4:
        UNITSND_PlayMonsterSkillSound(pUnit, nMode);
        break;
    case MONMODE_DEAD:
        UNITSND_StopMonsterVoices(pUnit);
        break;
    default:
        break;
    }
}


//1.10f: D2Client.0x6FB51030
//  Stops a monster's voices: every instance on its list whose sound's group is one of its monsounds.txt row's voices -- Attack1, Attack2, HitSound,
//  Neutral, Skill1..4, Init, Taunt and Flee, compared in that order -- is detached with bForce 1, the next node read first. Weapon1, Weapon2,
//  DeathSound and the footsteps are not compared. Only for a monster (the type after ConvertMode) with a row: UNITSND_StopMonsterSkillSounds' walk
//  over eleven columns instead of four.
//  The only argument is the unit; the two pushes after the list head's test are register saves, not arguments.
//  The original inlines UNITSND_GetSoundUnitType and UNITSND_GetSoundListHead here.
void __fastcall UNITSND_StopMonsterVoices(D2UnitStrc *pUnit)
{
    D2MonSoundsTxt *pMonSoundsTxt;
    D2SoundListNodeStrc *pNode;
    D2SoundListNodeStrc *pNext;
    int32_t hInstance;
    D2C_Sounds nGroupBase;

    if (UNITSND_GetSoundUnitType(pUnit) != UNIT_MONSTER)
        return;

    pMonSoundsTxt = UNITSND_GetMonSoundsRecordForUnit(pUnit);
    if (!pMonSoundsTxt)
        return;

    for (pNode = *UNITSND_GetSoundListHead(pUnit); pNode; pNode = pNext)
    {
        hInstance = pNode->nValue;
        pNext = pNode->pNext;
        nGroupBase = SOUNDHDR_GetGroupBaseIndex(SOUNDINST_GetSoundIdFromHandle(hInstance));
        if (nGroupBase == pMonSoundsTxt->dwAttack1 || nGroupBase == pMonSoundsTxt->dwAttack2
            || nGroupBase == pMonSoundsTxt->dwHitSound || nGroupBase == pMonSoundsTxt->dwNeutral
            || nGroupBase == pMonSoundsTxt->dwSkill[0] || nGroupBase == pMonSoundsTxt->dwSkill[1]
            || nGroupBase == pMonSoundsTxt->dwSkill[2] || nGroupBase == pMonSoundsTxt->dwSkill[3]
            || nGroupBase == pMonSoundsTxt->dwInit || nGroupBase == pMonSoundsTxt->dwTaunt
            || nGroupBase == pMonSoundsTxt->dwFlee)
            SOUNDINST_DetachFromUnit(hInstance, pUnit, TRUE);
    }
}


//1.10f: D2Client.0x6FB51140
//  Plays a unit hit, knocked back or killed: the impact, then the voice. A monster under STATE_INVIS returns at once. Otherwise the two impact
//  sounds of the unit's last hit class (dwLastHitClass, read before that test), from UNITSND_GetImpactSoundsForHitClass with nMode handed on, play
//  at once, the weapon's at 255 when the unit is the player; a monster's MONMODE_DEATH also stops its voices (UNITSND_StopMonsterVoices) and gives
//  its sounds -1 (UNITSND_AdjustUnitSoundRefCounts). Then a unit under STATE_FREEZE, or hit with the SilentVoice layer (the masked layer byte is
//  0xA0), returns.
//  The voice. A monster's, from its monsounds.txt row: for MONMODE_GETHIT and MONMODE_KNOCKBACK its HitSound at HitDelay, none while speech plays
//  on the unit; for any other mode (MONMODE_DEATH) its DeathSound at DeaDelay. A sound of diablo_death adds diablo_death_schwing at 106 frames and
//  diablo_death_hit at 193. A player's, from its class's pair (gpPlayerClassSounds): [0], <class>_hit_1, at 2 frames for PLRMODE_GETHIT and
//  PLRMODE_KNOCKBACK, none while speech plays; [1], <class>_death_1, at 1 frame for any other mode. A 0 sound is not played.
//  Then dwSfxTicks and gnLastMonsterVoiceFrame are stamped, on every path past the FREEZE and SilentVoice tests -- a missing row or pair, a voice
//  held back by speech and a unit of another type included; the three earlier returns (INVIS, FREEZE, SilentVoice) do not stamp. The stamp is a
//  plain store, with no assert.
//  The inlined UNITSND_GetSoundClassId has no NULL arms here: the unit is given.
//  BUG: the inlined type read tests a NULL unit (type 6), and dwLastHitClass is then read through it with no test (`mov ebp, [esi+0B0h]`,
//  0x6FB511A3), a fault at 0xB0.
//  UNREACHABLE: the one caller reaches this function only for a player or a monster.
//  The original inlines UNITSND_GetSoundUnitType, UNITSND_GetSoundClassId and the player class bound assert here.
void __fastcall UNITSND_PlayHitOrDeathSound(D2UnitStrc *pUnit, int32_t nMode)
{
    int32_t nType;
    uint32_t nHitClass;
    D2C_Sounds nWeaponSound;
    D2C_Sounds nLayerSound;
    int32_t hWeaponInstance;
    D2MonSoundsTxt *pMonSoundsTxt;
    BOOL bHit;
    D2C_Sounds nVoiceSound;
    int32_t nVoiceDelay;
    int32_t nClass;
    D2C_Sounds *pPlayerClassSounds;

    nType = UNITSND_GetSoundUnitType(pUnit);
    nHitClass = pUnit->dwLastHitClass;
    if (nType == UNIT_MONSTER && D2COMMON_STATES_CheckState(pUnit, STATE_INVIS))
        return;

    UNITSND_GetImpactSoundsForHitClass(pUnit, nHitClass, nMode, &nWeaponSound, &nLayerSound);
    hWeaponInstance = SOUNDINST_PlaySound(nWeaponSound, pUnit, 0, 0, 0);
    SOUNDINST_PlaySound(nLayerSound, pUnit, 0, 0, 0);
    if (hWeaponInstance && pUnit == CUNIT_GetPlayer())
        SOUNDINST_SetVolumeByHandle(hWeaponInstance, 255);
    if (nType == UNIT_MONSTER && nMode == MONMODE_DEATH)
    {
        UNITSND_StopMonsterVoices(pUnit);
        UNITSND_AdjustUnitSoundRefCounts(pUnit, -1);
    }
    if (D2COMMON_STATES_CheckState(pUnit, STATE_FREEZE))
        return;
    if ((nHitClass & 0xF0) == HITCLASS_Layer_SilentVoice)
        return;

    if (nType == UNIT_MONSTER)
    {
        pMonSoundsTxt = UNITSND_GetMonSoundsRecordForUnit(pUnit);
        if (pMonSoundsTxt)
        {
            bHit = nMode == MONMODE_GETHIT || nMode == MONMODE_KNOCKBACK;
            if (!bHit || !SOUNDINST_IsSpeechPlayingForUnit(pUnit))
            {
                nVoiceSound = bHit ? pMonSoundsTxt->dwHitSound : pMonSoundsTxt->dwDeathSound;
                nVoiceDelay = bHit ? pMonSoundsTxt->dwHitDelay : pMonSoundsTxt->dwDeaDelay;
                if (nVoiceSound)
                    SOUNDINST_PlaySound(nVoiceSound, pUnit, nVoiceDelay, 0, 0);
                if (nVoiceSound == SOUND_DIABLO_DEATH)
                {
                    SOUNDINST_PlaySound(SOUND_DIABLO_DEATH_SCHWING, pUnit, 106, 0, 0);
                    SOUNDINST_PlaySound(SOUND_DIABLO_DEATH_HIT, pUnit, 193, 0, 0);
                }
            }
        }
    }
    else if (nType == UNIT_PLAYER)
    {
        nClass = UNITSND_GetSoundClassId(pUnit);
        if ((uint32_t)nClass >= NUMBER_OF_PLAYERCLASSES)
        {
            FOG_DisplayAssert("eUnitClass < NUM_PLAYERCLASS", szUnitSndCpp, 2850);
            exit(-1);
        }
        pPlayerClassSounds = gpPlayerClassSounds[nClass];
        if (pPlayerClassSounds)
        {
            bHit = nMode == PLRMODE_GETHIT || nMode == PLRMODE_KNOCKBACK;
            if (!bHit || !SOUNDINST_IsSpeechPlayingForUnit(pUnit))
            {
                nVoiceSound = bHit ? pPlayerClassSounds[0] : pPlayerClassSounds[1];
                nVoiceDelay = bHit ? 2 : 1;
                if (nVoiceSound)
                    SOUNDINST_PlaySound(nVoiceSound, pUnit, nVoiceDelay, 0, 0);
            }
        }
    }

    pUnit->dwSfxTicks = GAME_GetFrameCount();
    gnLastMonsterVoiceFrame = GAME_GetFrameCount();
}


//1.10f: D2Client.0x6FB513B0
//  Plays a monster's skill sound for nMode: MONMODE_SKILL1..4 -> its monsounds.txt row's Skill1..4; any other mode, another type or no row,
//  nothing, and a 0 sound is not played. creature_chicken_1 (the chicken row's Skill1) waits 25 frames since the unit's dwSfxTicks (unsigned
//  compare), the frame taken before the mode is looked at. Then dwSfxTicks and gnLastMonsterVoiceFrame are stamped.
//  The dwSfxTicks setter is written out here: the frame fetched, the unit asserted, the frame stored, the argument evaluated before the setter's
//  body as `SetSfxTicks(pUnit, GAME_GetFrameCount())` compiles. The assert stays although the chicken's path asserted the unit already: the other
//  path reaches it untested.
//  The original inlines UNITSND_GetSoundUnitType, UNITSND_GetSfxTicks and that dwSfxTicks setter here.
void __fastcall UNITSND_PlayMonsterSkillSound(D2UnitStrc *pUnit, int32_t nMode)
{
    D2MonSoundsTxt *pMonSoundsTxt;
    uint32_t nNow;
    D2C_Sounds nSkillSound;
    int32_t nTicksFrame;

    if (UNITSND_GetSoundUnitType(pUnit) != UNIT_MONSTER)
        return;

    pMonSoundsTxt = UNITSND_GetMonSoundsRecordForUnit(pUnit);
    if (!pMonSoundsTxt)
        return;

    nNow = GAME_GetFrameCount();
    if (nMode == MONMODE_SKILL1)
        nSkillSound = pMonSoundsTxt->dwSkill[0];
    else if (nMode == MONMODE_SKILL2)
        nSkillSound = pMonSoundsTxt->dwSkill[1];
    else if (nMode == MONMODE_SKILL3)
        nSkillSound = pMonSoundsTxt->dwSkill[2];
    else if (nMode == MONMODE_SKILL4)
        nSkillSound = pMonSoundsTxt->dwSkill[3];
    else
        return;

    if (nSkillSound == SOUND_CREATURE_CHICKEN_1)
    {
        if (nNow - UNITSND_GetSfxTicks(pUnit) < 25)
            return;
    }
    else if (!nSkillSound)
        return;

    SOUNDINST_PlaySound(nSkillSound, pUnit, 0, 0, 0);
    nTicksFrame = GAME_GetFrameCount();
    if (!pUnit)
    {
        FOG_DisplayAssert("ptUnit", szUnitSndCpp, 3270);
        exit(-1);
    }
    pUnit->dwSfxTicks = nTicksFrame;
    gnLastMonsterVoiceFrame = GAME_GetFrameCount();
}


//1.10f: D2Client.0x6FB514E0
//  Plays a player's sound event: nSoundEvent, the 16-bit event of server packet 0x2C (UNITSND_PlayUnitSoundEvent hands it on for a player) or of
//  one of the client's own calls -- the item and cursor sounds, the class's lines and the quest lines.
//  The class's row first: a NULL unit or a class past the seven asserts (the NULL test joins the bound's assert); a NULL row returns. So does the
//  local player when it is dead or missing. Events 25..32 take a chat line, the row's [8] plus two an event (<class>_chat_a_1 .. <class>_chat_h_1),
//  unless speech plays on the unit; it is computed before anything else and played by the switch's default.
//  Events 33..83 are quest lines, the row's [9] plus the event less 33 (<class>_act1_complete_andariel at 33 to <class>_act5_defeat_baal at 83).
//  Eleven hand the line to SOUNDSONG_ScheduleSting as its second sound, with a music sting: 33 andariel, 34 bloodraven, 35 den, 50 radament, 52
//  tainted, 66 mephisto, 75 diablo, 80 shenk, 82 nihlathak and 83 baal, and 37 with none. The original compares them one by one in the order 35,
//  34, 37, 33, 50, 52, 66, 75, 80, 82, 83, an if chain's shape rather than a switch's, and so does the code.
//  37 is the Forgotten Tower's: no first sound, and the line left unarmed (bPlaySound2 0) while the music holds for 1500 frames; event 92
//  (UNITSND_PlayUnitSoundEvent) arms it 25 frames on. Every other quest line plays at once, at 0 frames for the ten area-entry lines (38, 40..44,
//  46, 47, 55, 56) and at 12 for the rest, and shows its subtitle.
//  Below 33 and above 83, a switch over 1..24: 1, 7, 8, 9 and 11 a fixed sound on the unit; 2..6 a fixed sound on no unit (the PlaySound unit
//  zeroed while the subtitle keeps the player); 19..24 the row's nag lines -- [2] impossible, [6] cantuseyet, [3] needmana, [4] needkey, [5]
//  cantcarry, [7] not_in_town -- which speech on the unit silences and which do not repeat the last nag line within 75 frames (gnLastNagSoundId,
//  gnLastNagSoundFrame; unsigned compare). Every other event -- 10, 12..18, the chat events, 0 and anything past 83 -- plays the chat line or
//  nothing.
//  The original's fixed sounds jump straight to the play; ours passes them through the two tests before it, which a non-zero constant and a FALSE
//  bIsNagSound pass. What plays shows its subtitle: TEXT_ChatShowSpeechSubtitle with the sound the new instance holds.
//  The original inlines the player class bound assert here.
void __fastcall UNITSND_PlayPlayerSoundEvent(D2UnitStrc *pPlayer, uint16_t nSoundEvent)
{
    D2C_Sounds *pPlayerClassSounds;
    int32_t nEvent;
    int32_t nSoundId;
    D2UnitStrc *pSoundUnit;
    int32_t nQuestSoundId;
    int32_t nDelayFrames;
    BOOL bIsNagSound;
    uint32_t nNow;

    if (!pPlayer || (uint32_t)pPlayer->dwClassId >= NUMBER_OF_PLAYERCLASSES)
    {
        FOG_DisplayAssert("eUnitClass < NUM_PLAYERCLASS", szUnitSndCpp, 2850);
        exit(-1);
    }
    pPlayerClassSounds = gpPlayerClassSounds[pPlayer->dwClassId];
    if (!pPlayerClassSounds)
        return;

    nSoundId = 0;
    nEvent = nSoundEvent;
    pSoundUnit = pPlayer;
    if (pPlayer == CUNIT_GetPlayer() && CUNIT_IsPlayerDeadOrMissing())
        return;

    if (nEvent >= 25 && nEvent <= 32 && !SOUNDINST_IsSpeechPlayingForUnit(pPlayer))
        nSoundId = pPlayerClassSounds[8] + 2 * (nEvent - 25);

    if (nEvent >= 33 && nEvent <= 83)
    {
        nQuestSoundId = pPlayerClassSounds[9] + nEvent - 33;
        if (nEvent == 35)
            SOUNDSONG_ScheduleSting(SOUND_MUSIC_QUEST_DEN, 0, 300, 1, (D2C_Sounds)nQuestSoundId, 250, TRUE);
        else if (nEvent == 34)
            SOUNDSONG_ScheduleSting(SOUND_MUSIC_QUEST_BLOODRAVEN, 475, 800, 1, (D2C_Sounds)nQuestSoundId, 525, TRUE);
        else if (nEvent == 37)
            SOUNDSONG_ScheduleSting(SOUND_NONE, 0, 1500, 0, (D2C_Sounds)nQuestSoundId, 0, FALSE);
        else if (nEvent == 33)
            SOUNDSONG_ScheduleSting(SOUND_MUSIC_QUEST_ANDARIEL, 0, 475, 0, (D2C_Sounds)nQuestSoundId, 425, TRUE);
        else if (nEvent == 50)
            SOUNDSONG_ScheduleSting(SOUND_MUSIC_QUEST_RADAMENT, 475, 1100, 1, (D2C_Sounds)nQuestSoundId, 525, TRUE);
        else if (nEvent == 52)
            SOUNDSONG_ScheduleSting(SOUND_MUSIC_QUEST_TAINTED, 0, 425, 1, (D2C_Sounds)nQuestSoundId, 375, TRUE);
        else if (nEvent == 66)
            SOUNDSONG_ScheduleSting(SOUND_MUSIC_QUEST_MEPHISTO, 0, 425, 0, (D2C_Sounds)nQuestSoundId, 375, TRUE);
        else if (nEvent == 75)
            SOUNDSONG_ScheduleSting(SOUND_MUSIC_QUEST_DIABLO, 0, 475, 0, (D2C_Sounds)nQuestSoundId, 325, TRUE);
        else if (nEvent == 80)
            SOUNDSONG_ScheduleSting(SOUND_MUSIC_QUEST_SHENK, 0, 500, 1, (D2C_Sounds)nQuestSoundId, 475, TRUE);
        else if (nEvent == 82)
            SOUNDSONG_ScheduleSting(SOUND_MUSIC_QUEST_NIHLATHAK, 0, 550, 1, (D2C_Sounds)nQuestSoundId, 400, TRUE);
        else if (nEvent == 83)
            SOUNDSONG_ScheduleSting(SOUND_MUSIC_QUEST_BAAL, 0, 525, 1, (D2C_Sounds)nQuestSoundId, 425, TRUE);
        else
        {
            switch (nEvent)
            {
            case 38:
            case 40:
            case 41:
            case 42:
            case 43:
            case 44:
            case 46:
            case 47:
            case 55:
            case 56:
                nDelayFrames = 0;
                break;
            default:
                nDelayFrames = 12;
                break;
            }
            TEXT_ChatShowSpeechSubtitle(pPlayer, SOUNDINST_GetSoundIdFromHandle(SOUNDINST_PlaySound((D2C_Sounds)nQuestSoundId, pPlayer, nDelayFrames, 0, 0)));
        }
        return;
    }

    bIsNagSound = FALSE;
    switch (nEvent)
    {
    case 1:
        nSoundId = SOUND_ITEM_PICKUP;
        break;
    case 2:
        nSoundId = SOUND_CURSOR_LEVEL_UP;
        pSoundUnit = NULL;
        break;
    case 3:
        nSoundId = SOUND_CURSOR_CONVERT_READY;
        pSoundUnit = NULL;
        break;
    case 4:
        nSoundId = SOUND_CURSOR_CONVERT_ITEM;
        pSoundUnit = NULL;
        break;
    case 5:
        nSoundId = SOUND_CURSOR_IDENTIFY_READY;
        pSoundUnit = NULL;
        break;
    case 6:
        nSoundId = SOUND_CURSOR_IDENTIFY_ITEM;
        pSoundUnit = NULL;
        break;
    case 7:
        nSoundId = SOUND_PLAYER_TOWNPORTAL_CAST;
        break;
    case 8:
        nSoundId = SOUND_PLAYER_TOWNPORTAL_ENTER;
        break;
    case 9:
        nSoundId = SOUND_CURSOR_DURABILITY_BREAK;
        break;
    case 11:
        nSoundId = SOUND_ITEM_KEY_USED;
        break;
    case 19:
        nSoundId = pPlayerClassSounds[2];
        bIsNagSound = TRUE;
        break;
    case 20:
        nSoundId = pPlayerClassSounds[6];
        bIsNagSound = TRUE;
        break;
    case 21:
        nSoundId = pPlayerClassSounds[3];
        bIsNagSound = TRUE;
        break;
    case 22:
        nSoundId = pPlayerClassSounds[4];
        bIsNagSound = TRUE;
        break;
    case 23:
        nSoundId = pPlayerClassSounds[5];
        bIsNagSound = TRUE;
        break;
    case 24:
        nSoundId = pPlayerClassSounds[7];
        bIsNagSound = TRUE;
        break;
    default:
        break;
    }

    if (bIsNagSound && SOUNDINST_IsSpeechPlayingForUnit(pPlayer))
        return;
    if (!nSoundId)
        return;
    if (bIsNagSound)
    {
        nNow = GAME_GetFrameCount();
        if (nSoundId == gnLastNagSoundId && nNow - gnLastNagSoundFrame < 75)
            return;
        gnLastNagSoundFrame = nNow;
        gnLastNagSoundId = nSoundId;
    }
    TEXT_ChatShowSpeechSubtitle(pPlayer, SOUNDINST_GetSoundIdFromHandle(SOUNDINST_PlaySound((D2C_Sounds)nSoundId, pSoundUnit, 0, 0, 0)));
}


//1.10f: D2Client.0x6FB51920
//  Plays a unit's sound event, the body of server packet 0x2C (SCMD_HandleSrv2C_PlayUnitSound): the unit is found by GUID and type, and nothing
//  more happens when there is none. Then, by the event:
//      10  shrine_refill on the unit, its instance's volume 120
//      12  the start sound (skills.txt stsound) of the skill that the stat list of state_evade names in stat 350 (modifierlist_skill), when the
//      unit has
//          that skill and the sound is above 0; it is read signed
//      13  object_trap_trigger      14  object_trap_release
//      15  rogue_confirm_a_1, for monster class 271 (roguehire) only
//      16  a monster's monsounds.txt Taunt -- sound 0 with no row
//      17  a monster's Flee, 6..8 frames on (SOUNDSONG_RollRandom(3) plus 6), at least 4 frames after any monster's last voice
//          (gnLastMonsterVoiceFrame; unsigned compare); dwSfxTicks (a plain store) and the voice frame stamped
//      18  the NPC speech OBJECT_PickNpcSpeechSound picks for the unit, played on the local player with play flags 1
//      84..87  a hireling's line, roguehire's female one or the male one, and its subtitle: 84 thank_you (4615/4625), 85 cant_use_ever (4612/4616),
//          86 cant_use_yet (4613/4619), 87 i_will_use_that (4614/4622)
//      90  ancient_act5_reward_spiel  91  cursor_level_up_hireling on the local player  92  SOUNDSONG_ScheduleSting2(25, 1), the second sting armed
//          (UNITSND_PlayPlayerSoundEvent's event 37)  93  object_corpse_loot
//  Every other event, and 84..87 after their line, goes on to UNITSND_PlayPlayerSoundEvent when the unit is a player: the hireling events too,
//  which a player unit takes as no event of its own.
//  nSoundEvent is 16 bits; dwUnitGUID is unsigned, as CUNIT_FindUnitByGuidAndType takes it; nUnitType is a byte.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, UNITSND_GetSoundClassId and UNITSND_GetSoundUnitType here.
void __fastcall UNITSND_PlayUnitSoundEvent(uint32_t dwUnitGUID, uint8_t nUnitType, uint16_t nSoundEvent)
{
    D2UnitStrc *pUnit;
    D2StatListStrc *pEvadeStatList;
    int32_t nSkillId;
    D2SkillsTxt *pSkillsTxt;
    D2MonSoundsTxt *pMonSoundsTxt;
    D2UnitStrc *pPlayer;
    D2C_Sounds nVoiceSound;

    pUnit = CUNIT_FindUnitByGuidAndType(dwUnitGUID, (D2C_UnitTypes)nUnitType);
    if (!pUnit)
        return;

    switch (nSoundEvent)
    {
    case 10:
        SOUNDINST_SetVolumeByHandle(SOUNDINST_PlaySound(SOUND_SHRINE_REFILL, pUnit, 0, 0, 0), 120);
        return;

    case 12:
        pEvadeStatList = D2COMMON_STATLIST_GetStatListFromUnitAndState(pUnit, STATE_EVADE);
        if (!pEvadeStatList)
            return;
        nSkillId = D2COMMON_STATLIST_GetStatValue(pEvadeStatList, STAT_MODIFIERLIST_SKILL, 0);
        if (!D2COMMON_SKILLS_GetHighestLevelSkillFromUnitAndId(pUnit, (D2C_Skills)nSkillId))
            return;
        pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
        if (pSkillsTxt && pSkillsTxt->wStSound > 0)
            SOUNDINST_PlaySound((D2C_Sounds)pSkillsTxt->wStSound, pUnit, 0, 0, 0);
        return;

    case 13:
        SOUNDINST_PlaySound(SOUND_OBJECT_TRAP_TRIGGER, pUnit, 0, 0, 0);
        return;

    case 14:
        SOUNDINST_PlaySound(SOUND_OBJECT_TRAP_RELEASE, pUnit, 0, 0, 0);
        return;

    case 15:
        if (UNITSND_GetSoundClassId(pUnit) == MONSTER_ROGUEHIRE)
            SOUNDINST_PlaySound(SOUND_ROGUE_CONFIRM_A_1, pUnit, 0, 0, 0);
        return;

    case 16:
        if (UNITSND_GetSoundUnitType(pUnit) != UNIT_MONSTER)
            return;
        pMonSoundsTxt = UNITSND_GetMonSoundsRecordForUnit(pUnit);
        if (pMonSoundsTxt)
            SOUNDINST_PlaySound(pMonSoundsTxt->dwTaunt, pUnit, 0, 0, 0);
        else
            SOUNDINST_PlaySound(SOUND_NONE, pUnit, 0, 0, 0);
        return;

    case 17:
        if (UNITSND_GetSoundUnitType(pUnit) != UNIT_MONSTER)
            return;
        pMonSoundsTxt = UNITSND_GetMonSoundsRecordForUnit(pUnit);
        if (!pMonSoundsTxt)
            return;
        if (GAME_GetFrameCount() - gnLastMonsterVoiceFrame < 4)
            return;
        SOUNDINST_PlaySound(pMonSoundsTxt->dwFlee, pUnit, SOUNDSONG_RollRandom(3) + 6, 0, 0);
        pUnit->dwSfxTicks = GAME_GetFrameCount();
        gnLastMonsterVoiceFrame = GAME_GetFrameCount();
        return;

    case 18:
        //  The player is fetched first, then the speech, as in the original.
        pPlayer = CUNIT_GetPlayer();
        SOUNDINST_PlaySound(OBJECT_PickNpcSpeechSound(pUnit, 1), pPlayer, 0, 1, 0);
        return;

    case 84:
        nVoiceSound = UNITSND_GetSoundClassId(pUnit) == MONSTER_ROGUEHIRE ? SOUND_HIREABLE_FEMALE_THANK_YOU : SOUND_HIREABLE_MALE_THANKS_1;
        SOUNDINST_PlaySound(nVoiceSound, pUnit, 0, 0, 0);
        TEXT_ChatShowSpeechSubtitle(pUnit, nVoiceSound);
        break;

    case 85:
        nVoiceSound = UNITSND_GetSoundClassId(pUnit) == MONSTER_ROGUEHIRE ? SOUND_HIREABLE_FEM_CANT_USE_THAT_EVER : SOUND_HIRE_MALE_CANT_USE_THAT_EVER_1;
        SOUNDINST_PlaySound(nVoiceSound, pUnit, 0, 0, 0);
        TEXT_ChatShowSpeechSubtitle(pUnit, nVoiceSound);
        break;

    case 86:
        nVoiceSound = UNITSND_GetSoundClassId(pUnit) == MONSTER_ROGUEHIRE ? SOUND_HIREABLE_FEM_CANT_USE_THAT_YET : SOUND_HIRE_MALE_CANT_USE_THAT_YET_1;
        SOUNDINST_PlaySound(nVoiceSound, pUnit, 0, 0, 0);
        TEXT_ChatShowSpeechSubtitle(pUnit, nVoiceSound);
        break;

    case 87:
        nVoiceSound = UNITSND_GetSoundClassId(pUnit) == MONSTER_ROGUEHIRE ? SOUND_HIREABLE_FEMALE_I_WILL_USE_THAT : SOUND_HIREABLE_MALE_I_WILL_USE_THAT_1;
        SOUNDINST_PlaySound(nVoiceSound, pUnit, 0, 0, 0);
        TEXT_ChatShowSpeechSubtitle(pUnit, nVoiceSound);
        break;

    case 90:
        SOUNDINST_PlaySound(SOUND_ANCIENT_ACT5_REWARD_SPIEL, pUnit, 0, 0, 0);
        return;

    case 91:
        SOUNDINST_PlaySound(SOUND_CURSOR_LEVEL_UP_HIRELING, CUNIT_GetPlayer(), 0, 0, 0);
        return;

    case 92:
        SOUNDSONG_ScheduleSting2(25, 1);
        return;

    case 93:
        SOUNDINST_PlaySound(SOUND_OBJECT_CORPSE_LOOT, pUnit, 0, 0, 0);
        return;

    default:
        break;
    }

    if (pUnit->dwUnitType == UNIT_PLAYER)
        UNITSND_PlayPlayerSoundEvent(pUnit, nSoundEvent);
}


//1.10f: D2Client.0x6FB51F00
//  Adds nDelta to the reference count of the sounds a unit can make (SOUNDHDR_AdjustRefCount). The unit is asserted, then bHasActiveSound (+88h)
//  makes it once each way: set, a positive delta returns; clear, a negative one returns; otherwise it becomes nDelta > 0 -- a zero delta clears it
//  and counts nothing. A monster's six monsounds.txt sounds -- Attack1, Attack2, HitSound, DeathSound, Flee, FootstepLayer, in that order -- or an
//  object's eight mode sounds; another type, nothing.
//  An object's class is bounded by 573 (unsigned) under an assert of its own, "eUnitClass < NUM_OBJECTS".
//  The original inlines UNITSND_GetSoundUnitType, UNITSND_GetSoundClassId and that object class bound assert here.
void __fastcall UNITSND_AdjustUnitSoundRefCounts(D2UnitStrc *pUnit, int32_t nDelta)
{
    int32_t nType;
    D2MonSoundsTxt *pMonSoundsTxt;
    uint32_t nObjectClass;
    D2ClientObjectModeSoundStrc *pObjectSounds;
    int32_t i;

    nType = UNITSND_GetSoundUnitType(pUnit);
    if (!pUnit)
    {
        FOG_DisplayAssert("ptUnit", szUnitSndCpp, 3325);
        exit(-1);
    }

    if (pUnit->bHasActiveSound)
    {
        if (nDelta > 0)
            return;
    }
    else if (nDelta < 0)
        return;
    pUnit->bHasActiveSound = nDelta > 0;

    if (nType == UNIT_MONSTER)
    {
        pMonSoundsTxt = UNITSND_GetMonSoundsRecordForUnit(pUnit);
        if (!pMonSoundsTxt)
            return;
        SOUNDHDR_AdjustRefCount(pMonSoundsTxt->dwAttack1, nDelta);
        SOUNDHDR_AdjustRefCount(pMonSoundsTxt->dwAttack2, nDelta);
        SOUNDHDR_AdjustRefCount(pMonSoundsTxt->dwHitSound, nDelta);
        SOUNDHDR_AdjustRefCount(pMonSoundsTxt->dwDeathSound, nDelta);
        SOUNDHDR_AdjustRefCount(pMonSoundsTxt->dwFlee, nDelta);
        SOUNDHDR_AdjustRefCount(pMonSoundsTxt->dwFootstepLayer, nDelta);
    }
    else if (nType == UNIT_OBJECT)
    {
        nObjectClass = UNITSND_GetSoundClassId(pUnit);
        if (nObjectClass >= 573)
        {
            FOG_DisplayAssert("eUnitClass < NUM_OBJECTS", szUnitSndCpp, 4627);
            exit(-1);
        }
        pObjectSounds = gpObjectModeSounds[nObjectClass];
        if (pObjectSounds)
        {
            for (i = 0; i < 8; ++i)
                SOUNDHDR_AdjustRefCount(pObjectSounds->nModeSounds[i], nDelta);
        }
    }
}


//1.10f: D2Client.0x6FB520A0
//  Plays the player's line on entering an area, from the music's tick (SOUNDSONG_UpdateMusic). The local player's level, once per change of level
//  (gnLastLevelIdForSound) and only below levels.txt's count (signed compare); a level already played (gbLevelSoundPlayed) says nothing. Otherwise
//  the area-entry table is walked: gpAreaEntryQuestLines, pointers to fourteen D2AreaEntryQuestLineStrc records, a NULL pointer ending the walk.
//  A record whose level ids (up to ten, 0-terminated) include the player's level marks all of its levels played; then, if its quest is 0 or
//  QUEST_ShouldPlayAreaEntrySpiel lets it, the player's dwSfxTicks is more than 62 frames old (unsigned) and no speech plays anywhere
//  (SOUNDINST_IsAnySpeechActive), the record's event (a word) goes to UNITSND_PlayPlayerSoundEvent.
//  The fourteen, event <- level ids: 38 burial <- 17; 40 catacombs <- 34..37; 41 den <- 8; 42 jail <- 29..31; 43 monastery <- 26, 27; 44 tower <-
//  20..25; 46 tristram <- 38; 47 wilderness <- 2..7; 55 arcane <- 74; 56 clawviper <- 58; 76 act5 wilderness <- 110; 78 mountaintop <- 120; 77
//  nihlathak <- 121; 79 worldstone <- 132; slots 14 and 15 are NULL.
//  BUG: the walk's bound is 0x6FB96538, twenty pointers past the table's head where the table is sixteen: the four slots past it are the first
//  sixteen bytes of an assert string. The bound is compared as the original compares it, signed.
//  UNREACHABLE: the NULL in slot 14 ends the walk first.
//  The original inlines UNITSND_GetSfxTicks here.
void __cdecl UNITSND_PlayAreaEntryQuestLine(void)
{
    D2UnitStrc *pPlayer;
    D2ActiveRoomStrc *pRoom;
    int32_t nLevelId;
    int32_t bLevelPlayed;
    D2AreaEntryQuestLineStrc **ppQuestLine;
    D2AreaEntryQuestLineStrc *pQuestLine;
    BOOL bMatch;
    int32_t nLevelIdx;
    int32_t nGroupLevelId;
    int32_t iMark;
    int32_t nMarkLevelId;
    uint32_t nSfxTicks;

    pPlayer = CUNIT_GetPlayer();
    pRoom = D2COMMON_UNITS_GetRoom(pPlayer);
    if (!pRoom)
        return;

    nLevelId = D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom);
    if (nLevelId >= sgptDataTables->nLevelsTxtRecordCount || nLevelId == gnLastLevelIdForSound)
        return;
    bLevelPlayed = gbLevelSoundPlayed[nLevelId];
    gnLastLevelIdForSound = nLevelId;
    if (bLevelPlayed)
        return;

    ppQuestLine = gpAreaEntryQuestLines;
    for (;;)
    {
        pQuestLine = *ppQuestLine;
        if (!pQuestLine)
            return;

        bMatch = FALSE;
        for (nLevelIdx = 0; nLevelIdx < 10; ++nLevelIdx)
        {
            nGroupLevelId = pQuestLine->nLevelIds[nLevelIdx];
            if (!nGroupLevelId)
                break;
            if (nGroupLevelId == nLevelId)
            {
                bMatch = TRUE;
                break;
            }
        }
        if (bMatch)
            break;

        ++ppQuestLine;
        //  Twenty slots from the table's head, four past its end; slot 14's NULL always ends the walk before this bound is reached.
        if ((int32_t)ppQuestLine >= (int32_t)gpAreaEntryQuestLines + 20 * (int32_t)sizeof(D2AreaEntryQuestLineStrc *))
            return;
    }

    for (iMark = 0; iMark < 10; ++iMark)
    {
        nMarkLevelId = pQuestLine->nLevelIds[iMark];
        if (!nMarkLevelId)
            break;
        gbLevelSoundPlayed[nMarkLevelId] = 1;
    }

    if (pQuestLine->nQuestId && !QUEST_ShouldPlayAreaEntrySpiel(pQuestLine->nQuestId))
        return;

    nSfxTicks = UNITSND_GetSfxTicks(pPlayer);
    if (GAME_GetFrameCount() - nSfxTicks <= 62)
        return;
    if (SOUNDINST_IsAnySpeechActive())
        return;

    UNITSND_PlayPlayerSoundEvent(pPlayer, pQuestLine->nSoundEvent);
}


//1.10f: D2Client.0x6FB52190
//  Plays a monster's greeting when the client creates it (CUNIT_CreateMonsterUnit): its monsounds.txt Init, once -- not for a dead monster, not
//  with no Init, and not when dwSfxAsyncTicks is already set. Played, it stamps gnLastNeutralSoundFrame and dwSfxAsyncTicks with the frame taken
//  before the row was looked up and re-rolls gnNeutralSoundReArmDelay to 30..90 frames (SOUNDSONG_RollRandomInRange), as an idle voice does
//  (UNITSND_UpdateUnitModeSound).
//  Otherwise -- a monster with a row and no greeting to give -- a monster in MONMODE_SKILL1 plays its skill sound (UNITSND_PlayMonsterSkillSound,
//  mode 8). A non-monster or no row, nothing. The dwSfxAsyncTicks store is a plain store, with no assert.
//  The original inlines UNITSND_GetSoundUnitType, UNITSND_GetSfxAsyncTicks and the sound code's mode read here.
void __fastcall UNITSND_PlayMonsterInitSound(D2UnitStrc *pUnit)
{
    int32_t nFrame;
    D2MonSoundsTxt *pMonSoundsTxt;
    D2C_Sounds nInitSound;
    int32_t nSoundType;
    int32_t nSoundClass;
    int32_t nSoundMode;

    if (UNITSND_GetSoundUnitType(pUnit) != UNIT_MONSTER)
        return;

    nFrame = GAME_GetFrameCount();
    pMonSoundsTxt = UNITSND_GetMonSoundsRecordForUnit(pUnit);
    if (!pMonSoundsTxt)
        return;

    nInitSound = pMonSoundsTxt->dwInit;
    if (nInitSound && !CUNIT_IsUnitDead(pUnit) && !UNITSND_GetSfxAsyncTicks(pUnit))
    {
        SOUNDINST_PlaySound(nInitSound, pUnit, 0, 0, 0);
        gnLastNeutralSoundFrame = nFrame;
        pUnit->dwSfxAsyncTicks = nFrame;
        gnNeutralSoundReArmDelay = SOUNDSONG_RollRandomInRange(30, 90);
        return;
    }

    nSoundType = pUnit ? (int32_t)pUnit->dwUnitType : (int32_t)UNIT_TYPES_COUNT;
    nSoundClass = pUnit ? (int32_t)pUnit->dwClassId : -1;
    nSoundMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
    if (!nSoundType)
        D2COMMON_11013_ConvertMode(pUnit, &nSoundType, &nSoundClass, &nSoundMode, szUnitSndCpp, 2841);
    if (nSoundMode == MONMODE_SKILL1)
        UNITSND_PlayMonsterSkillSound(pUnit, MONMODE_SKILL1);
}

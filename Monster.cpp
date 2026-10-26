#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Game.h"
#include "Monster.h"

//  Monster.cpp -- the monsters of D2Client.dll 1.10: the client's construction, update and free of a monster unit (MONSTER_InitClientUnit,
//  MONSTER_DoClientAction, the animation advance for a mode), the critters' AI, the player-slot table, the type-flag and monstats accessors
//  and the hireling. Address range 0x6FB3F270..0x6FB44A4D. The original's path literal is UNIT\Monster.cpp.

//  The unit's source path, as its asserts and __FILE__ arguments pass it.
static const char szMonsterCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\UNIT\\Monster.cpp";

static_assert(offsetof(D2MonsterDataStrc, pMonstatsTxt) == 0
                  && offsetof(D2MonsterDataStrc, pAiControl) == 0x28
                  && offsetof(D2MonsterDataStrc, nCritterIdleTicks) == 0x30
                  && offsetof(D2MonsterDataStrc, bCritterLatch) == 0x34
                  && offsetof(D2MonsterDataStrc, unk0x3C) == 0x3C
                  && offsetof(D2UnkMonsterDataStrc, unk0x00) == 0,
              "MONSTER_BindMonStatsRecord: `mov [esi], eax` (0x6FB40ABF) "
              "and `mov [esi+28h], dx` (0x6FB40A91); `lea eax, [ecx+30h]` "
              "(0x6FB40AE3), `lea eax, [ecx+34h]` (0x6FB40B43), `mov eax, "
              "[ecx+3Ch]` (0x6FB40B04), `mov [ecx+3Ch], edx` (0x6FB40B20)");
static_assert(offsetof(D2MonStatsTxt, wMonSound) == 0x14
                  && offsetof(D2MonStatsTxt, nSkill) == 0x170
                  && sizeof(((D2MonStatsTxt *)0)->nSkill) == 8 * 2
                  && offsetof(D2MonSoundsTxt, dwSkill) == 0x44
                  && sizeof(((D2MonSoundsTxt *)0)->dwSkill) == 4 * 4,
              "MONSTER_GetSkillSound: `movsx eax, word ptr [esi+14h]` "
              "(0x6FB40A1B), `movsx ebx, word ptr [esi+ecx*2+170h]` "
              "(0x6FB40A31) under `cmp ecx, 8` (0x6FB40A5C), the four cases "
              "at +44h..+50h (0x6FB40A49..0x6FB40A58)");
static_assert(offsetof(D2MonStats2Txt, nLight_R) == 0x11F
                  && offsetof(D2MonStats2Txt, nLight_G) == 0x120
                  && offsetof(D2MonStats2Txt, nLight_B) == 0x121
                  && LEVEL_DENOFEVIL == 8 && QUEST_A1Q1_DENOFEVIL == 1
                  && MONALIGN_GOOD == 1 && MONALIGN_NEUTRAL == 2,
              "MONSTER_UpdateClientLight: `mov cl, [esi+11Fh]`, `mov al, "
              "[esi+120h]`, `mov dl, [esi+121h]` (0x6FB40C68, 0x6FB40C5E, "
              "0x6FB40C58); `cmp eax, 8` (0x6FB40C1E); `push 1` (0x6FB40C23); "
              "`cmp al, 1` / `cmp al, 2` (0x6FB40C45, 0x6FB40C49)");
static_assert(offsetof(D2MonStatsTxt, nResistances) == 0x144
                  && sizeof(((D2MonStatsTxt *)0)->nResistances[0]) == 3 * 2
                  && offsetof(D2MonStatsTxt, nSkillMode) == 0x180
                  && offsetof(D2MonStatsTxt, nSkLvl) == 0x198
                  && offsetof(D2DifficultyLevelsTxt, dwMonsterSkillBonus) == 0x10
                  && STAT_DAMAGERESIST == 0x24 && NUM_DIFFICULTIES == 3,
              "MONSTER_InitClientUnit: `movsx eax, word ptr [edi+ebp*2+144h]` (0x6FB40E50) and "
              "the next five rows 6 bytes apart (+14Ah..+162h), `push 24h` (0x6FB40E5B); `movsx "
              "edx, byte ptr [edi+ebp+180h]` (0x6FB41111); `mov al, [edi+ebp+198h]` (0x6FB410EA); "
              "`mov edx, [eax+10h]` (0x6FB410D3); `cmp ebp, 3` (0x6FB40DEA)");
static_assert(MONSTATS2FLAGINDEX_ISSEL == 3 && MONSTATS2FLAGINDEX_SHADOW == 14
                  && MONSTATS2FLAGINDEX_DEADCOL == 19
                  && MONSTER_MUMMY1 == 96 && MONSTER_VILEMOTHER1 == 298 && MONSTER_VILECHILD1 == 301
                  && MONSTER_IRONGOLEM == 291 && MONSTER_VALKYRIE == 357
                  && MONSTER_SHADOWWARRIOR == 417 && MONSTER_SHADOWMASTER == 418
                  && MONSTER_HYDRA1 == 351 && MONSTER_HYDRA2 == 352 && MONSTER_HYDRA3 == 353
                  && MONSTER_BONEWALL == 344,
              "MONSTER_InitClientUnit: [ecx+4] against mask 3 (0x6FB41007), [ecx+5] against mask 6 "
              "(0x6FB41027), [edx+6] against mask 3 (0x6FB40F88); `cmp ebx, 60h` and `12Dh` "
              "(0x6FB4124C, 0x6FB41258); 123h, 165h, 1A1h, 1A2h (0x6FB40F42..0x6FB40F57); 15Fh, "
              "160h, 161h, 165h, 158h (0x6FB41278..0x6FB412A2); MONSTER_MatchVileMotherDirection's "
              "`cmp eax, 12Ah` (0x6FB416FA)");
static_assert(MONSTER_KASHYA == 150 && MONSTER_BIRD1 == 157 && MONSTER_BIRD2 == 158
                  && MONSTER_PARROT == 319 && MONSTER_BLADECREEPER == 413
                  && MONSTER_CATAPULT1 == 497 && MONSTER_CATAPULT2 == 498
                  && MONSTER_CATAPULT3 == 499 && MONSTER_CATAPULT4 == 500 && MONSTER_BAALCLONE == 570
                  && MISSILE_BLADE_CREEPER == 392 && MISSILE_BAALTELEPORT == 613 && COLLIDE_NONE == 0,
              "MONSTER_LinkOwnedUnits: the jump tables' cases 150, 157, 158, 319 (0x6FB412FB) and "
              "413, 497..500, 570 (0x6FB4139E); `mov edx, 188h` (0x6FB41493), `mov edx, 265h` "
              "(0x6FB41509); the birds' `push 0` (0x6FB4132C)");
static_assert(offsetof(D2ClientMonModeDescStrc, nPathStep) == 0
                  && offsetof(D2ClientMonModeDescStrc, nAnimEnd) == 4
                  && offsetof(D2ClientMonModeDescStrc, nSplEndTrigger) == 8
                  && offsetof(D2MonStats2Txt, nModeEnabledWhenMovingFlags) == 0x104
                  && offsetof(D2MonStats2Txt, nInfernoRollback) == 0x10A
                  && offsetof(D2MonStatsTxt, nSplEndDeath) == 0x1A4,
              "MONSTER_UpdateAnimMode and AdvanceAnimForMode: `mov eax, [ecx]` (0x6FB42170), `mov eax, "
              "[ebx+4]` (0x6FB41DFF), `mov ecx, [ebx+8]` (0x6FB4233C); `mov dl, [ecx+eax+104h]` "
              "(0x6FB41F79), `mov dl, [edi+10Ah]` (0x6FB420E2); `mov cl, [eax+1A4h]` (0x6FB41DC5)");
static_assert(MONMODE_SKILL3 == 10 && MONSTATS2FLAGINDEX_CRITTER == 13 && STATE_FREEZE == 1
                  && UNITFLAG_DOUPDATE == 1 && UNITFLAG_ISMERC == 0x200
                  && MONSTER_ANDARIEL == 156 && MONSTER_MEPHISTO == 242 && MONSTER_DIABLO == 243
                  && MONSTER_DIABLOCLONE == 333 && MONSTER_EVILHUT == 528 && MONSTER_BAALCRAB == 544
                  && OBJECT_CLIENTSMOKE == 478 && OVERLAY_ICE_CAGE == 227 && OVERLAYTYPE_STOP == 5
                  && NUMBER_OF_MONMODES == 16 && DIFFMODE_NORMAL == 0 && DIFFMODE_NIGHTMARE == 1
                  && DIFFMODE_HELL == 2,
              "MONSTER_UpdateAnimMode: `cmp edi, 10h` (0x6FB41EC9, 0x6FB41EDF), `cmp edi, 0Ah` "
              "(0x6FB41F27), [eax+5] against mask 5 (0x6FB4223B), "
              "`push eax` with eax 1 (0x6FB41EAC), `and ecx, 1` (0x6FB423AD), `shr eax, 9` (0x6FB4225B), "
              "`cmp ecx, 0F3h` and `14Dh` (0x6FB41EEB, 0x6FB41F05), `cmp ecx, 220h` (0x6FB41F39), `cmp "
              "dword ptr [esi+4], 210h` (0x6FB4239C), `mov ecx, 1DEh` (0x6FB423D3); "
              "MONSTER_ChangeMonsterClass: `sub eax, 9Ch` (0x6FB41CE7), `cmp eax, 0F2h` (0x6FB41CE0), "
              "`mov edx, 0E3h` (0x6FB41D30), `push 5` (0x6FB41D2E), the difficulty's `sub eax, 0`, "
              "`dec eax`, `dec eax` (0x6FB41CFD, 0x6FB41D02, 0x6FB41D05)");
static_assert(offsetof(D2MonStatsTxt, wAiParam) == 0x56 && offsetof(D2MonStatsTxt, nSplClientEnd) == 0x1A7
                  && offsetof(D2MonStats2Txt, nBleed) == 0x11D && offsetof(D2SeedStrc, nHighSeed) == 4,
              "MONSTER_SpawnAttackMissile: wAiParam[2] at `movsx eax, word ptr [ecx+eax*2+62h]` "
              "(0x6FB426F7), the seed's high half at +4 (0x6FB426E5); MONSTER_DoClientSplEndEffect: "
              "`mov cl, [eax+1A7h]` (0x6FB42846); MONSTER_SpawnBlood: `mov cl, [eax+11Dh]` (0x6FB42B6D)");
static_assert(MISSILE_MUMMY1 == 63 && MISSILE_MUMMY4 == 66 && MISSILE_PANTHERPOTORANGE == 238
                  && MISSILE_PANTHERPOTGREEN == 239 && MISSILE_CATAPULTCHARGEDBALL_ON == 406
                  && MISSILE_BLOOD1 == 18 && MONSTER_QUILLRAT1 == 63 && STAT_ITEM_EXTRABLOOD == 0x8C
                  && MONSTER_ICEGLOBE == 231 && MONSTER_WILLOWISP1 == 118 && MONSTER_TRAPPEDSOUL1 == 403
                  && MONSTER_TRAPPEDSOUL2 == 404 && MONSTER_VULTURE1 == 110 && MONSTER_BATDEMON1 == 136
                  && MONSTER_MAGGOTQUEEN1 == 284 && MONSTER_FROGDEMON1 == 247 && MONMODE_ATTACK1 == 4
                  && MONMODE_SKILL2 == 9 && UNITFLAGEX_UNK_PATH_RELATED == 0x2000,
              "MONSTER_SpawnAttackMissile: `cmp ebp, 3Fh` and `42h` (0x6FB4245D, 0x6FB4246C), `0EEh`, `0EFh`, "
              "`196h` (0x6FB42517, 0x6FB42527, 0x6FB42533), `cmp eax, 3Fh` (0x6FB42689); MONSTER_SpawnBlood: "
              "`add edx, 12h` (0x6FB42D0C), `push 8Ch` (0x6FB42AE8); MONSTER_DoClientSplEndEffect: `cmp edi, "
              "0E7h`, `76h` (0x6FB428CC, 0x6FB428D8), `193h`, `194h` (0x6FB42873, 0x6FB4287B), `6Eh` "
              "(0x6FB4286A), `88h` (0x6FB4289E), `11Ch` (0x6FB429EF), `0F7h` (0x6FB429E7), the jump table's "
              "cases 4 and 9 after `sub eax, 2` (0x6FB42857), `shr ecx, 0Dh` (0x6FB42A0F)");
static_assert(offsetof(D2MonStatsTxt, wMinion) == 0x26
                  && CLTACTION_MONCAST_XY == 4 && CLTACTION_MONCAST_UNIT == 5 && CLTACTION_MONATTACK1_UNIT == 10
                  && CLTACTION_MONATTACK1_XY == 11 && CLTACTION_MONSKILL1_XY == 12 && CLTACTION_MONSKILL1_UNIT == 13
                  && CLTACTION_MONSKILL2_XY == 14 && CLTACTION_MONSKILL2_UNIT == 15 && CLTACTION_MONATTACK2_UNIT == 16
                  && CLTACTION_MONATTACK2_XY == 17 && CLTACTION_MONSKILL3_XY == 26 && CLTACTION_MONSKILL3_UNIT == 27
                  && CLTACTION_MONSKILL4_XY == 28 && CLTACTION_MONSKILL4_UNIT == 29,
              "MONSTER_DoClientAction: `mov ax, [ebp+26h]` (0x6FB4371D); the jump table's cases 5, 10, 13, 15, 16, "
              "27, 29 (0x6FB42FD3) and 4, 11, 12, 14, 17, 26, 28 (0x6FB431B6); `cmp ebx, 0Ah`, `0Dh`, `0Fh` "
              "(0x6FB4303A, 0x6FB43079, 0x6FB430AC)");
static_assert(MONSTER_FINGERMAGE1 == 304 && MONSTER_WARRIV1 == 155 && MONSTER_DOPPLEZON == 356
                  && MONSTER_BARRICADETOWER == 435 && MONSTER_BAALCRABSTAIRS == 559 && SKILL_QUEENDEATH == 216
                  && MONSTER_THORNHULK1 == 127 && MONSTER_MAGGOTEGG1 == 190 && MONSTER_BONEFETISH1 == 212
                  && MONSTER_CANTOR1 == 238 && MONSTER_TENTACLE1 == 258 && MONSTER_TENTACLEHEAD1 == 261
                  && MONSTER_DOOMKNIGHT1 == 310 && MONSTER_DOOMKNIGHT2 == 311 && MONSTER_DOOMKNIGHT3 == 312
                  && MONSTER_MEGADEMON1 == 360 && MONSTER_VENOMLORD == 558 && MONSTER_PLAGUEPOPPY == 425
                  && MONSTER_CYCLEOFLIFE == 426 && MONSTER_VINECREATURE == 427 && MONSTER_MINION1 == 453
                  && MONSTER_SUICIDEMINION1 == 461 && MONSTER_SUCCUBUS1 == 469 && MONSTER_SUCCUBUSWITCH1 == 474,
              "MONSTER_DoClientAction: `cmp [esp+0A4h+pEvent], 130h` (0x6FB43030), `cmp eax, 9Bh` (0x6FB430C0), "
              "`cmp eax, 164h` (0x6FB437C8), `cmp ebx, 1B3h` (0x6FB43C49), `sub eax, 0D7h` after `sub eax, 158h` "
              "(0x6FB43CCE), `mov dword ptr [esp+0A8h+var_78], 0D8h` (0x6FB438C2); the jump tables' cases 127, 190, "
              "212, 238, 258, 261, 310..312 (0x6FB437EF) and 360, 425..427, 453, 461, 469, 474, 558, 559 "
              "(0x6FB43935)");
static_assert(MISSILE_VINE_BEAST_DEATH == 470 && MISSILE_BAALCLONEDEATH == 614 && MISSILE_TOWER_DEATH == 424
                  && MISSILE_DOPPLEZONEXPLODE == 328 && MISSILE_ICEBREAKSMOKE == 275 && MISSILE_ICEBREAKSMALL - 1 == 271
                  && OVERLAY_FINGERMAGEFLAMES == 169 && OVERLAY_VULTUREDEMONFEATHERS == 146
                  && OVERLAY_WILLOWISPDEATH == 184 && OVERLAY_THORNEDHULK_DEATH == 79 && OVERLAY_FETISHDEATH1 == 188
                  && OVERLAY_FETISHDEATH2 == 189 && OVERLAY_ZAKARUMPRIEST_DEATH == 78 && OVERLAY_FROGDEMON_DEATH == 82
                  && OVERLAY_TENTACLEHEADBLOODRIPPLE == 157 && OVERLAY_UNDEADHORRORDEATH == 187
                  && OVERLAY_MEGADEMONDEATH == 185 && OVERLAY_SHADOWWARRIORDEATH == 271
                  && OVERLAY_SUCCUBAE_DEATH == 229 && OVERLAY_CATAPULT_DEATH_S == 225 && OVERLAY_CATAPULT_DEATH_E == 226
                  && OVERLAY_SUICIDE_DEATH == 204 && (int32_t)MONSTER_HYDRA1 - (int32_t)OVERLAY_HYDRA_END1 == 0xAD
                  && (int32_t)MONSTER_HYDRA2 - (int32_t)OVERLAY_HYDRA_END2 == 0xAD
                  && (int32_t)MONSTER_HYDRA3 - (int32_t)OVERLAY_HYDRA_END3 == 0xAD,
              "MONSTER_DoClientAction: `mov edx, 1D6h`, `266h`, `1A8h`, `148h` (0x6FB43A58, 0x6FB43B3C, 0x6FB43CAD, "
              "0x6FB4390C), `mov [esp+0A4h+var_4C], 113h` (0x6FB43698), `add edx, 10Fh` (0x6FB43667); `mov edx, "
              "0A9h`, `92h`, `0B8h`, `4Fh`, `0BCh`, `0BDh`, `4Eh`, `52h`, `9Dh`, `0BBh`, `0B9h`, `10Fh`, `0E5h`, "
              "`0E1h`, `0E2h`, `0CCh` (0x6FB43046, 0x6FB43837, 0x6FB438DC, 0x6FB437F6, 0x6FB438F2, 0x6FB438FE, "
              "0x6FB43821, 0x6FB4382B, 0x6FB4388A, 0x6FB438E6, 0x6FB4393C, 0x6FB43B6F, 0x6FB43AAC, 0x6FB43A98, "
              "0x6FB43AA2, 0x6FB439D9); `lea edx, [eax-0ADh]` (0x6FB4389E)");
static_assert(MONSTATSFLAGINDEX_ZOO == 22 && MONSTER_BAT == 159 && MONSTER_MAGGOT == 227 && MONSTER_BUG == 268
                  && MONSTER_SCORPION == 269 && MONSTER_FETISHSHAMAN1 == 278 && MONSTER_FETISHSHAMAN2 == 279
                  && MONSTER_FETISHSHAMAN3 == 280 && MONSTER_FETISHSHAMAN4 == 281 && MONSTER_FETISHSHAMAN5 == 282
                  && MONSTER_LARVA == 283 && MONSTER_SNAKE == 318 && MONSTER_FISH == 320 && MONSTER_BUNNY == 556
                  && MONSTER_WORLDSTONEEFFECT == 574,
              "MONSTER_CritterTickAmbient: `mov dl, [eax+0Eh]` and `and edx, [eax+18h]` on gdwBitMasks (0x6FB3F486, "
              "0x6FB3F48E); the byte table's cases 159, 227, 268, 269 (0x6FB3F571); `cmp eax, 116h` and `11Ah` "
              "(0x6FB3F550, 0x6FB3F545); `sub eax, 11Bh`, `23h`, `dec eax` (0x6FB3F6B2, 0x6FB3F6B9, 0x6FB3F6BE), "
              "`cmp eax, 140h` (0x6FB3F6A5); `sub eax, 153h`, `0D9h`, `12h` (0x6FB3F6E7, 0x6FB3F6EE, 0x6FB3F6F5); "
              "MONSTER_CritterFindNearestThreatCallback: `cmp eax, 22Ch` (0x6FB3F7F3)");
static_assert(MISSILE_WORLD_STONE_CHIP_1 == 570 && MISSILE_WORLD_STONE_CHIP_4 == 573,
              "MONSTER_EnvTickWorldstoneChips: `and eax, 3` / `add eax, 23Ah` (0x6FB407F2, 0x6FB407F5) -- the four chips");


//1.10f: D2Client.0x6FB3F270
//  Visits the units near pUnit until one answers. The unit's room (D2Common's UNITS_GetRoom; none answers 0), then every room of D2Common's
//  adjacent-room list for it (the room's own ppRoomList and nNumRooms), and in each the units from pUnitFirst by pRoomNext. Each is handed to
//  the callback as pfnVisit(pUnit, pOther, pCallbackCtx, pCallbackParam) and its first non-zero answer is returned at once -- a find more than
//  a for-each. 0 when nothing answers. The room count is compared unsigned.
//  __fastcall over `retn 8`: pUnit in ecx, pCallbackCtx in edx, pCallbackParam and the callback on the stack. The original inlines this walk in
//  MONSTER_CritterTickCrawler and MONSTER_CritterTickBunny.
int32_t __fastcall MONSTER_ForEachUnitNearby(D2UnitStrc *pUnit, void *pCallbackCtx, void *pCallbackParam, int32_t (__fastcall *pfnVisit)(D2UnitStrc *, D2UnitStrc *, void *, void *))
{
    D2ActiveRoomStrc *pRoom;
    D2ActiveRoomStrc **ppRoomList;
    int32_t nNumRooms;
    uint32_t i;
    D2UnitStrc *pOther;
    int32_t nAnswer;

    pRoom = D2COMMON_UNITS_GetRoom(pUnit);
    if (!pRoom)
        return 0;

    D2COMMON_DUNGEON_GetAdjacentRoomsListFromRoom(pRoom, &ppRoomList, &nNumRooms);
    for (i = 0; i < (uint32_t)nNumRooms; ++i)
    {
        //  BUG: a list entry is read with no NULL test. UNREACHABLE: D2Common's DRLGROOM_ReorderNearRoomList stores only the near rooms that
        //  are active and counts what it stores, so the first nNumRooms entries are never NULL.
        for (pOther = ppRoomList[i]->pUnitFirst; pOther; pOther = pOther->pRoomNext)
        {
            nAnswer = pfnVisit(pUnit, pOther, pCallbackCtx, pCallbackParam);
            if (nAnswer)
                return nAnswer;
        }
    }

    return 0;
}


//1.10f: D2Client.0x6FB3F2F0
//  Puts a player into the first free slot of gapClientPlayers: eight slots, the first that holds 0 takes the unit, and a full table takes
//  nothing and says nothing. The one caller is PLAYER_InitClientUnit. pPlayer in ecx; the bytes walk a pointer and a count, the C keeps the
//  index.
//  void: eax leaves holding the slot's address (or the address one past the table) and the one caller discards it.
void __fastcall MONSTER_RegisterPlayerSlot(D2UnitStrc *pPlayer)
{
    for (int32_t i = 0; i < 8; ++i)
    {
        if (!gapClientPlayers[i])
        {
            gapClientPlayers[i] = pPlayer;
            return;
        }
    }
}


//1.10f: D2Client.0x6FB3F310
//  Takes a player out of gapClientPlayers: the slot that holds it is emptied, and a unit no slot holds changes nothing. The one caller is
//  PLAYER_FreeClientUnit. pPlayer in ecx.
//  void: the slot's address is left in eax and discarded by the one caller.
void __fastcall MONSTER_UnregisterPlayerSlot(D2UnitStrc *pPlayer)
{
    for (int32_t i = 0; i < 8; ++i)
    {
        if (gapClientPlayers[i] == pPlayer)
        {
            gapClientPlayers[i] = NULL;
            return;
        }
    }
}


//1.10f: D2Client.0x6FB3F330
//  Empties the eight player slots, one `rep stosd` (ecx 8) over the whole table; the other two accessors bound the same table with
//  `cmp edx, 8`. The sole caller is GAME_InitGame. 0x20 is eight slots of one pointer each.
static_assert(8 * sizeof(D2UnitStrc *) == 0x20,
              "gapClientPlayers: the rep stosd at 0x6FB3F331 fills 0x20 bytes");

void __cdecl MONSTER_ClearPlayerSlots()
{
    memset(gapClientPlayers, 0, sizeof(gapClientPlayers));
}


//1.10f: D2Client.0x6FB3F350
//  One tick of a client-only monster, the critters' AI. CUNIT_UpdateAllUnits calls it for every monster of the client-id table that survives
//  its update. One argument, in ecx. The original inlines MONSTER_CritterSetCooldown and MONSTER_CritterStandStill here.
//
//  Three gates come first:
//    * the client timer (MONSTER_GetClientTimerPtr, pMonsterData +0x30, nCritterIdleTicks): a monster with no data returns at once; a
//      positive count is decremented and returns (signed compare);
//    * the nearest player among gapClientPlayers' eight slots, by D2Common's UNITS_GetDistanceToOtherUnit (signed compare), and
//      CUNIT_GetPlayer when no slot gives one; with no player at all the timer takes 25 and the critter stands still;
//    * a nearest player more than 30 away (signed compare): the timer takes the distance less 15, capped at 200 by the setter, and the
//      critter stands still. CUNIT_GetPlayer's player comes with no distance (ebx still holds 0x7FFFFFFF), so a critter no slot gives a
//      player always takes this gate: 200 idle ticks, standing still.
//
//  The zoo arm. A class with monstats' `zoo` flag (MONSTATSFLAGINDEX_ZOO, byte 2 bit 6 of the flag word, tested through gdwBitMasks) takes
//  this arm and never the switch; stock monstats.txt sets it on ordinary monsters, none a class the switch names. A dead monster (D2Common's
//  MONSTERS_IsDead) or one not in MONMODE_NEUTRAL does nothing; the mode is read before the call. Then, for a class whose MONMODE_WALK is
//  enabled (MONSTER_IsMonModeEnabled), a roll of 100: 40..59 walks to the nearest player (MONSTER_CritterMoveToUnit, velocity 6), 60..79
//  steps 8 away from it (velocity 4), 80 and up wanders 8 (velocity 4); below 40, and for a class that cannot walk, 20 idle ticks and the
//  critter stands still. The two middle bands test the player for NULL where every path into them has one -- written as the bytes have it.
//
//  The switch on the class is signed, with a byte table for 149..269:
//    chicken, bug                       the threat search, below
//    rat, scorpion, larva, minispider   MONSTER_CritterTickCrawler
//    bird1, bird2, parrot               MONSTER_CritterTickBird
//    bat                                in MONMODE_NEUTRAL: a player 5 or more away (unsigned compare) wanders 9; nearer, a roll of
//                                       100 at 66 or more wanders 12 at velocity 30, below it the bat flees 8 at velocity 30
//    maggot                             MONSTER_CritterTickMaggot
//    fetishshaman1..5                   nothing: `cmp eax, 116h / jge` goes to the return, so the five are cases of their own, not
//                                       the default, which stands the critter still
//    snake                              MONSTER_CritterTickSnake
//    fish                               stands still (a jump into the no-player gate's copy)
//    bunny                              MONSTER_CritterTickBunny
//    worldstoneeffect                   MONSTER_EnvTickWorldstoneChips
//    anything else                      stands still
//  Every Tick* is handed the nearest player and its distance.
//
//  The threat search, chicken and bug, in MONMODE_NEUTRAL: MONSTER_ForEachUnitNearby with MONSTER_CritterFindNearestThreatCallback, the
//  nearest threat and its distance coming back through two locals set to NULL and 0x7FFFFFFF. With one found, the footprint mask becomes
//  COLLIDE_MASK_MONSTER_PATH (0x3C01); at 4 or more (signed compare) a roll of 100 below 30 wanders 2, otherwise 5 idle ticks; nearer, the
//  mask is narrowed to 0x0C01 and the critter flees 4 with CLTACTION_MONSKILL1_XY at velocity 120, wandering 4 the same way when the step
//  is blocked. Nothing here puts the mask back.
void __fastcall MONSTER_CritterTickAmbient(D2UnitStrc *pMonster)
{
    int32_t *pClientTimer;
    int32_t nNearestDist;
    D2UnitStrc *pNearestPlayer;
    D2UnitStrc **ppPlayerSlot;
    int32_t nSlotsLeft;
    uint32_t nPlayerAnimMode;
    int32_t nDistance;
    D2MonStatsTxt *pMonStatsTxt;
    uint32_t dwAnimMode;
    int32_t nRoll100;
    D2UnitStrc *pThreat;
    int32_t nThreatDist;
    D2DynamicPathStrc *pDynamicPath;

    pClientTimer = MONSTER_GetClientTimerPtr(pMonster);
    if (!pClientTimer)
        return;

    if (*pClientTimer > 0)
    {
        --*pClientTimer;
        return;
    }

    nNearestDist = 0x7FFFFFFF;
    pNearestPlayer = NULL;
    ppPlayerSlot = gapClientPlayers;
    nSlotsLeft = 8;
    do
    {
        if (*ppPlayerSlot)
        {
            //  BUG: the player walk skips modes 0 and 12. For a player 0 is PLRMODE_DEATH but 12 is PLRMODE_KICK (the dead are PLRMODE_DEAD,
            //  17; 12 is MONMODE_DEAD, a monster's). REACHABLE: a dead player can be the nearest, and a kicking one is passed over for the
            //  tick; stock Skills.txt gives the kick animation to Kick, Dragon Talon and Dragon Tail.
            nPlayerAnimMode = (*ppPlayerSlot)->dwAnimMode;
            if (nPlayerAnimMode != PLRMODE_DEATH && nPlayerAnimMode != MONMODE_DEAD)
            {
                nDistance = D2COMMON_UNITS_GetDistanceToOtherUnit(pMonster, *ppPlayerSlot);
                if (nDistance < nNearestDist)
                {
                    pNearestPlayer = *ppPlayerSlot;
                    nNearestDist = nDistance;
                }
            }
        }

        ++ppPlayerSlot;
    }
    while (--nSlotsLeft);

    if (!pNearestPlayer)
    {
        pNearestPlayer = CUNIT_GetPlayer();
        if (!pNearestPlayer)
        {
            MONSTER_CritterSetCooldown(pMonster, 25);
            MONSTER_CritterStandStill(pMonster);
            return;
        }
    }

    if (nNearestDist > 30)
    {
        MONSTER_CritterSetCooldown(pMonster, nNearestDist - 15);
        MONSTER_CritterStandStill(pMonster);
        return;
    }

    pMonStatsTxt = DATATBLS_GetMonStatsTxtRecord(pMonster->dwClassId);
    if (pMonStatsTxt && (pMonStatsTxt->nMonStatsFlags[MONSTATSFLAGINDEX_ZOO >> 3] & gdwBitMasks[MONSTATSFLAGINDEX_ZOO & 7]))
    {
        dwAnimMode = pMonster->dwAnimMode;
        if (D2COMMON_MONSTERS_IsDead(pMonster) || dwAnimMode != MONMODE_NEUTRAL)
            return;

        if (MONSTER_IsMonModeEnabled((D2C_MonsterIds)pMonster->dwClassId, MONMODE_WALK))
        {
            nRoll100 = SEED_RollLimitedRandomNumber(&pMonster->pSeed, 100);
            if (nRoll100 >= 40)
            {
                if (nRoll100 < 60 && pNearestPlayer)
                    MONSTER_CritterMoveToUnit(pMonster, pNearestPlayer, CLTACTION_WALKTOUNIT, 6);
                else if (nRoll100 < 80 && pNearestPlayer)
                    MONSTER_CritterFleeFromTarget(pMonster, pNearestPlayer, 8, CLTACTION_WALKTOXY, 4);
                else
                    MONSTER_CritterWanderRandom(pMonster, 8, CLTACTION_WALKTOXY, 4);
                return;
            }
        }

        MONSTER_CritterSetCooldown(pMonster, 20);
        MONSTER_CritterStandStill(pMonster);
        return;
    }

    switch (pMonster->dwClassId)
    {
    case MONSTER_CHICKEN:
    case MONSTER_BUG:
        if (pMonster->dwAnimMode == MONMODE_NEUTRAL)
        {
            pThreat = NULL;
            nThreatDist = 0x7FFFFFFF;
            MONSTER_ForEachUnitNearby(pMonster, &pThreat, &nThreatDist, (int32_t (__fastcall *)(D2UnitStrc *, D2UnitStrc *, void *, void *)) MONSTER_CritterFindNearestThreatCallback);
            if (pThreat)
            {
                pDynamicPath = pMonster->pDynamicPath;
                D2COMMON_PATH_SetFootprintCollisionMask(pDynamicPath, COLLIDE_MASK_MONSTER_PATH);
                if (nThreatDist >= 4)
                {
                    if (SEED_RollLimitedRandomNumber(&pMonster->pSeed, 100) < 30)
                        MONSTER_CritterWanderRandom(pMonster, 2, CLTACTION_WALKTOXY, 0);
                    else
                        MONSTER_CritterSetCooldown(pMonster, 5);
                }
                else
                {
                    D2COMMON_PATH_SetFootprintCollisionMask(pDynamicPath, COLLIDE_MASK_PLAYER_WW);
                    if (!MONSTER_CritterFleeFromTarget(pMonster, pThreat, 4, CLTACTION_MONSKILL1_XY, 120))
                        MONSTER_CritterWanderRandom(pMonster, 4, CLTACTION_MONSKILL1_XY, 120);
                }
            }
        }
        break;

    case MONSTER_RAT:
    case MONSTER_SCORPION:
    case MONSTER_LARVA:
    case MONSTER_MINISPIDER:
        MONSTER_CritterTickCrawler(pMonster, pNearestPlayer, nNearestDist);
        break;

    case MONSTER_BIRD1:
    case MONSTER_BIRD2:
    case MONSTER_PARROT:
        MONSTER_CritterTickBird(pMonster, pNearestPlayer, nNearestDist);
        break;

    case MONSTER_BAT:
        if (pMonster->dwAnimMode == MONMODE_NEUTRAL)
        {
            if ((uint32_t)nNearestDist >= 5)
                MONSTER_CritterWanderRandom(pMonster, 9, CLTACTION_WALKTOXY, 0);
            else if (SEED_RollLimitedRandomNumber(&pMonster->pSeed, 100) >= 66)
                MONSTER_CritterWanderRandom(pMonster, 12, CLTACTION_WALKTOXY, 30);
            else
                MONSTER_CritterFleeFromTarget(pMonster, pNearestPlayer, 8, CLTACTION_WALKTOXY, 30);
        }
        break;

    case MONSTER_MAGGOT:
        MONSTER_CritterTickMaggot(pMonster, pNearestPlayer, nNearestDist);
        break;

    case MONSTER_FETISHSHAMAN1:
    case MONSTER_FETISHSHAMAN2:
    case MONSTER_FETISHSHAMAN3:
    case MONSTER_FETISHSHAMAN4:
    case MONSTER_FETISHSHAMAN5:
        break;

    case MONSTER_SNAKE:
        MONSTER_CritterTickSnake(pMonster, pNearestPlayer, nNearestDist);
        break;

    case MONSTER_FISH:
        MONSTER_CritterStandStill(pMonster);
        break;

    case MONSTER_BUNNY:
        MONSTER_CritterTickBunny(pMonster, pNearestPlayer, nNearestDist);
        break;

    case MONSTER_WORLDSTONEEFFECT:
        MONSTER_EnvTickWorldstoneChips(pMonster, pNearestPlayer, nNearestDist);
        break;

    default:
        MONSTER_CritterStandStill(pMonster);
        break;
    }
}

//1.10f: D2Client.0x6FB3F7D0
//  The threat search's callback: MONSTER_ForEachUnitNearby calls it for every unit in the rooms near the critter and stops at a non-zero
//  answer; this one always answers 0, so every unit is seen. `retn 8`: the walker's own second and third arguments, the addresses of the
//  nearest threat and its distance. The original inlines it, in a copy of the walk, in MONSTER_CritterTickBunny.
//  A candidate counts when it is not the critter, not NULL, and a player or a monster other than the chicken and the bunny -- so the rat,
//  the bird and every other critter count, and the bug, the search's other user, flees from bugs. The candidate nearer than the best so
//  far (signed compare) is kept.
int32_t __fastcall MONSTER_CritterFindNearestThreatCallback(D2UnitStrc *pCritter, D2UnitStrc *pCandidate, D2UnitStrc **ppNearest, int32_t *pnBestDistance)
{
    int32_t nDistance;

    if (pCritter == pCandidate || !pCandidate)
        return 0;

    if (pCandidate->dwUnitType != UNIT_PLAYER && (pCandidate->dwUnitType != UNIT_MONSTER || pCandidate->dwClassId == MONSTER_CHICKEN || pCandidate->dwClassId == MONSTER_BUNNY))
    {
        return 0;
    }

    //  BUG: the dead test is CUNIT_IsUnitDead of the critter, not of the candidate. NOT ESTABLISHED: the one caller runs the search for a
    //  critter in MONMODE_NEUTRAL, so it refuses only one flagged UNITFLAG_ISDEAD, and a dead player or monster counts as a threat; whether
    //  a corpse lies in the rooms around the chicken (Act 1 town) and the bug (Act 2 town) was not measured.
    if (CUNIT_IsUnitDead(pCritter))
        return 0;

    nDistance = D2COMMON_UNITS_GetDistanceToOtherUnit(pCritter, pCandidate);
    if (nDistance < *pnBestDistance)
    {
        *ppNearest = pCandidate;
        *pnBestDistance = nDistance;
    }

    return 0;
}

//1.10f: D2Client.0x6FB3F830
//  Walks a critter to a unit: issues nAction with the block {the target's type, its id, 0, the path's type, nVelocityPercent, the path's
//  maximum distance}, type 6 (UNIT_TYPES_COUNT) and id -1 standing for no target. `retn 8`: nAction and nVelocityPercent on the stack.
//  The block as MONSTER_DoClientAction reads it for WALKTOUNIT, WALKTOXY and MONSKILL1_XY: [0] and [1] the target or the point; [3], above 0,
//  set back as the path's type; [4] the velocity the tail moves STAT_VELOCITYPERCENT to when it is not 0. [2] and [5] are not read for
//  these actions, and [6], the hit class GETHIT, KNOCKBACK and DEATH read, is never written: the frame is seven ints and nothing stores
//  the last. The C leaves it unwritten too, in this function, MONSTER_CritterFleeFromTarget and MONSTER_CritterWanderRandom alike.
void __fastcall MONSTER_CritterMoveToUnit(D2UnitStrc *pUnit, D2UnitStrc *pTarget, D2C_ClientActions nAction, int32_t nVelocityPercent)
{
    D2DynamicPathStrc *pDynamicPath;
    int32_t pParams[7];

    pDynamicPath = pUnit->pDynamicPath;
    pParams[0] = pTarget ? (int32_t)pTarget->dwUnitType : (int32_t)UNIT_TYPES_COUNT;
    pParams[1] = pTarget ? (int32_t)pTarget->dwUnitId : -1;
    pParams[2] = 0;
    pParams[3] = D2COMMON_PATH_GetType(pDynamicPath);
    pParams[4] = nVelocityPercent;
    pParams[5] = D2COMMON_PATH_GetMaxDistance(pDynamicPath);
    MONSTER_DoClientAction(nAction, pUnit, pParams);
}

//1.10f: D2Client.0x6FB3F8A0
//  Steps a critter away from a unit, or answers 0. The direction is a sign per axis (the target's coordinate above the critter's gives -1,
//  below it +1, equal 0), compared unsigned. The point one step that way is tested with D2Common's COLLISION_CheckMaskWithSize, size 1,
//  against the path's own footprint mask; a blocked one answers 0 and issues nothing. Otherwise nAction goes out with the point nDistance
//  steps away (signed multiply) in MONSTER_CritterMoveToUnit's block, and the answer is 1. `retn 0Ch`: nDistance (a byte, `and eax, 0FFh`),
//  nAction and nVelocityPercent. The footprint mask is read before the room and is whatever the caller last set: the chicken's search
//  narrows it to 0x0C01 just before. pTarget is read untested. The original inlines UNITS_GetXPosition here.
int32_t __fastcall MONSTER_CritterFleeFromTarget(D2UnitStrc *pUnit, D2UnitStrc *pTarget, uint8_t nDistance, D2C_ClientActions nAction, int32_t nVelocityPercent)
{
    uint32_t nMyX;
    uint32_t nMyY;
    uint32_t nTargetX;
    uint32_t nTargetY;
    int32_t nStepX;
    int32_t nStepY;
    D2DynamicPathStrc *pDynamicPath;
    int32_t nFootprintMask;
    D2ActiveRoomStrc *pRoom;
    int32_t pParams[7];

    nMyX = UNITS_GetXPosition(pUnit);
    nMyY = UNITS_GetYPosition(pUnit);
    nTargetX = UNITS_GetXPosition(pTarget);
    nTargetY = UNITS_GetYPosition(pTarget);

    nStepY = 0;
    nStepX = 0;
    if (nTargetX > nMyX)
        nStepX = -1;
    if (nTargetX < nMyX)
        nStepX = 1;
    if (nTargetY > nMyY)
        nStepY = -1;
    if (nTargetY < nMyY)
        nStepY = 1;

    //  BUG: the dynamic path is tested for NULL inside the inlined accessors, and the same path then goes untested to D2Common's PATH_GetType
    //  and PATH_GetMaxDistance, which read it at once; likewise in MONSTER_CritterWanderRandom, and in MONSTER_CritterMoveToUnit with no
    //  test at all. UNREACHABLE: MONSTER_InitClientUnit allocates the path for every monster CUNIT_CreateMonsterUnit makes, the server's and
    //  the client-only ones, and only MONSTER_FreeUnitData frees it.
    pDynamicPath = pUnit->pDynamicPath;
    nFootprintMask = D2COMMON_PATH_GetFootprintCollisionMask(pDynamicPath);
    pRoom = D2COMMON_UNITS_GetRoom(pUnit);
    if (D2COMMON_COLLISION_CheckMaskWithSize(pRoom, nMyX + nStepX, nMyY + nStepY, 1, (uint16_t)nFootprintMask))
        return 0;

    pParams[0] = nMyX + nStepX * nDistance;
    pParams[1] = nMyY + nStepY * nDistance;
    pParams[2] = 0;
    pParams[3] = D2COMMON_PATH_GetType(pDynamicPath);
    pParams[4] = nVelocityPercent;
    pParams[5] = D2COMMON_PATH_GetMaxDistance(pDynamicPath);
    MONSTER_DoClientAction(nAction, pUnit, pParams);
    return 1;
}

//1.10f: D2Client.0x6FB3FA50
//  Steps a critter nDistance along both axes, the sign of each a coin: two rolls of the unit's own seed, X first, a set low bit making that
//  step negative -- so, with every caller's distance 2 or more, the point is one of the four diagonals nDistance away. Then nAction goes out
//  with the point in MONSTER_CritterMoveToUnit's block. No collision test and no answer: the move is issued whatever lies there. `retn 8`:
//  nAction and nVelocityPercent; nDistance in dl, a byte (`and eax, 0FFh`). The original inlines UNITS_GetXPosition and
//  SEED_RollRandomNumber here.
//  Each roll's low bit is also stored to a frame slot nothing reads, a variable of the original's source that the C cannot keep.
void __fastcall MONSTER_CritterWanderRandom(D2UnitStrc *pUnit, uint8_t nDistance, D2C_ClientActions nAction, int32_t nVelocityPercent)
{
    int32_t nMyX;
    int32_t nMyY;
    int32_t nStepX;
    int32_t nStepY;
    D2DynamicPathStrc *pDynamicPath;
    int32_t pParams[7];

    nMyX = UNITS_GetXPosition(pUnit);
    nMyY = UNITS_GetYPosition(pUnit);

    nStepX = nDistance;
    nStepY = nDistance;
    if ((uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) & 1)
        nStepX = -nStepX;
    if ((uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) & 1)
        nStepY = -nStepY;

    //  BUG: as in MONSTER_CritterFleeFromTarget, the path tested inside the inlined accessors is read untested by D2Common below.
    //  UNREACHABLE: every client monster has a path.
    pDynamicPath = pUnit->pDynamicPath;
    pParams[0] = nMyX + nStepX;
    pParams[1] = nMyY + nStepY;
    pParams[2] = 0;
    pParams[3] = D2COMMON_PATH_GetType(pDynamicPath);
    pParams[4] = nVelocityPercent;
    pParams[5] = D2COMMON_PATH_GetMaxDistance(pDynamicPath);
    MONSTER_DoClientAction(nAction, pUnit, pParams);
}

//1.10f: D2Client.0x6FB3FB80
//  Stands a critter still: CLTACTION_NEUTRAL at its own position, the block zeroed first (`rep stosd`, ecx 7) and the point written into
//  [0] and [1]. The original inlines UNITS_GetXPosition here.
void __fastcall MONSTER_CritterStandStill(D2UnitStrc *pUnit)
{
    int32_t pParams[7];

    memset(pParams, 0, sizeof(pParams));
    pParams[0] = UNITS_GetXPosition(pUnit);
    pParams[1] = UNITS_GetYPosition(pUnit);
    MONSTER_DoClientAction(CLTACTION_NEUTRAL, pUnit, pParams);
}

//1.10f: D2Client.0x6FB3FC10
//  Sets a critter's idle ticks: min(nTicks, 200) (signed compare) into the client timer MONSTER_CritterTickAmbient counts down, when the
//  monster has one.
void __fastcall MONSTER_CritterSetCooldown(D2UnitStrc *pUnit, int32_t nTicks)
{
    int32_t *pClientTimer;

    if (nTicks >= 200)
        nTicks = 200;

    pClientTimer = MONSTER_GetClientTimerPtr(pUnit);
    if (pClientTimer)
        *pClientTimer = nTicks;

}

//1.10f: D2Client.0x6FB3FC30
//  One tick of a flying critter (bird1, bird2, parrot), from MONSTER_CritterTickAmbient's switch. `retn 4`: nNearest on the stack; neither
//  it nor pTarget in edx is read, and the dispatcher hands every Tick* the same three. The original inlines SEED_RollLimitedRandomNumber and
//  UNITS_GetXPosition here.
//  In MONMODE_NEUTRAL only, and only for a monster with client data: the flag MONSTER_GetClientFlagPtr answers (pMonsterData +0x34,
//  bCritterLatch) says the bird has flown once. A bird whose flag is set and that is more than 25 from the local player (signed compare) is
//  removed from the client unit table (CUNIT_RemoveUnitByClientId); any other sets the flag and flies. The distance is taken first, flag
//  or not.
//  The destination is drawn around the PLAYER. A coin -- SEED_RollRandomNumber's low bit -- gives one axis the full 40 and the other a roll
//  of 40 (set: X; clear: Y); a coin each then negates X and Y. The seed advances four times a flight, three calls and one inlined roll.
//  Each coin is also stored to a frame slot that is overwritten or never read, variables of the original's source that the C cannot keep.
//  The bird is then sent with PATH_SetNewDistance(90) first and CLTACTION_WALKTOXY at velocity 0 to 3 * destination - 2 * its own position
//  on each axis: the destination overshot by twice what is left to cover. [6] of the block is not written, and WALKTOXY does not read it.
void __fastcall MONSTER_CritterTickBird(D2UnitStrc *pBird, D2UnitStrc *pTarget, int32_t nNearest)
{
    int32_t *pClientFlag;
    D2UnitStrc *pPlayer;
    int32_t nDistance;
    D2SeedStrc *pSeed;
    int32_t nOffsetX;
    int32_t nOffsetY;
    int32_t nPlayerX;
    int32_t nPlayerY;
    int32_t nDestX;
    int32_t nDestY;
    int32_t nBirdX;
    int32_t nBirdY;
    D2DynamicPathStrc *pDynamicPath;
    int32_t pParams[7];

    UNREFERENCED_PARAMETER(pTarget);
    UNREFERENCED_PARAMETER(nNearest);

    if (!pBird || pBird->dwAnimMode != MONMODE_NEUTRAL)
        return;

    pClientFlag = MONSTER_GetClientFlagPtr(pBird);
    if (!pClientFlag)
        return;

    //  BUG: CUNIT_GetPlayer's answer goes untested into D2Common's UNITS_GetDistanceToOtherUnit as its second unit, and the position copies
    //  below read it; a NULL player would fault there. LATENT: the tick runs only inside the gbInGame tick, which is set only once the
    //  player's room is found.
    pPlayer = CUNIT_GetPlayer();
    nDistance = D2COMMON_UNITS_GetDistanceToOtherUnit(pBird, pPlayer);
    if (*pClientFlag && nDistance > 25)
    {
        CUNIT_RemoveUnitByClientId(pBird->dwUnitId, UNIT_MONSTER);
        return;
    }

    *pClientFlag = 1;
    pSeed = &pBird->pSeed;
    if ((uint32_t)SEED_RollRandomNumber(pSeed) & 1)
    {
        nOffsetX = 40;
        nOffsetY = SEED_RollLimitedRandomNumber(pSeed, 40);
    }
    else
    {
        nOffsetY = 40;
        nOffsetX = SEED_RollLimitedRandomNumber(pSeed, 40);
    }

    if ((uint32_t)SEED_RollRandomNumber(pSeed) & 1)
        nOffsetX = -nOffsetX;
    if ((uint32_t)SEED_RollRandomNumber(pSeed) & 1)
        nOffsetY = -nOffsetY;

    nPlayerX = UNITS_GetXPosition(pPlayer);
    nPlayerY = UNITS_GetYPosition(pPlayer);
    nDestX = nPlayerX + nOffsetX;
    nDestY = nPlayerY + nOffsetY;

    nBirdX = UNITS_GetXPosition(pBird);
    nBirdY = UNITS_GetYPosition(pBird);
    D2COMMON_PATH_SetNewDistance(pBird->pDynamicPath, 90);
    pDynamicPath = pBird->pDynamicPath;
    pParams[0] = 3 * nDestX - 2 * nBirdX;
    pParams[1] = 3 * nDestY - 2 * nBirdY;
    pParams[2] = 0;
    pParams[3] = D2COMMON_PATH_GetType(pDynamicPath);
    pParams[4] = 0;
    pParams[5] = D2COMMON_PATH_GetMaxDistance(pDynamicPath);
    MONSTER_DoClientAction(CLTACTION_WALKTOXY, pBird, pParams);
}

//1.10f: D2Client.0x6FB3FE20
//  One tick of a maggot, monster 227. `retn 4`; nNearest is not read. The original inlines UNITS_GetXPosition, SEED_RollLimitedRandomNumber,
//  MONSTER_CritterSetCooldown and MONSTER_CritterStandStill here.
//  A dead maggot does nothing (D2Common's MONSTERS_IsDead). A maggot on the very client position of the nearest player (both coordinates
//  equal, not a distance) takes CLTACTION_DEATH in place: stepped on. Otherwise, in MONMODE_NEUTRAL, a roll of 100 at 40 or more (signed
//  compare) wanders 4 at velocity 0, and below it the maggot idles 20 ticks and stands still.
void __fastcall MONSTER_CritterTickMaggot(D2UnitStrc *pMaggot, D2UnitStrc *pTarget, int32_t nNearest)
{
    uint32_t dwAnimMode;
    int32_t nMyX;
    int32_t nMyY;
    int32_t nTargetX;
    int32_t nTargetY;
    int32_t pParams[7];

    UNREFERENCED_PARAMETER(nNearest);

    //  BUG: the maggot is tested for NULL when its mode is read, before MONSTERS_IsDead, and nothing after repeats the test; the position
    //  copy then reads the unit. UNREACHABLE: the one caller, MONSTER_CritterTickAmbient, has read the unit for its timer and its class
    //  switch, so no NULL arrives.
    dwAnimMode = pMaggot ? pMaggot->dwAnimMode : 0;
    if (D2COMMON_MONSTERS_IsDead(pMaggot))
        return;

    nMyX = UNITS_GetXPosition(pMaggot);
    nMyY = UNITS_GetYPosition(pMaggot);
    nTargetX = UNITS_GetXPosition(pTarget);
    nTargetY = UNITS_GetYPosition(pTarget);
    if (nMyX == nTargetX && nMyY == nTargetY)
    {
        memset(pParams, 0, sizeof(pParams));
        pParams[0] = UNITS_GetXPosition(pMaggot);
        pParams[1] = UNITS_GetYPosition(pMaggot);
        MONSTER_DoClientAction(CLTACTION_DEATH, pMaggot, pParams);
        return;
    }

    if (dwAnimMode != MONMODE_NEUTRAL)
        return;

    if (SEED_RollLimitedRandomNumber(&pMaggot->pSeed, 100) >= 40)
    {
        MONSTER_CritterWanderRandom(pMaggot, 4, CLTACTION_WALKTOXY, 0);
        return;
    }

    MONSTER_CritterSetCooldown(pMaggot, 20);
    MONSTER_CritterStandStill(pMaggot);
}

//1.10f: D2Client.0x6FB3FFE0
//  One tick of a crawling critter: the rat, the scorpion, the larva and the minispider. `retn 4`: nNearest on the stack, and every test of it
//  is unsigned (`jnb`, `jbe`, `ja`), so it is typed `unsigned int`. The original inlines MONSTER_ForEachUnitNearby (with the corpse callback
//  constant), MONSTER_CritterSetCooldown, MONSTER_CritterStandStill, MONSTER_CritterMoveToUnit and SEED_RollLimitedRandomNumber here.
//  A crawler dying or dead (MONMODE_DEATH, MONMODE_DEAD -- a monster's modes) does nothing. Then:
//    * nearer than 2 and on the player's very position: CLTACTION_DEATH in place, stepped on;
//    * not in MONMODE_NEUTRAL: nothing;
//    * nearer than 4: it flees 6 at velocity 0. The action is pushed from eax, which holds the mode just compared with 1 --
//      CLTACTION_WALKTOXY's value, written as that;
//    * farther than 7: a search of the nearby rooms for a corpse, MONSTER_CritterPickRandomCorpseCallback over MONSTER_ForEachUnitNearby.
//  With client data (MONSTER_GetClientFlagPtr, +0x34) the flag is a latch: a corpse found while unlatched (and the player farther than 7) is
//  walked to -- CLTACTION_WALKTOUNIT at velocity 6 -- and latches it; while latched the crawler idles 32 ticks and stands still, dropping the
//  latch on a roll of 100 below 20; unlatched with no corpse to go to, a roll below 75 idles 30 ticks and stands still, and 75 and up
//  wanders 5 at velocity 0.
//  Without client data it stands still, and the 32 ticks it sets first go nowhere: MONSTER_GetClientTimerPtr refuses exactly what
//  MONSTER_GetClientFlagPtr refused, so the copied setter's store is skipped.
//  The corpse search is MONSTER_ForEachUnitNearby inlined with the callback constant. The callback answers 0 on every path, so the walk's
//  early exit is dead and every unit of every adjacent room is offered.
void __fastcall MONSTER_CritterTickCrawler(D2UnitStrc *pUnit, D2UnitStrc *pTarget, uint32_t nNearest)
{
    uint32_t dwAnimMode;
    int32_t nMyX;
    int32_t nMyY;
    int32_t nTargetX;
    int32_t nTargetY;
    D2UnitStrc *pFound;
    int32_t nCorpseDistance;
    int32_t *pClientFlag;
    int32_t pParams[7];

    if (!pUnit)
        return;

    dwAnimMode = pUnit->dwAnimMode;
    if (dwAnimMode == MONMODE_DEAD || dwAnimMode == MONMODE_DEATH)
        return;

    if (nNearest < 2)
    {
        nMyX = UNITS_GetXPosition(pUnit);
        nMyY = UNITS_GetYPosition(pUnit);
        nTargetX = UNITS_GetXPosition(pTarget);
        nTargetY = UNITS_GetYPosition(pTarget);
        if (nMyX == nTargetX && nMyY == nTargetY)
        {
            memset(pParams, 0, sizeof(pParams));
            pParams[0] = UNITS_GetXPosition(pUnit);
            pParams[1] = UNITS_GetYPosition(pUnit);
            MONSTER_DoClientAction(CLTACTION_DEATH, pUnit, pParams);
            return;
        }
    }

    if (dwAnimMode != MONMODE_NEUTRAL)
        return;

    if (nNearest < 4)
    {
        MONSTER_CritterFleeFromTarget(pUnit, pTarget, 6, CLTACTION_WALKTOXY, 0);
        return;
    }

    pFound = NULL;
    nCorpseDistance = 0;
    if (nNearest > 7)
    {
        MONSTER_ForEachUnitNearby(pUnit, &pFound, &nCorpseDistance, (int32_t (__fastcall *)(D2UnitStrc *, D2UnitStrc *, void *, void *)) MONSTER_CritterPickRandomCorpseCallback);
    }

    pClientFlag = MONSTER_GetClientFlagPtr(pUnit);
    if (!pClientFlag)
    {
        MONSTER_CritterSetCooldown(pUnit, 32);
        MONSTER_CritterStandStill(pUnit);
        return;
    }

    if (pFound && nNearest > 7)
    {
        if (!*pClientFlag)
        {
            MONSTER_CritterMoveToUnit(pUnit, pFound, CLTACTION_WALKTOUNIT, 6);
            *pClientFlag = 1;
            return;
        }
    }
    else if (!*pClientFlag)
    {
        if (SEED_RollLimitedRandomNumber(&pUnit->pSeed, 100) < 75)
        {
            MONSTER_CritterSetCooldown(pUnit, 30);
            MONSTER_CritterStandStill(pUnit);
            return;
        }

        MONSTER_CritterWanderRandom(pUnit, 5, CLTACTION_WALKTOXY, 0);
        return;
    }

    if (SEED_RollLimitedRandomNumber(&pUnit->pSeed, 100) < 20)
        *pClientFlag = 0;
    MONSTER_CritterSetCooldown(pUnit, 32);
    MONSTER_CritterStandStill(pUnit);
}

//1.10f: D2Client.0x6FB402F0
//  The corpse search's callback of MONSTER_CritterTickCrawler (its one reference is that function's inlined walk). `retn 8`: the walk's two
//  addresses, the chosen unit and its distance. The original inlines SEED_RollRandomNumber here.
//  A candidate counts when it is dead by its own unit type's modes -- a player in PLRMODE_DEATH or PLRMODE_DEAD, a monster in MONMODE_DEATH
//  or MONMODE_DEAD -- the distinction MONSTER_CritterTickAmbient's player walk does not make. Then 3 to 25 from the critter (signed
//  compares) and more than 7 from the local player; then a coin on the critter's seed keeps it. So the pick is not the nearest: every
//  corpse in range may replace the last, and the last to win its coin stays. The coin is also stored to a frame slot that is never read.
//  The answer is 0 on every path; the player arm's refusal returns eax unchanged, the unit type it has just found to be 0.
int32_t __fastcall MONSTER_CritterPickRandomCorpseCallback(D2UnitStrc *pCritter, D2UnitStrc *pCandidate, D2UnitStrc **ppChosen, int32_t *pnDistance)
{
    uint32_t dwAnimMode;
    int32_t nDistance;

    if (!pCandidate)
        return 0;

    if (pCandidate->dwUnitType != UNIT_PLAYER && pCandidate->dwUnitType != UNIT_MONSTER)
        return 0;

    dwAnimMode = pCandidate->dwAnimMode;
    if (pCandidate->dwUnitType == UNIT_PLAYER)
    {
        if (dwAnimMode != PLRMODE_DEATH && dwAnimMode != PLRMODE_DEAD)
            return 0;
    }
    else if (dwAnimMode != MONMODE_DEATH && dwAnimMode != MONMODE_DEAD)
    {
        return 0;
    }

    nDistance = D2COMMON_UNITS_GetDistanceToOtherUnit(pCritter, pCandidate);
    if (nDistance < 3 || nDistance > 25)
        return 0;

    //  BUG: CUNIT_GetPlayer's answer goes untested into UNITS_GetDistanceToOtherUnit as its second unit, as in MONSTER_CritterTickBird; a NULL
    //  player would fault in D2Common. LATENT: only inside the gbInGame tick.
    if (D2COMMON_UNITS_GetDistanceToOtherUnit(pCandidate, CUNIT_GetPlayer()) <= 7)
        return 0;

    if ((uint32_t)SEED_RollRandomNumber(&pCritter->pSeed) & 1)
    {
        *pnDistance = nDistance;
        *ppChosen = pCandidate;
    }

    return 0;
}

//1.10f: D2Client.0x6FB40390
//  One tick of a snake, monster 318. `retn 4`: nNearest, compared unsigned. A dead snake does nothing (MONSTERS_IsDead); one on the player's
//  very position takes CLTACTION_DEATH in place, stepped on. Otherwise, in MONMODE_NEUTRAL: with the player 1 or nearer it flees 4 at
//  velocity 0; farther, a roll of 100 at 75 or more wanders 4 at velocity 0 and below it the snake idles 32 ticks and stands still. The
//  snake itself is never tested for NULL: MONSTERS_IsDead answers FALSE for it and the position copy reads it; the one caller hands it a
//  unit whose class its switch has read. The original inlines UNITS_GetXPosition, UNITS_GetYPosition, SEED_RollLimitedRandomNumber,
//  MONSTER_CritterSetCooldown and MONSTER_CritterStandStill here.
void __fastcall MONSTER_CritterTickSnake(D2UnitStrc *pSnake, D2UnitStrc *pThreat, uint32_t nNearest)
{
    int32_t nSnakeX;
    int32_t nSnakeY;
    int32_t nThreatX;
    int32_t nThreatY;
    int32_t pParams[7];

    if (D2COMMON_MONSTERS_IsDead(pSnake))
        return;

    nSnakeX = UNITS_GetXPosition(pSnake);
    nSnakeY = UNITS_GetYPosition(pSnake);
    nThreatX = UNITS_GetXPosition(pThreat);
    nThreatY = UNITS_GetYPosition(pThreat);
    if (nSnakeX == nThreatX && nSnakeY == nThreatY)
    {
        memset(pParams, 0, sizeof(pParams));
        pParams[0] = UNITS_GetXPosition(pSnake);
        pParams[1] = UNITS_GetYPosition(pSnake);
        MONSTER_DoClientAction(CLTACTION_DEATH, pSnake, pParams);
        return;
    }

    if (pSnake->dwAnimMode != MONMODE_NEUTRAL)
        return;

    if (nNearest <= 1)
    {
        MONSTER_CritterFleeFromTarget(pSnake, pThreat, 4, CLTACTION_WALKTOXY, 0);
        return;
    }

    if (SEED_RollLimitedRandomNumber(&pSnake->pSeed, 100) >= 75)
    {
        MONSTER_CritterWanderRandom(pSnake, 4, CLTACTION_WALKTOXY, 0);
        return;
    }

    MONSTER_CritterSetCooldown(pSnake, 32);
    MONSTER_CritterStandStill(pSnake);
}

//1.10f: D2Client.0x6FB40560
//  One tick of a bunny, monster 556, in MONMODE_NEUTRAL only. `retn 4`; neither pTarget nor nNearest is read. The original inlines
//  UNITS_GetYPosition, UNITS_GetXPosition, MONSTER_ForEachUnitNearby, SEED_RollLimitedRandomNumber and MONSTER_CritterSetCooldown here.
//  The bunny's own client position is taken Y first, then X, and the room after both: a cell where D2Common's COLLISION_CheckMask finds
//  COLLIDE_MISSILE, COLLIDE_PLAYER or COLLIDE_MONSTER (mask 0x1C0) kills it in place, CLTACTION_DEATH.
//  Otherwise it runs a threat search of its own: MONSTER_ForEachUnitNearby with MONSTER_CritterFindNearestThreatCallback, both inlined --
//  the chicken's and the bug's search in MONSTER_CritterTickAmbient, out of line there. So a bunny ignores bunnies and chickens, and the
//  callback's dead test is the bunny's own (CUNIT_IsUnitDead), as in the callback's BUG note. With a threat found, the footprint mask
//  becomes COLLIDE_MASK_MONSTER_PATH; nearer than 6 (signed compare) it is narrowed to COLLIDE_MASK_PLAYER_WW and the bunny flees 6 at
//  velocity 120, wandering 6 the same way when the step is blocked; at 6 or more a roll of 100 below 30 wanders 2 at velocity 0, and the
//  rest idle 5 ticks. The chicken's and the bug's arm in MONSTER_CritterTickAmbient has these branches with 4 where the bunny has 6, and
//  CLTACTION_MONSKILL1_XY for the flee and its fallback.
void __fastcall MONSTER_CritterTickBunny(D2UnitStrc *pBunny, D2UnitStrc *pTarget, int32_t nNearest)
{
    int32_t nBunnyY;
    int32_t nBunnyX;
    D2UnitStrc *pThreat;
    int32_t nThreatDist;
    D2DynamicPathStrc *pDynamicPath;
    int32_t pParams[7];

    UNREFERENCED_PARAMETER(pTarget);
    UNREFERENCED_PARAMETER(nNearest);

    if (!pBunny || pBunny->dwAnimMode != MONMODE_NEUTRAL)
        return;

    nBunnyY = UNITS_GetYPosition(pBunny);
    nBunnyX = UNITS_GetXPosition(pBunny);
    if (D2COMMON_COLLISION_CheckMask(D2COMMON_UNITS_GetRoom(pBunny), nBunnyX, nBunnyY, COLLIDE_MISSILE | COLLIDE_PLAYER | COLLIDE_MONSTER))
    {
        memset(pParams, 0, sizeof(pParams));
        pParams[0] = UNITS_GetXPosition(pBunny);
        pParams[1] = UNITS_GetYPosition(pBunny);
        MONSTER_DoClientAction(CLTACTION_DEATH, pBunny, pParams);
        return;
    }

    pThreat = NULL;
    nThreatDist = 0x7FFFFFFF;
    MONSTER_ForEachUnitNearby(pBunny, &pThreat, &nThreatDist, (int32_t (__fastcall *)(D2UnitStrc *, D2UnitStrc *, void *, void *)) MONSTER_CritterFindNearestThreatCallback);
    if (!pThreat)
        return;

    pDynamicPath = pBunny->pDynamicPath;
    D2COMMON_PATH_SetFootprintCollisionMask(pDynamicPath, COLLIDE_MASK_MONSTER_PATH);
    if (nThreatDist < 6)
    {
        D2COMMON_PATH_SetFootprintCollisionMask(pDynamicPath, COLLIDE_MASK_PLAYER_WW);
        if (!MONSTER_CritterFleeFromTarget(pBunny, pThreat, 6, CLTACTION_WALKTOXY, 120))
            MONSTER_CritterWanderRandom(pBunny, 6, CLTACTION_WALKTOXY, 120);
        return;
    }

    if (SEED_RollLimitedRandomNumber(&pBunny->pSeed, 100) < 30)
    {
        MONSTER_CritterWanderRandom(pBunny, 2, CLTACTION_WALKTOXY, 0);
        return;
    }

    MONSTER_CritterSetCooldown(pBunny, 5);
}

//1.10f: D2Client.0x6FB40790
//  The Worldstone Chamber's falling chips, monster 574 (worldstoneeffect), dispatched with the critters. `retn 4`; neither pTarget nor
//  nNearest is read, and the unit is not tested for NULL -- the one caller hands it a unit its switch has read. The original inlines
//  MONSTER_CritterSetCooldown, SEED_RollLimitedRandomNumber and UNITS_GetXPosition here.
//  With no local player it idles 5 ticks. Otherwise a roll of 100 below 5 (signed compare) spawns one chip: the missile
//  MISSILE_WORLD_STONE_CHIP_1 + (SEED_RollRandomNumber's low two bits), at the unit's client position plus a roll of 60 less 30 on each
//  axis, through MISSILE_ClientSpawnSimple with skill 0 at level 1. A coin then sends it up or down: set, it starts 100 below (GfxCoords z
//  -100) and rises (GfxVector z 6, ParticleVector z 300); clear, it starts 300 above and falls (-6, -100). The coin is passed on to
//  PARTICLE_SetUnitRisesToTarget; then D2Common's MISSILE_SetTotalFrames and MISSILE_SetCurrentFrame take 60 each, MISSILE_SetTargetX 100 and
//  MISSILE_SetTargetY 300. The coin is also stored to the slot the missile id used, never read.
//  The seed advances five times for a chip (the 1-in-20 roll, the missile id, the two offsets and the coin) and once for a tick that spawns
//  nothing.
void __fastcall MONSTER_EnvTickWorldstoneChips(D2UnitStrc *pUnit, D2UnitStrc *pTarget, int32_t nNearest)
{
    D2SeedStrc *pSeed;
    int32_t nMissileId;
    int32_t nUnitX;
    int32_t nUnitY;
    int32_t nSpawnX;
    int32_t nSpawnY;
    D2UnitStrc *pChip;
    int32_t bRising;

    UNREFERENCED_PARAMETER(pTarget);
    UNREFERENCED_PARAMETER(nNearest);

    pSeed = &pUnit->pSeed;
    if (!CUNIT_GetPlayer())
    {
        MONSTER_CritterSetCooldown(pUnit, 5);
        return;
    }

    if (SEED_RollLimitedRandomNumber(pSeed, 100) >= 5)
        return;

    nMissileId = ((uint32_t)SEED_RollRandomNumber(pSeed) & 3) + MISSILE_WORLD_STONE_CHIP_1;
    nUnitX = UNITS_GetXPosition(pUnit);
    nSpawnX = SEED_RollLimitedRandomNumber(pSeed, 60) + nUnitX - 30;
    nUnitY = UNITS_GetYPosition(pUnit);
    nSpawnY = SEED_RollLimitedRandomNumber(pSeed, 60) + nUnitY - 30;
    pChip = MISSILE_ClientSpawnSimple(pUnit, nMissileId, nSpawnX, nSpawnY, SKILL_ATTACK, 1);
    if (!pChip)
        return;

    bRising = (uint32_t)SEED_RollRandomNumber(pSeed) & 1;
    if (bRising)
    {
        PARTICLE_SetUnitGfxCoords(pChip, 0, 0, -100);
        PARTICLE_SetUnitGfxVector(pChip, 0, 0, 6, 1);
        PARTICLE_SetUnitParticleVector(pChip, 0, 0, 300, 1);
    }
    else
    {
        PARTICLE_SetUnitGfxCoords(pChip, 0, 0, 300);
        PARTICLE_SetUnitGfxVector(pChip, 0, 0, -6, 1);
        PARTICLE_SetUnitParticleVector(pChip, 0, 0, -100, 1);
    }

    PARTICLE_SetUnitRisesToTarget(pChip, bRising);
    PARTICLE_UpdateUnitParticle(pChip);
    D2COMMON_MISSILE_SetTotalFrames(pChip, 60);
    D2COMMON_MISSILE_SetCurrentFrame(pChip, 60);
    D2COMMON_MISSILE_SetTargetX(pChip, 100);
    D2COMMON_MISSILE_SetTargetY(pChip, 300);
}


//1.10f: D2Client.0x6FB40970
//  Whether a monster class's animation mode is enabled: monstats2's mode bitfield read a byte at a time,
//  `nModeFlags[nMode >> 3] & gdwBitMasks[nMode & 7]`. The answer is the mask bit, 1 to 0x80 by mode, not a normalised 0/1 (`and eax, ebx`
//  over the zero-extended byte); the one caller, MONSTER_CritterTickAmbient, reads it as a truth value. A NULL record answers 0. The shift is
//  `sar edx, 3`, signed, on a parameter typed D2C_MonModes whose generated base is unsigned -- hence the cast. The original inlines
//  DATATBLS_GetMonStats2TxtRecord here.
int32_t __fastcall MONSTER_IsMonModeEnabled(D2C_MonsterIds nMonsterClassId, D2C_MonModes nMode)
{
    D2MonStats2Txt *pMonStats2Txt;

    pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord((int32_t)nMonsterClassId);
    if (!pMonStats2Txt)
        return 0;

    return gdwBitMasks[nMode & 7] & pMonStats2Txt->nModeFlags[(int32_t)nMode >> 3];
}


//1.10f: D2Client.0x6FB409E0
//  The sound a monster makes for one of its skills. monstats.txt lists the monster's skills in nSkill[8]; its wMonSound row of monsounds.txt
//  lists a sound per slot in dwSkill[4]; the slot ties them. The loop walks all eight skills and a switch (a four-slot jump table, not an
//  indexed read) picks the sound for the first four slots only, so a skill in slot 5..8 has no sound, and a skill listed twice in the first
//  four keeps the LAST match. A negative sound comes back as 0. The class id test is signed and the skill compare takes the monstats word
//  sign-extended against the whole nSkillId. D2Common's DATATBLS_GetMonSoundsTxtRecordFromSoundId is `__stdcall` and is handed wMonSound
//  sign-extended. The original inlines DATATBLS_GetMonStatsTxtRecord here.
D2C_Sounds __fastcall MONSTER_GetSkillSound(D2UnitStrc *pUnit, D2C_Skills nSkillId)
{
    D2MonStatsTxt *pMonStatsTxt;
    D2MonSoundsTxt *pMonSoundsTxt;
    D2C_Sounds nSoundId;
    int32_t i;

    if (!pUnit)
        return SOUND_NONE;

    pMonStatsTxt = DATATBLS_GetMonStatsTxtRecord(pUnit->dwClassId);
    if (!pMonStatsTxt)
        return SOUND_NONE;

    pMonSoundsTxt = D2COMMON_DATATBLS_GetMonSoundsTxtRecordFromSoundId(pMonStatsTxt->wMonSound);
    if (!pMonSoundsTxt)
        return SOUND_NONE;

    nSoundId = SOUND_NONE;
    for (i = 0; i < 8; ++i)
    {
        if (pMonStatsTxt->nSkill[i] == (int32_t)nSkillId)
        {
            switch (i)
            {
            case 0:
                nSoundId = pMonSoundsTxt->dwSkill[0];
                break;
            case 1:
                nSoundId = pMonSoundsTxt->dwSkill[1];
                break;
            case 2:
                nSoundId = pMonSoundsTxt->dwSkill[2];
                break;
            case 3:
                nSoundId = pMonSoundsTxt->dwSkill[3];
                break;
            }
        }
    }

    return nSoundId < 0 ? SOUND_NONE : nSoundId;
}


//1.10f: D2Client.0x6FB40A80
//  Binds a monster's monstats.txt record into pMonsterData->pMonstatsTxt, or NULL for a class out of range, and first stores the 16-bit
//  argument into the low half of the pAiControl slot (`mov [esi+28h], dx`, a word store into a pointer). The one caller,
//  CUNIT_CreateMonsterUnit, passes 0; the client keeps no AI control, and nothing here reads the word. Nothing happens for NULL, a unit that
//  is not UNIT_MONSTER or one with no monster data. The original inlines DATATBLS_GetMonStatsTxtRecord here.
void __fastcall MONSTER_BindMonStatsRecord(D2UnitStrc *pUnit, int16_t nAiControl)
{
    D2MonsterDataStrc *pMonsterData;

    if (!pUnit || pUnit->dwUnitType != UNIT_MONSTER)
        return;

    pMonsterData = pUnit->pMonsterData;
    if (!pMonsterData)
        return;

    *(int16_t *)&pMonsterData->pAiControl = nAiControl;

    pMonsterData->pMonstatsTxt = DATATBLS_GetMonStatsTxtRecord(pUnit->dwClassId);
}


//1.10f: D2Client.0x6FB40AD0
//  The address of pMonsterData +0x30, or NULL unless the unit is a monster with monster data. What the callers keep there is an idle count:
//  MONSTER_CritterTickAmbient takes one off while it is above 0 and skips the tick, and MONSTER_CritterSetCooldown and its copies set it.
//  The slot is a union of a server pointer (pMonInteract) and the client's int32_t nCritterIdleTicks, so the address is the member's. The
//  first use reads it signed (`jle`) and stores the decrement back; the others each store an immediate or a register.
int32_t *__fastcall MONSTER_GetClientTimerPtr(D2UnitStrc *pUnit)
{
    D2MonsterDataStrc *pMonsterData;

    if (pUnit && pUnit->dwUnitType == UNIT_MONSTER && (pMonsterData = pUnit->pMonsterData) != NULL)
        return &pMonsterData->nCritterIdleTicks;

    return NULL;
}


//1.10f: D2Client.0x6FB40AF0
//  A hireling's owner, pMonsterData +0x3C: the player's unit id, or -1 for NULL, a unit that is not a monster, or one with no monster data -- the
//  same -1 the field holds for a monster with no player owner, so a caller cannot tell the two apart. The one writer is MONSTER_SetHirelingOwnerGUID,
//  from CUNIT_CreateMonsterUnit's assign record (pAssign[5]), which SCMD_HandleSrvAC_SpawnMonster fills with -1 and, when packet 0xAC's last flag bit
//  is set, with the 32 bits after it. A client-only monster holds 0, not -1. The one reader is NPCMENU_ShowNpcAlert: 534 act5pow, the caged soldiers
//  of Act 5, plays wussie_help_me when this answers -1 and wussie_cheer_1 otherwise. Whether 1.10's server ever sends a caged soldier an owner -- and
//  so whether the cheer can play -- is not established.
int32_t __fastcall MONSTER_GetHirelingOwnerGUID(D2UnitStrc *pUnit)
{
    D2MonsterDataStrc *pMonsterData;

    if (pUnit && pUnit->dwUnitType == UNIT_MONSTER && (pMonsterData = pUnit->pMonsterData) != NULL)
        return pMonsterData->unk0x3C.unk0x00;

    return -1;
}


//1.10f: D2Client.0x6FB40B10
//  Sets a hireling's owner, pMonsterData +0x3C: the setter of MONSTER_GetHirelingOwnerGUID. Nothing for NULL, a non-monster or no monster
//  data.
void __fastcall MONSTER_SetHirelingOwnerGUID(D2UnitStrc *pUnit, int32_t nOwnerGUID)
{
    D2MonsterDataStrc *pMonsterData;

    if (pUnit && pUnit->dwUnitType == UNIT_MONSTER)
    {
        pMonsterData = pUnit->pMonsterData;
        if (pMonsterData)
            pMonsterData->unk0x3C.unk0x00 = nOwnerGUID;
    }
}


//1.10f: D2Client.0x6FB40B30
//  The address of pMonsterData +0x34 (bCritterLatch), or NULL unless the unit is a monster with monster data -- the twin of
//  MONSTER_GetClientTimerPtr. Its two callers are MONSTER_CritterTickBird (the bird's once-flown flag, which lets its despawn fire) and
//  MONSTER_CritterTickCrawler (the latch, set when it walks to a corpse and dropped on a roll of 100 below 20). Both hold 0 or 1 there and
//  only test it against zero; int32_t is the width the bytes give it.
int32_t *__fastcall MONSTER_GetClientFlagPtr(D2UnitStrc *pUnit)
{
    D2MonsterDataStrc *pMonsterData;

    if (pUnit && pUnit->dwUnitType == UNIT_MONSTER && (pMonsterData = pUnit->pMonsterData) != NULL)
        return &pMonsterData->bCritterLatch;

    return NULL;
}


//1.10f: D2Client.0x6FB40B50
//  Gives a monster its light, replacing the one it has. The radius is the larger of D2Common's MONSTERS_GetMaximalLightRadius and monstats2's
//  `Light` column (unsigned bytes); the colour is the same row's nLight_R/G/B. A radius of 0 makes no light. The original inlines
//  DATATBLS_GetMonStatsTxtRecord and DATATBLS_GetMonStats2TxtRecord here.
//  The Den of Evil override. In level 8 (LEVEL_DENOFEVIL), while QUESTS_GetQuestState(0, 1) answers non-zero (quest 1, the Den of Evil), a
//  monster that is not UNITFLAG_ISASYNC and whose monstats `Align` is neither MONALIGN_GOOD nor MONALIGN_NEUTRAL gets radius 3 whatever the
//  tables say, even a monster whose own radius is 0. Bit 21 is taken as `shr ecx, 15h / and ecx, 1` and the bit is also stored to a slot
//  nothing in the function reads.
//  The light: DLIGHTMAP_AddLight with nType 0, the radius zero-extended, intensity 255 and the three colour bytes pushed from registers whose
//  upper bytes are not cleared -- AddLight keeps the low byte only, so the C passes the bytes. An old light at pUnit->pLight (+0x64) is freed
//  after the new one is made, the new one stored, and its radius handed to CUNIT_SetUnitStartLight by a tail jump.
void __fastcall MONSTER_UpdateClientLight(D2UnitStrc *pUnit)
{
    D2MonStatsTxt *pMonStatsTxt;
    D2MonStats2Txt *pMonStats2Txt;
    uint8_t nRadius;
    uint8_t nFinalRadius;
    D2ClientLightStrc *pLight;

    if (!pUnit || pUnit->dwUnitType != UNIT_MONSTER)
        return;

    pMonStatsTxt = DATATBLS_GetMonStatsTxtRecord(pUnit->dwClassId);
    if (!pMonStatsTxt)
        return;

    pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord(pUnit->dwClassId);
    if (!pMonStats2Txt)
        return;

    nRadius = D2COMMON_MONSTERS_GetMaximalLightRadius(pUnit);
    nFinalRadius = nRadius;
    if (nRadius < pMonStats2Txt->nLight)
    {
        nRadius = pMonStats2Txt->nLight;
        nFinalRadius = nRadius;
    }

    if (D2COMMON_DUNGEON_GetLevelIdFromRoom(D2COMMON_UNITS_GetRoom(pUnit)) == LEVEL_DENOFEVIL && QUESTS_GetQuestState(0, QUEST_A1Q1_DENOFEVIL) && !((pUnit->dwFlags >> 21) & 1) && pMonStatsTxt->nAlign != MONALIGN_GOOD
        && pMonStatsTxt->nAlign != MONALIGN_NEUTRAL)
    {
        nFinalRadius = 3;
    }
    else if (!nRadius)
    {
        return;
    }

    pLight = DLIGHTMAP_AddLight(pUnit, 0, nFinalRadius, 255, pMonStats2Txt->nLight_R, pMonStats2Txt->nLight_G, pMonStats2Txt->nLight_B);
    if (pUnit->pLight)
        CUNIT_GfxFreeUnitLightMap(pUnit);
    pUnit->pLight = pLight;
    CUNIT_SetUnitStartLight(pUnit, nFinalRadius);
}


//1.10f: D2Client.0x6FB40CC0
//  MONTYPEFLAG_CHAMPION, 4. It and the four type-flag tests below have one shape: NULL or a unit that is not UNIT_MONSTER answers FALSE,
//  anything else tail-jumps to MONSTER_TestTypeFlag with the flag in edx. That callee repeats both tests itself, reads a WORD at +0x16 and
//  answers 0 or 1, so these answer 0 or 1 too. The flags are D2C_MonTypeFlags, bits of pMonsterData->nTypeFlag.
BOOL __fastcall MONSTER_IsChampion(D2UnitStrc *pUnit)
{
    if (pUnit && pUnit->dwUnitType == UNIT_MONSTER)
        return MONSTER_TestTypeFlag(pUnit, MONTYPEFLAG_CHAMPION);

    return FALSE;
}


//1.10f: D2Client.0x6FB40CE0
//  MONTYPEFLAG_GHOSTLY, 0x40. One caller, UNITGFX_GfxDrawUnit.
BOOL __fastcall MONSTER_IsGhostly(D2UnitStrc *pUnit)
{
    if (pUnit && pUnit->dwUnitType == UNIT_MONSTER)
        return MONSTER_TestTypeFlag(pUnit, MONTYPEFLAG_GHOSTLY);

    return FALSE;
}


//1.10f: D2Client.0x6FB40D00
//  MONTYPEFLAG_UNIQUE, 8.
BOOL __fastcall MONSTER_IsUnique(D2UnitStrc *pUnit)
{
    if (pUnit && pUnit->dwUnitType == UNIT_MONSTER)
        return MONSTER_TestTypeFlag(pUnit, MONTYPEFLAG_UNIQUE);

    return FALSE;
}


//1.10f: D2Client.0x6FB40D20
//  MONTYPEFLAG_MINION, 0x10.
BOOL __fastcall MONSTER_IsMinion(D2UnitStrc *pUnit)
{
    if (pUnit && pUnit->dwUnitType == UNIT_MONSTER)
        return MONSTER_TestTypeFlag(pUnit, MONTYPEFLAG_MINION);

    return FALSE;
}


//1.10f: D2Client.0x6FB40D40
//  MONTYPEFLAG_SUPERUNIQUE, 2. One caller, UNITSND_GetMonSoundsRecordForUnit.
BOOL __fastcall MONSTER_IsSuperUnique(D2UnitStrc *pUnit)
{
    if (pUnit && pUnit->dwUnitType == UNIT_MONSTER)
        return MONSTER_TestTypeFlag(pUnit, MONTYPEFLAG_SUPERUNIQUE);

    return FALSE;
}


static_assert(sizeof(D2C_MonTypeFlags) == 2 && MONTYPEFLAG_SUPERUNIQUE == 2 && MONTYPEFLAG_CHAMPION == 4
                  && MONTYPEFLAG_UNIQUE == 8 && MONTYPEFLAG_MINION == 0x10 && MONTYPEFLAG_GHOSTLY == 0x40
                  && MONTYPEFLAG_MULTISHOT == 0x80,
              "the type-flag masks the original passes as literals, a WORD (`and ax, dx`, 0x6FB449E7): `mov edx, 4` "
              "(0x6FB40CC9), 0x40 (0x6FB40CE9), 8 (0x6FB40D09), 0x10 (0x6FB40D29), 2 (0x6FB40D49); MonUnique.cpp's "
              "multishot guard, `or byte ptr [eax+16h], 80h` (0x6FB45D90). CUnit.cpp and SCmd.cpp lean on these too");


//1.10f: D2Client.0x6FB40D60
//  Fills in a freshly allocated monster on the client. The one caller is CUNIT_CreateMonsterUnit, after it has bound the monstats record, the
//  owner GUID, the name, the modifiers and the components. A class with no monstats or no monstats2 record gets nothing. The original
//  inlines DATATBLS_GetMonStatsTxtRecord, DATATBLS_GetMonStats2TxtRecord, SEED_RollLimitedRandomNumber, MONSTER_BindMonStatsRecord,
//  MONSTER_GetBaseMonsterId, MONSTER_GetBaseId, MONSTER_IsMonModeEnabled and SEED_RollRandomNumber here.
//
//  In order: the difficulty, asserted below NUM_DIFFICULTY_LEVELS (GAME_GetDifficulty answers a byte, tested signed); a stat list with LEVEL
//  from monstats nLevel, ATTACKRATE 100, VELOCITYPERCENT 75, ANIMRATE 100 and the six resistances, each a monstats word of the difficulty
//  read signed (`movsx`) where the fields are typed unsigned; D2Common's classic scaling, with the difficulty asked for again; MAXHP and
//  HITPOINTS 25600, which is 100 in the 8-bit fixed point both stats are kept in; the dynamic path at nX, nY and its velocity, monstats
//  nVelocity << 8; an inventory; the animation mode; a dead monster's collision; the starting frame; four unit flags; the monstats record
//  bound again; the owned units' links and the light; the skill list, the unique modifiers and the eight monstats skills; and the facing,
//  unless the monster is an NPC.
//
//  The inventory goes to a monster with none whose monstats flags it INTERACT (byte 1 of the flag word against Fog's mask 1, a byte test) or
//  to the iron golem, the valkyrie or either shadow, whatever its flags. NOT ESTABLISHED: the server's rule for an inventory differs
//  (stock bloodraven and flyingscimitar have one on the server only, and INTERACT classes on the client only), and whether D2Game sends an
//  item into one of the two was not looked for. A dead monster whose monstats2 lacks DEADCOL loses its footprint and takes the small
//  no-presence pattern with the corpse mask, as Player.cpp's dead player does.
//
//  The flags come from monstats2's flag word, each a byte against the low byte of Fog's mask, and go to the whole dword: read twice, changed
//  with `or al` or `and al`, written back four times. ISSEL sets or clears UNITFLAG_TARGETABLE; SHADOW clears UNITFLAG_DRAWSHADOW and its
//  absence sets it; UNITFLAG_ISVALIDTARGET is set unconditionally, a second store of the same register; ISATT sets or clears
//  UNITFLAG_CANBEATTACKED.
//
//  The facing, the last act, is skipped for a monster flagged NPC (MONSTATSFLAGINDEX_NPC), which keeps the direction its path was made with.
//  Otherwise it is 0; a monster whose monstats2 enables MONMODE_WALK rolls one of 64 from its own seed; a monster of BaseId mummy1 faces 7;
//  one of BaseId vilechild1 takes what MONSTER_ForEachUnitNearby answers with MONSTER_MatchVileMotherDirection -- its mother's direction
//  when it stands where she spat it, else 0; and by the argument's class, hydra1 faces 24, hydra3 40, and hydra2, the valkyrie and the bone
//  wall 0. The BaseId lookups read the unit's dwClassId, the class tests the argument. The direction goes to the path pointer kept since
//  the path was made, not to the field read again.
void __fastcall MONSTER_InitClientUnit(D2UnitStrc *pUnit, D2ActiveRoomStrc *pRoom, int32_t nX, int32_t nY, D2C_MonsterIds nMonClassId, int32_t nAnimMode)
{
    D2MonStatsTxt *pMonStatsTxt;
    D2MonStats2Txt *pMonStats2Txt;
    D2MonStatsTxt *pClassMonStatsTxt;
    D2MonsterDataStrc *pMonsterData;
    D2DynamicPathStrc *pDynamicPath;
    uint8_t nDifficulty;
    uint8_t nScalingDifficulty;
    BOOL bExpansion;
    uint32_t nSkillBonus;
    int32_t nBaseId;
    uint8_t nDirection;
    int32_t i;

    pMonStatsTxt = DATATBLS_GetMonStatsTxtRecord((int32_t)nMonClassId);
    if (!pMonStatsTxt)
        return;

    pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord((int32_t)nMonClassId);
    if (!pMonStats2Txt)
        return;

    nDifficulty = GAME_GetDifficulty();
    if ((int32_t)nDifficulty >= (int32_t)NUM_DIFFICULTIES)
    {
        FOG_DisplayAssert("nDifficultyLevel < NUM_DIFFICULTY_LEVELS", szMonsterCpp, 496);
        exit(-1);
    }

    D2COMMON_STATLIST_AllocStatListEx(pUnit, 0, NULL, NULL);
    D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_LEVEL, (int16_t)pMonStatsTxt->nLevel[nDifficulty], 0);
    D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_ATTACKRATE, 100, 0);
    D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_VELOCITYPERCENT, 75, 0);
    D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_OTHER_ANIMRATE, 100, 0);
    D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_DAMAGERESIST, (int16_t)pMonStatsTxt->nResistances[0][nDifficulty], 0);
    D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_MAGICRESIST, (int16_t)pMonStatsTxt->nResistances[1][nDifficulty], 0);
    D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_FIRERESIST, (int16_t)pMonStatsTxt->nResistances[2][nDifficulty], 0);
    D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_LIGHTRESIST, (int16_t)pMonStatsTxt->nResistances[3][nDifficulty], 0);
    D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_COLDRESIST, (int16_t)pMonStatsTxt->nResistances[4][nDifficulty], 0);
    D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_POISONRESIST, (int16_t)pMonStatsTxt->nResistances[5][nDifficulty], 0);

    nScalingDifficulty = GAME_GetDifficulty();
    bExpansion = GAME_IsExpansionGame();
    D2COMMON_MONSTERS_ApplyClassicScaling(pUnit, bExpansion, nScalingDifficulty);
    D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_MAXHP, 100 << 8, 0);
    D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_HITPOINTS, 100 << 8, 0);

    D2COMMON_PATH_AllocDynamicPath(NULL, pRoom, nX, nY, pUnit, TRUE);
    pDynamicPath = pUnit->pDynamicPath;
    //  `movsx` then `shl` in the original; written through unsigned, which shifts the same bits and is defined for a negative velocity.
    D2COMMON_PATH_SetVelocity(pDynamicPath, (int32_t)((uint32_t)pMonStatsTxt->nVelocity << 8), szMonsterCpp, 520);

    if (!pUnit->pInventory && ((pMonStatsTxt->nMonStatsFlags[MONSTATSFLAGINDEX_INTERACT >> 3] & gdwBitMasks[MONSTATSFLAGINDEX_INTERACT & 7]) || nMonClassId == MONSTER_IRONGOLEM || nMonClassId == MONSTER_VALKYRIE
            || nMonClassId == MONSTER_SHADOWWARRIOR || nMonClassId == MONSTER_SHADOWMASTER))
    {
        pUnit->pInventory = D2COMMON_INVENTORY_AllocInventory(NULL, pUnit);
    }

    D2COMMON_UNITS_ChangeAnimMode(pUnit, nAnimMode);
    if (D2COMMON_MONSTERS_IsDead(pUnit) && !(pMonStats2Txt->nFlags[MONSTATS2FLAGINDEX_DEADCOL >> 3] & gdwBitMasks[MONSTATS2FLAGINDEX_DEADCOL & 7]))
    {
        D2COMMON_PATH_RemoveCollisionFootprintForUnit(pUnit, TRUE);
        D2COMMON_PATH_SetUnitCollisionPattern(pUnit, COLLISION_PATTERN_SMALL_NO_PRESENCE);
        D2COMMON_PATH_SetFootprintCollisionMask(pUnit->pDynamicPath, COLLIDE_CORPSE);
    }

    pUnit->nSeqCurrentFramePrecise = (int32_t)SEED_RollLimitedRandomNumberInline(&pUnit->pSeed, pUnit->dwFrameCountPrecise);

    if (pMonStats2Txt->nFlags[MONSTATS2FLAGINDEX_ISSEL >> 3] & gdwBitMasks[MONSTATS2FLAGINDEX_ISSEL & 7])
        pUnit->dwFlags |= UNITFLAG_TARGETABLE;
    else
        pUnit->dwFlags &= ~UNITFLAG_TARGETABLE;

    if (pMonStats2Txt->nFlags[MONSTATS2FLAGINDEX_SHADOW >> 3] & gdwBitMasks[MONSTATS2FLAGINDEX_SHADOW & 7])
        pUnit->dwFlags &= ~UNITFLAG_DRAWSHADOW;
    else
        pUnit->dwFlags |= UNITFLAG_DRAWSHADOW;

    pUnit->dwFlags |= UNITFLAG_ISVALIDTARGET;

    if (pMonStats2Txt->nFlags[MONSTATS2FLAGINDEX_ISATT >> 3] & gdwBitMasks[MONSTATS2FLAGINDEX_ISATT & 7])
        pUnit->dwFlags |= UNITFLAG_CANBEATTACKED;
    else
        pUnit->dwFlags &= ~UNITFLAG_CANBEATTACKED;

    if (pUnit->dwUnitType == UNIT_MONSTER)
    {
        pMonsterData = pUnit->pMonsterData;
        if (pMonsterData)
        {
            *(int16_t *)&pMonsterData->pAiControl = 0;

            pMonsterData->pMonstatsTxt = DATATBLS_GetMonStatsTxtRecord(pUnit->dwClassId);
        }
    }

    MONSTER_LinkOwnedUnits(pUnit, nMonClassId);
    MONSTER_UpdateClientLight(pUnit);
    pUnit->pSkills = D2COMMON_SKILLS_AllocSkillList(NULL);
    MONUNIQUE_ApplyModifiers(pUnit);

    //  The skills: a slot whose skill id is not negative and whose level byte is positive, signed (on a field typed unsigned), is assigned at
    //  that level plus the bonus; the mode byte is sign-extended and always set. INERT: D2Common's SKILLS_SetSkillMode ignores a NULL skill
    //  and a mode outside 0..32.
    nSkillBonus = D2COMMON_DATATBLS_GetDifficultyLevelsTxtRecord(nDifficulty)->dwMonsterSkillBonus;
    for (i = 0; i < 8; ++i)
    {
        if (pMonStatsTxt->nSkill[i] >= 0 && (int8_t)pMonStatsTxt->nSkLvl[i] > 0)
        {
            D2COMMON_SKILLS_AssignSkill(pUnit, (D2C_Skills)pMonStatsTxt->nSkill[i], (int32_t)nSkillBonus + (int8_t)pMonStatsTxt->nSkLvl[i], TRUE, szMonsterCpp, 575);
            D2COMMON_SKILLS_SetSkillMode(D2COMMON_SKILLS_GetHighestLevelSkillFromUnitAndId(pUnit, (D2C_Skills)pMonStatsTxt->nSkill[i]), (int8_t)pMonStatsTxt->nSkillMode[i]);
        }
    }
    D2COMMON_11062_Return(pUnit, (int32_t)nSkillBonus);

    pClassMonStatsTxt = DATATBLS_GetMonStatsTxtRecord(pUnit->dwClassId);
    if (pClassMonStatsTxt && (pClassMonStatsTxt->nMonStatsFlags[MONSTATSFLAGINDEX_NPC >> 3] & gdwBitMasks[MONSTATSFLAGINDEX_NPC & 7]))
    {
        return;
    }

    nDirection = 0;

    if (pUnit->dwUnitType == UNIT_MONSTER)
    {
        nBaseId = MONSTER_GetBaseId(pUnit->dwClassId);
        if (nBaseId < 0 || nBaseId >= sgptDataTables->nMonStatsTxtRecordCount)
            nBaseId = -1;
    }
    else
    {
        nBaseId = -1;
    }

    if (MONSTER_IsMonModeEnabled((D2C_MonsterIds)pUnit->dwClassId, MONMODE_WALK))
    {
        nDirection = (uint8_t)(SEED_RollRandomNumber(&pUnit->pSeed) & 0x3F);
    }

    if (nBaseId == (int32_t)MONSTER_MUMMY1)
        nDirection = 7;
    else if (nBaseId == (int32_t)MONSTER_VILECHILD1)
        nDirection = (uint8_t)MONSTER_ForEachUnitNearby(pUnit, 0, 0, MONSTER_MatchVileMotherDirection);

    if (nMonClassId == MONSTER_HYDRA1)
        nDirection = 24;
    else if (nMonClassId == MONSTER_HYDRA2)
        nDirection = 0;
    else if (nMonClassId == MONSTER_HYDRA3)
        nDirection = 40;
    else if (nMonClassId == MONSTER_VALKYRIE || nMonClassId == MONSTER_BONEWALL)
        nDirection = 0;

    D2COMMON_PATH_SetDirection(pDynamicPath, nDirection);
}


//1.10f: D2Client.0x6FB412D0
//  What a monster's class links it to, at its construction: a switch on the class id, two jump tables behind one-byte index tables. The
//  classes, by D2C_MonsterIds:
//    Kashya                       stored in gpKashyaUnit;
//    the two birds and the parrot the small no-presence collision pattern and the move-test mask COLLIDE_NONE;
//    act2guard2                   targets the player and faces it -- a tail call -- when there is a player;
//    bone wall                    a direction rolled from the PLAYER's seed, one of 64, set between D2Common #10203's two calls (TRUE clears
//                                 the path's flag 40h, FALSE sets it);
//    blade creeper                its blade creeper missile at its own position, owned by it;
//    the four catapults           one of five missiles from gnCatapultMissiles, linked both ways, given particles, attached and
//                                 UNITFLAGEX_NODRAW;
//    Baal's clone                 the baalteleport missile at its position.
//  From MONSTER_InitClientUnit the bone wall's roll is overwritten: that function's last act faces a bone wall 0. The roll still advances
//  the player's seed.
//  BUG: the act2guard2 arm tests the player and the bone wall's does not: a NULL player would fault reading +20h. LATENT: a monster is
//  constructed from a monster packet of a loaded game or as a client-only unit made in one.
//  The catapult roll is the low dword `% 5`, unsigned, then asserted below scnCatapultMissiles with a signed compare -- an assert no value
//  can fire. The spawn position is read Y first, then X, in every arm. Callers: MONSTER_InitClientUnit and MONSTER_ChangeMonsterClass. The
//  original inlines SEED_RollRandomNumber and UNITS_GetYPosition here.
void __fastcall MONSTER_LinkOwnedUnits(D2UnitStrc *pUnit, D2C_MonsterIds nClassId)
{
    D2UnitStrc *pPlayer;
    D2UnitStrc *pMissile;
    D2DynamicPathStrc *pDynamicPath;
    uint8_t nDirection;
    int32_t nMissileIndex;
    int32_t nMissileId;
    int32_t nX;
    int32_t nY;

    switch (nClassId)
    {
    case MONSTER_KASHYA:
        gpKashyaUnit = pUnit;
        break;

    case MONSTER_BIRD1:
    case MONSTER_BIRD2:
    case MONSTER_PARROT:
        D2COMMON_PATH_SetUnitCollisionPattern(pUnit, COLLISION_PATTERN_SMALL_NO_PRESENCE);
        D2COMMON_PATH_SetMoveTestCollisionMask(pUnit->pDynamicPath, COLLIDE_NONE);
        break;

    case MONSTER_ACT2GUARD2:
        pPlayer = CUNIT_GetPlayer();
        if (pPlayer)
        {
            D2COMMON_UNITS_SetTargetUnitForDynamicUnit(pUnit, pPlayer);
            CUNIT_FaceUnitAtPathTarget(pUnit, 0);
        }
        break;

    case MONSTER_BONEWALL:
        pPlayer = CUNIT_GetPlayer();
        nDirection = (uint8_t)(SEED_RollRandomNumber(&pPlayer->pSeed) & 0x3F);
        pDynamicPath = pUnit->pDynamicPath;
        D2COMMON_10203_PATH_SetRotateFlag(pDynamicPath, TRUE);
        D2COMMON_PATH_SetDirection(pDynamicPath, nDirection);
        D2COMMON_10203_PATH_SetRotateFlag(pDynamicPath, FALSE);
        break;

    case MONSTER_BLADECREEPER:
        nY = UNITS_GetYPosition(pUnit);
        nX = UNITS_GetXPosition(pUnit);
        pMissile = MISSILE_ClientSpawnSimple(pUnit, MISSILE_BLADE_CREEPER, nX, nY, SKILL_ATTACK, 1);
        if (pMissile)
            D2COMMON_UNITS_StoreOwner(pMissile, pUnit);
        break;

    case MONSTER_CATAPULT1:
    case MONSTER_CATAPULT2:
    case MONSTER_CATAPULT3:
    case MONSTER_CATAPULT4:
        nMissileIndex = (int32_t)((uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) % scnCatapultMissiles);
        if (nMissileIndex >= scnCatapultMissiles)
        {
            FOG_DisplayAssert("nMissileIndex < scnCatapultMissiles", szMonsterCpp, 372);
            exit(-1);
        }

        nMissileId = gnCatapultMissiles[nMissileIndex];
        nY = UNITS_GetYPosition(pUnit);
        nX = UNITS_GetXPosition(pUnit);
        pMissile = MISSILE_ClientSpawnSimple(pUnit, nMissileId, nX, nY, SKILL_ATTACK, 1);
        if (pMissile)
        {
            D2COMMON_UNITS_StoreOwner(pMissile, pUnit);
            D2COMMON_UNITS_StoreOwner(pUnit, pMissile);
            PARTICLE_AllocUnitParticles(pMissile);
            PARTICLE_SetUnitAttached(pMissile, 1);
            MONSTER_SetFlagEx(pMissile, UNITFLAGEX_NODRAW, 1);
        }
        break;

    case MONSTER_BAALCLONE:
        nY = UNITS_GetYPosition(pUnit);
        nX = UNITS_GetXPosition(pUnit);
        MISSILE_ClientSpawnSimple(pUnit, MISSILE_BAALTELEPORT, nX, nY, SKILL_ATTACK, 1);
        break;
    }
}


//1.10f: D2Client.0x6FB41690
//  Whether this is the vile mother the child came from: a MONSTER_ForEachUnitNearby callback, whose one reference is
//  MONSTER_InitClientUnit. It answers the other unit's direction when that unit is a monster of BaseId vilemother1, at a distance of at most
//  1 (signed compare), and the child stands exactly three steps from it the way it faces: D2Common #11053 brings the 64 directions to 8, the
//  table below turns them, and #11055 gives the step. Otherwise 0 -- which a mother facing 0 also answers. MONSTER_ForEachUnitNearby stops
//  at the first non-zero answer and answers 0 when there is none, so the walk goes on past a mother facing 0. The answer is the direction
//  byte zero-extended.
//  `retn 8`: the callback's two stack arguments, pUnused1 and pUnused2 -- the walker's own second and third arguments, 0 and 0 from
//  MONSTER_InitClientUnit -- are purged and never read. The expected position is the mother's plus twice the step plus the step, computed for
//  both axes before the child's are read. The original inlines MONSTER_GetBaseMonsterId and UNITS_GetXPosition here.
//  BUG: the server spits the child three steps away along the direction to the mother's target, and this test follows the mother's own
//  facing through {6, 7, 0, 1, 2, 3, 4, 5} instead; where the two disagree the match fails and the child faces 0. REACHABLE: stock
//  levels.txt places vilemother1..5 in Act 4 and Act 5 areas.
int32_t __fastcall MONSTER_MatchVileMotherDirection(D2UnitStrc *pChild, D2UnitStrc *pOther, void *pUnused1, void *pUnused2)
{
    uint8_t aDirRemap[8];
    uint8_t nMotherDirection;
    int32_t nBaseId;
    int32_t nStepX;
    int32_t nStepY;
    int32_t nExpectedX;
    int32_t nExpectedY;

    (void)pUnused1;
    (void)pUnused2;

    nBaseId = MONSTER_GetBaseMonsterId(pOther);
    if (nBaseId < 0 || nBaseId >= sgptDataTables->nMonStatsTxtRecordCount || nBaseId != (int32_t)MONSTER_VILEMOTHER1)
        return 0;

    if (D2COMMON_UNITS_GetDistanceToOtherUnit(pChild, pOther) > 1)
        return 0;

    nMotherDirection = D2COMMON_PATH_GetDirection(pOther->pDynamicPath);
    aDirRemap[0] = 6;
    aDirRemap[1] = 7;
    aDirRemap[2] = 0;
    aDirRemap[3] = 1;
    aDirRemap[4] = 2;
    aDirRemap[5] = 3;
    aDirRemap[6] = 4;
    aDirRemap[7] = 5;
    D2COMMON_11055(aDirRemap[D2COMMON_11053(nMotherDirection)], &nStepX, &nStepY);

    nExpectedX = UNITS_GetXPosition(pOther) + nStepX * 2 + nStepX;
    nExpectedY = UNITS_GetYPosition(pOther) + nStepY * 2 + nStepY;
    if (nExpectedX != UNITS_GetXPosition(pChild))
        return 0;
    if (nExpectedY != UNITS_GetYPosition(pChild))
        return 0;

    return nMotherDirection;
}


//1.10f: D2Client.0x6FB41860
//  Releases what hangs off a monster: its graphics, its inventory's items (keeping those another unit also holds, bKeepItemsHeldElsewhere 1)
//  and the inventory itself; while an act is loaded (gpAct), its collision footprint (D2Common #10228: the footprint, when it has a dynamic
//  path), its room and its light map; the dynamic path; the cast light D2Common hands back for NULL, through DLIGHTMAP_RemoveLight; and
//  the monster's name, pMonsterData +2Ch, FOG_Free'd (source line 102) and cleared. The one caller is CUNIT_DestroyUnit's UNIT_MONSTER arm.
//  FOG_Free takes the pointer and the path in ecx and edx.
void __fastcall MONSTER_FreeUnitData(D2UnitStrc *pUnit)
{
    D2ClientLightStrc *pLight;
    D2MonsterDataStrc *pMonsterData;

    if (!pUnit)
        return;

    GFX_FreeUnitGfxInfo(pUnit);
    CUNIT_FreeUnitInventory(pUnit, 1);
    D2COMMON_INVENTORY_FreeInventory(pUnit->pInventory);
    pUnit->pInventory = NULL;
    if (gpAct)
    {
        D2COMMON_10228(pUnit);
        D2COMMON_UNITROOM_RemoveUnitFromRoom(pUnit);
        CUNIT_GfxFreeUnitLightMap(pUnit);
    }

    D2COMMON_PATH_FreeDynamicPath(NULL, pUnit->pDynamicPath);
    pUnit->pDynamicPath = NULL;
    pLight = (D2ClientLightStrc *)D2COMMON_SKILLS_SwapCastLight(pUnit, NULL);
    if (pLight)
        DLIGHTMAP_RemoveLight(pLight);

    if (pUnit->dwUnitType == UNIT_MONSTER)
    {
        pMonsterData = pUnit->pMonsterData;
        if (pMonsterData && pMonsterData->wszMonName)
        {
            FOG_Free(pMonsterData->wszMonName, szMonsterCpp, 102, 0);
            pMonsterData->wszMonName = NULL;
        }
    }
}


//1.10f: D2Client.0x6FB41900
//  Turns a live monster into another class, in place. Two callers: STATES_SetFunc5_ChangeClass, which takes the class from the state's
//  STAT_SHORTPARAM1, and MONSTER_DoClientAction. A class with no monstats or no monstats2 record changes nothing: the two lookups and their
//  NULL tests jump to the epilogue before anything is freed. The original inlines DATATBLS_GetMonStatsTxtRecord,
//  DATATBLS_GetMonStats2TxtRecord, SEED_RollLimitedRandomNumber, MONSTER_BindMonStatsRecord and SEED_RollRandomNumber here.
//
//  The teardown is not MONSTER_FreeUnitData's: the graphics; the inventory's items, bKeepItemsHeldElsewhere 0 (the free passes 1); the
//  inventory; the light map, with no gpAct test; the cast light -- and no footprint, room or path. Both end with the name's free, written
//  in place in both (source line 102). Then the palette index is cleared and the class stored, and only then is the dynamic path tested:
//  a unit with none stops there, torn down, its new class stored, nothing rebuilt.
//
//  The build-up is a shorter MONSTER_InitClientUnit over the path the unit has: the velocity, monstats nVelocity << 8; the alive collision,
//  D2Common #10144 with bForGameLogic 0; an inventory for a monster flagged INTERACT (the `pInventory` test always passes, the field zeroed
//  earlier) and not for the four classes InitClientUnit adds; ATTACKRATE, VELOCITYPERCENT and ANIMRATE all 100, where InitClientUnit gives
//  VELOCITYPERCENT 75; the six resistances of the difficulty, each read signed, with no assert on the difficulty; MAXHP and HITPOINTS 32768,
//  which is 128 in the stats' 8-bit fixed point where InitClientUnit stores 100 << 8 (REFUTED as a defect: 128 is the client's scale,
//  packet 0xAC's MAXHP 0x8000 and 0xAB's `nLife << 8`); MONMODE_NEUTRAL; a start frame from the unit's seed; the flags; the monstats record
//  bound again; the owned units' links, the light, the unique modifiers (before the skill list here, after it in InitClientUnit) and the
//  eight monstats skills; a facing rolled from the seed unless the monster is an NPC; the graphics; the palette shift.
//  Not here: STAT_LEVEL, the classic scaling, the dead monster's collision, the skill bonus, the skill level's test (a slot whose level is
//  0 or below is assigned, only its skill id is tested), the skill mode, D2COMMON_11062_Return and the fixed facings by class. The skills go
//  in with bRemove 0 where InitClientUnit pushes 1.
//
//  The flags are InitClientUnit's, by the same monstats2 bits (ISSEL, SHADOW clearing UNITFLAG_DRAWSHADOW, UNITFLAG_ISVALIDTARGET set,
//  ISATT), each a byte against the low byte of Fog's mask, written back four times, with UNITFLAGEX_NODRAW cleared between the first two,
//  which InitClientUnit does not do.
//
//  The palette shift: nTransLvl + 2 when monstats nTransLvl is under 8 (an unsigned byte), else 2, as CUNIT_CreateMonsterUnit starts; then
//  by the unit's class, bounded first (-1 out of range): andariel, duriel, mephisto and diablo take 2, 3 or 4 by the difficulty;
//  diabloclone 5; drehyaiced (Anya in her ice) takes OVERLAY_ICE_CAGE (kind OVERLAYTYPE_STOP) and keeps its shift; and a shift of 8 or
//  more becomes 2 (signed compare), which only nTransLvl 6 and 7 can give. The second setter is a tail jump. The class tests are a switch
//  in the bytes' shape (`cmp eax, 0F3h / jg`, `cmp eax, 0F2h / jge`, then 9Ch and 37h subtracted), written as one switch.
void __fastcall MONSTER_ChangeMonsterClass(D2UnitStrc *pUnit, D2C_MonsterIds nNewMonClassId)
{
    D2MonStatsTxt *pMonStatsTxt;
    D2MonStats2Txt *pMonStats2Txt;
    D2ClientLightStrc *pLight;
    D2MonsterDataStrc *pMonsterData;
    D2DynamicPathStrc *pDynamicPath;
    uint8_t nDifficulty;
    int32_t nClassId;
    int32_t nShift;
    int32_t i;

    pMonStatsTxt = DATATBLS_GetMonStatsTxtRecord((int32_t)nNewMonClassId);
    if (!pMonStatsTxt)
        return;

    pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord((int32_t)nNewMonClassId);
    if (!pMonStats2Txt)
        return;

    GFX_FreeUnitGfxInfo(pUnit);
    CUNIT_FreeUnitInventory(pUnit, 0);
    D2COMMON_INVENTORY_FreeInventory(pUnit->pInventory);
    pUnit->pInventory = NULL;
    CUNIT_GfxFreeUnitLightMap(pUnit);
    pLight = (D2ClientLightStrc *)D2COMMON_SKILLS_SwapCastLight(pUnit, NULL);
    if (pLight)
        DLIGHTMAP_RemoveLight(pLight);

    //  The name's free, as at the end of MONSTER_FreeUnitData (the same source line).
    if (pUnit->dwUnitType == UNIT_MONSTER)
    {
        pMonsterData = pUnit->pMonsterData;
        if (pMonsterData && pMonsterData->wszMonName)
        {
            FOG_Free(pMonsterData->wszMonName, szMonsterCpp, 102, 0);
            pMonsterData->wszMonName = NULL;
        }
    }

    pDynamicPath = pUnit->pDynamicPath;
    pUnit->nPaletteIndex = 0;
    pUnit->dwClassId = nNewMonClassId;
    //  BUG: a unit with no dynamic path leaves here torn down, its new class stored and nothing rebuilt. UNREACHABLE: every client monster
    //  gets its path in MONSTER_InitClientUnit and keeps it until MONSTER_FreeUnitData.
    if (!pDynamicPath)
        return;

    //  `movsx` then `shl` in the original; written through unsigned, as in InitClientUnit.
    D2COMMON_PATH_SetVelocity(pDynamicPath, (int32_t)((uint32_t)pMonStatsTxt->nVelocity << 8), szMonsterCpp, 695);
    D2COMMON_PATH_SetUnitAliveCollision(pUnit, FALSE);
    if ((pMonStatsTxt->nMonStatsFlags[MONSTATSFLAGINDEX_INTERACT >> 3] & gdwBitMasks[MONSTATSFLAGINDEX_INTERACT & 7]) && !pUnit->pInventory)
    {
        pUnit->pInventory = D2COMMON_INVENTORY_AllocInventory(NULL, pUnit);
    }

    D2COMMON_STATLIST_AllocStatListEx(pUnit, 0, NULL, NULL);
    D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_ATTACKRATE, 100, 0);
    D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_VELOCITYPERCENT, 100, 0);
    D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_OTHER_ANIMRATE, 100, 0);
    nDifficulty = GAME_GetDifficulty();
    D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_DAMAGERESIST, (int16_t)pMonStatsTxt->nResistances[0][nDifficulty], 0);
    D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_MAGICRESIST, (int16_t)pMonStatsTxt->nResistances[1][nDifficulty], 0);
    D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_FIRERESIST, (int16_t)pMonStatsTxt->nResistances[2][nDifficulty], 0);
    D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_LIGHTRESIST, (int16_t)pMonStatsTxt->nResistances[3][nDifficulty], 0);
    D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_COLDRESIST, (int16_t)pMonStatsTxt->nResistances[4][nDifficulty], 0);
    D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_POISONRESIST, (int16_t)pMonStatsTxt->nResistances[5][nDifficulty], 0);
    D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_MAXHP, 128 << 8, 0);
    D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_HITPOINTS, 128 << 8, 0);
    D2COMMON_UNITS_ChangeAnimMode(pUnit, MONMODE_NEUTRAL);

    pUnit->nSeqCurrentFramePrecise = (int32_t)SEED_RollLimitedRandomNumberInline(&pUnit->pSeed, pUnit->dwFrameCountPrecise);

    if (pMonStats2Txt->nFlags[MONSTATS2FLAGINDEX_ISSEL >> 3] & gdwBitMasks[MONSTATS2FLAGINDEX_ISSEL & 7])
        pUnit->dwFlags |= UNITFLAG_TARGETABLE;
    else
        pUnit->dwFlags &= ~UNITFLAG_TARGETABLE;

    pUnit->dwFlagEx &= ~UNITFLAGEX_NODRAW;

    if (pMonStats2Txt->nFlags[MONSTATS2FLAGINDEX_SHADOW >> 3] & gdwBitMasks[MONSTATS2FLAGINDEX_SHADOW & 7])
        pUnit->dwFlags &= ~UNITFLAG_DRAWSHADOW;
    else
        pUnit->dwFlags |= UNITFLAG_DRAWSHADOW;

    pUnit->dwFlags |= UNITFLAG_ISVALIDTARGET;

    if (pMonStats2Txt->nFlags[MONSTATS2FLAGINDEX_ISATT >> 3] & gdwBitMasks[MONSTATS2FLAGINDEX_ISATT & 7])
        pUnit->dwFlags |= UNITFLAG_CANBEATTACKED;
    else
        pUnit->dwFlags &= ~UNITFLAG_CANBEATTACKED;

    if (pUnit->dwUnitType == UNIT_MONSTER)
    {
        pMonsterData = pUnit->pMonsterData;
        if (pMonsterData)
        {
            *(int16_t *)&pMonsterData->pAiControl = 0;

            pMonsterData->pMonstatsTxt = DATATBLS_GetMonStatsTxtRecord(pUnit->dwClassId);
        }
    }

    MONSTER_LinkOwnedUnits(pUnit, nNewMonClassId);
    MONSTER_UpdateClientLight(pUnit);
    MONUNIQUE_ApplyModifiers(pUnit);
    pUnit->pSkills = D2COMMON_SKILLS_AllocSkillList(NULL);

    //  The skills: a slot whose skill id is not negative (`test ax, ax / jl`), at its level sign-extended (`movsx`); ebp walks nSkill two
    //  bytes a slot while edi indexes nSkLvl.
    for (i = 0; i < 8; ++i)
    {
        if (pMonStatsTxt->nSkill[i] >= 0)
        {
            D2COMMON_SKILLS_AssignSkill(pUnit, (D2C_Skills)pMonStatsTxt->nSkill[i], (int8_t)pMonStatsTxt->nSkLvl[i], FALSE, szMonsterCpp, 745);
        }
    }

    if (!(pMonStatsTxt->nMonStatsFlags[MONSTATSFLAGINDEX_NPC >> 3] & gdwBitMasks[MONSTATSFLAGINDEX_NPC & 7]))
    {
        D2COMMON_PATH_SetDirection(pDynamicPath, (uint8_t)(SEED_RollRandomNumber(&pUnit->pSeed) & 0x3F));
    }

    UNITGFX_UnitInitGfx(pUnit);

    nShift = 2;
    if (pMonStatsTxt->nTransLvl < 8)
        nShift = pMonStatsTxt->nTransLvl + 2;

    nClassId = pUnit->dwClassId;
    if (nClassId < 0 || nClassId >= sgptDataTables->nMonStatsTxtRecordCount)
        nClassId = -1;

    switch (nClassId)
    {
    case MONSTER_ANDARIEL:
    case MONSTER_DURIEL:
    case MONSTER_MEPHISTO:
    case MONSTER_DIABLO:
        //  GAME_GetDifficulty's byte, `and eax, 0FFh`: 0 goes straight to the 2, 1 and 2 straight to the setters.
        switch (GAME_GetDifficulty())
        {
        case DIFFMODE_NORMAL:
            nShift = 2;
            break;
        case DIFFMODE_NIGHTMARE:
            nShift = 3;
            break;
        case DIFFMODE_HELL:
            nShift = 4;
            break;
        default:
            if (nShift >= 8)
                nShift = 2;
            break;
        }
        break;

    case MONSTER_DIABLOCLONE:
        nShift = 5;
        break;

    case MONSTER_DREHYAICED:
        GFX_AddOverlay(pUnit, OVERLAY_ICE_CAGE, OVERLAYTYPE_STOP, 0, 0, 0, 0, 0);
        if (nShift >= 8)
            nShift = 2;
        break;

    default:
        if (nShift >= 8)
            nShift = 2;
        break;
    }

    CUNIT_SetUnitPaletteShift1(pUnit, nShift);
    CUNIT_SetUnitPaletteShift2(pUnit, nShift);
}


//1.10f: D2Client.0x6FB41D70
//  Advances a monster's animation by its mode's descriptor. The one caller is MONSTER_UpdateAnimMode, with the descriptor it chose. NULL
//  answers 0. The original inlines DATATBLS_GetMonStatsTxtRecord here.
//  The special death comes first: a monster whose monstats nSplEndDeath is 1 (a byte read unsigned), in MONMODE_DEATH, at the end of its
//  frame cycle, runs MONSTER_DoClientAction(CLTACTION_DEAD) over seven zeroed dwords (`rep stosd`). A monster whose monstats lookup
//  refuses answers 0 there and never reaches the descriptor; a unit that is not a monster goes straight to it.
//  The descriptor's nAnimEnd, a switch whose bound is unsigned (`cmp eax, 3 / ja`):
//      0  D2Common's UNITS_StopSequence (#10373)
//      1  at the end of the frame cycle, answer UNITS_IsAtEndOfFrameCycle's own answer; otherwise UNITS_StopSequence
//      2  at the end of the cycle, freeze on the last frame -- wAnimSpeed 0 and the frame dwFrameCountPrecise - 256 -- and in
//         MONMODE_DEATH set UNITFLAG_DRAWSHADOW (`or al, 20h`); otherwise UNITS_StopSequence
//      3  rewind to frame 1 (256) and freeze
//  Only case 1 can answer anything but 0. The caller reads the answer for the descriptors whose nSplEndTrigger is 2 or 3.
//  BUG: gMonModeDescs[MONMODE_KNOCKBACK] is {1, 2, 2}, a trigger that reads an answer case 2 always makes 0. UNREACHABLE: no class takes
//  gMonModeDescsMoving[13], whose {0, 1, 2} could fire, because D2Common fills monstats2's moving bits from A1mv, A2mv, SCmv and S1mv..S4mv
//  alone, so MONSTER_DoClientSplEndEffect's knockback block never runs.
int32_t __fastcall MONSTER_AdvanceAnimForMode(D2UnitStrc *pUnit, const D2ClientMonModeDescStrc *pModeDesc)
{
    D2MonStatsTxt *pMonStatsTxt;
    int32_t pParams[7];
    int32_t bAnimEnded;

    if (!pUnit)
        return 0;

    if (pUnit->dwUnitType == UNIT_MONSTER)
    {
        pMonStatsTxt = DATATBLS_GetMonStatsTxtRecord(pUnit->dwClassId);
        if (!pMonStatsTxt)
            return 0;

        if (pMonStatsTxt->nSplEndDeath == 1 && pUnit->dwAnimMode == MONMODE_DEATH && D2COMMON_UNITS_IsAtEndOfFrameCycle(pUnit))
        {
            memset(pParams, 0, sizeof(pParams));
            MONSTER_DoClientAction(CLTACTION_DEAD, pUnit, pParams);
        }
    }

    bAnimEnded = 0;
    switch (pModeDesc->nAnimEnd)
    {
    case 0:
        D2COMMON_UNITS_StopSequence(pUnit);
        break;

    case 1:
        bAnimEnded = D2COMMON_UNITS_IsAtEndOfFrameCycle(pUnit);
        if (!bAnimEnded)
            D2COMMON_UNITS_StopSequence(pUnit);
        break;

    case 2:
        if (D2COMMON_UNITS_IsAtEndOfFrameCycle(pUnit))
        {
            pUnit->wAnimSpeed = 0;
            pUnit->nSeqCurrentFramePrecise = pUnit->dwFrameCountPrecise - 256;
            if (pUnit->dwAnimMode == MONMODE_DEATH)
                pUnit->dwFlags |= UNITFLAG_DRAWSHADOW;
        }
        else
        {
            D2COMMON_UNITS_StopSequence(pUnit);
        }
        break;

    case 3:
        //  BUG: case 3 is dead: no descriptor of either table has nAnimEnd 3. UNREACHABLE.
        pUnit->nSeqCurrentFramePrecise = 256;
        pUnit->wAnimSpeed = 0;
        break;
    }

    return bAnimEnded;
}


//1.10f: D2Client.0x6FB41E90
//  One client frame of a monster's animation -- the monster arm of UNITMODE_UpdateUnit, its one caller. Nothing for NULL, a unit that is not
//  a monster, a frozen one that is not dead (STATE_FREEZE, then CUNIT_IsUnitDead) or a mode of 16 or more (`jge`, signed, on the unsigned
//  field). The original inlines DATATBLS_GetMonStatsTxtRecord and UNITS_GetYPosition here.
//
//  The descriptor comes from gMonModeDescs or gMonModeDescsMoving, sixteen {nPathStep, nAnimEnd, nSplEndTrigger} each. Diablo and
//  diabloclone in MONMODE_SKILL4 take gMonModeDescsMoving[6]; baalthrone, baalcrab and baalclone in MONMODE_SKILL3 take [7] -- the classes
//  by hcIdx (D2C_MonsterIds), not by monstats.txt row. Otherwise a mode that monstats2's nModeEnabledWhenMovingFlags sets (`sar ecx, 3`)
//  takes its row of gMonModeDescsMoving, a sand leaper in MONMODE_DEATH takes that table's row 0 (IsSandLeaper is handed edi, the mode, 0
//  on that arm) and anything else gMonModeDescs, indexed by dwAnimMode read again. A class with no monstats2 record ends the frame there.
//  The type and the mode are tested a second time and the chosen row's address for NULL: by their shape an inlined helper's own checks,
//  which nothing in the bytes names and none of which can fail.
//
//  The used skill's frame, for a monster with a used skill and a skills.txt record: when the skill's flags have bit 1 and the unit steps
//  along its path (UNITMODE_StepUnitAlongPath), the skill takes flag 2 -- unless its SrvDoFunc is 67 -- and its client do-func runs. The
//  do-func runs at action frame 4 (a byte); or at action frame 1, 2 or 3 when the step did not run it and UNITFLAG_SKSRVDOFUNC is clear.
//  Otherwise, by SrvDoFunc, sign-extended (`movsx`):
//      95   the do-func; then a frame past monstats2 nInfernoAnim goes back to nInfernoRollback
//      126  the do-func; then frame 12 or later goes back to frame 8
//      152  while STATE_INFERNO is on: the do-func from nInfernoAnim on, and from nInfernoRollback on a frame one to four below it, from
//           SEED_RollLimitedRandomNumber called, not inlined
//  Skills.txt rows: 67 Charge and SerpentCharge; 95 FetishInferno, mon inferno sentry, Baal Inferno, MegademonInferno and Horror Arctic
//  Blast; 126 Imp Inferno; 152 DiabLight.
//
//  The neutral monster's target: in MONMODE_NEUTRAL, unless the unit is a monster whose pMonsterData +28h has bit 0 set (a byte; the word
//  InitClientUnit and ChangeMonsterClass store 0 and MONSTER_BindMonStatsRecord its argument; what sets the bit was not looked for), the
//  path's target unit is found again by its type and GUID, and D2Common #10213 runs when the unit has no path, the path no target, or the
//  target was found again -- not when the GUID names no unit. The slot that held the mode takes the target's type; the C keeps two
//  variables.
//
//  The monster turns to the player. A monster flagged INTERACT (a town NPC) that is not a CRITTER (monstats2), stands in MONMODE_NEUTRAL,
//  is not a mercenary (UNITFLAG_ISMERC) and is none of the three ancient statues faces the player on one frame in ten (`% 100`, unsigned,
//  then `cmp edx, 0Ah / jge`) while D2Common #10399 puts the two under 7 apart (`jnb`, unsigned). The roll's seed pointer is tested for NULL
//  first, which cannot fail. A class whose monstats lookup refuses ends the frame here, so nothing below runs for it.
//
//  The end: the descriptor's nSplEndTrigger decides MONSTER_DoClientSplEndEffect -- 1 the path step, 2 and 3 MONSTER_AdvanceAnimForMode's
//  answer, 4 the step and the end of the frame cycle; MONSTER_SpawnAttackMissile, MONUNIQUE_MonUModRunCltFuncsCol2 and, for a unit with a
//  path, D2Common's PATH_AdjustDirection (#10193) come between. Then the evil hut: in MONMODE_DEATH with UNITFLAG_DOUPDATE set, the flag is
//  cleared and a client-only OBJECT_CLIENTSMOKE is made where it stands, Y read before X; the active state functions when D2Common's state
//  mask says so; the mode's sound; the footstep.
void __fastcall MONSTER_UpdateAnimMode(D2UnitStrc *pUnit)
{
    const D2ClientMonModeDescStrc *pModeDesc;
    D2MonStatsTxt *pMonStatsTxt;
    D2MonStats2Txt *pMonStats2Txt;
    D2SkillStrc *pSkill;
    D2SkillsTxt *pSkillsTxt;
    D2MonsterDataStrc *pMonsterData;
    D2DynamicPathStrc *pDynamicPath;
    D2SeedStrc *pSeed;
    D2UnitStrc *pTarget;
    D2UnitStrc *pPlayer;
    D2UnitGUID dwTargetGUID;
    int32_t nTargetType;
    int32_t nAnimMode;
    int32_t nSkillFlags;
    int32_t nActionFrame;
    int32_t nFrame;
    int32_t nX;
    int32_t nY;
    BOOL bDoFuncPending;
    BOOL bStepped;
    int32_t bAnimEnded;
    BOOL bFireSplEnd;

    if (!pUnit || pUnit->dwUnitType != UNIT_MONSTER)
        return;

    if (D2COMMON_STATES_CheckState(pUnit, STATE_FREEZE) && !CUNIT_IsUnitDead(pUnit))
        return;

    //  `jge`, signed, both times: the casts keep the compare signed.
    nAnimMode = (int32_t)pUnit->dwAnimMode;
    if (nAnimMode >= (int32_t)NUMBER_OF_MONMODES)
        return;

    //  The second test of the type and the mode.
    if (pUnit->dwUnitType != UNIT_MONSTER || nAnimMode >= (int32_t)NUMBER_OF_MONMODES)
        return;

    if (pUnit->dwClassId == MONSTER_DIABLO && nAnimMode == MONMODE_SKILL4)
    {
        pModeDesc = &gMonModeDescsMoving[6];
    }
    else if (pUnit->dwClassId == MONSTER_DIABLOCLONE && nAnimMode == MONMODE_SKILL4)
    {
        pModeDesc = &gMonModeDescsMoving[6];
    }
    else if (pUnit->dwClassId == MONSTER_BAALTHRONE && nAnimMode == MONMODE_SKILL3)
    {
        pModeDesc = &gMonModeDescsMoving[7];
    }
    else if (pUnit->dwClassId == MONSTER_BAALCRAB && nAnimMode == MONMODE_SKILL3)
    {
        pModeDesc = &gMonModeDescsMoving[7];
    }
    else if (pUnit->dwClassId == MONSTER_BAALCLONE && nAnimMode == MONMODE_SKILL3)
    {
        pModeDesc = &gMonModeDescsMoving[7];
    }
    else
    {
        pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord(pUnit->dwClassId);
        if (!pMonStats2Txt)
            return;

        if (pMonStats2Txt->nModeEnabledWhenMovingFlags[nAnimMode >> 3] & gdwBitMasks[nAnimMode & 7])
            pModeDesc = &gMonModeDescsMoving[nAnimMode];
        else if (nAnimMode == MONMODE_DEATH && D2COMMON_MONSTERS_IsSandLeaper(pUnit, FALSE))
            pModeDesc = &gMonModeDescsMoving[0];
        else
            pModeDesc = &gMonModeDescs[pUnit->dwAnimMode];

        //  The row's address tested for NULL, not done for the borrowings above or the sand leaper's row.
        if (!pModeDesc)
            return;
    }

    if (pUnit->dwUnitType == UNIT_MONSTER)
    {
        pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
        if (pSkill)
        {
            pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szMonsterCpp, 948));
            if (pSkillsTxt)
            {
                nSkillFlags = D2COMMON_SKILLS_GetFlags(pSkill);
                bDoFuncPending = TRUE;
                if ((nSkillFlags & 1) && UNITMODE_StepUnitAlongPath(pUnit))
                {
                    if (pSkillsTxt->wSrvDoFunc != 67)
                        D2COMMON_SKILLS_SetFlags(pSkill, nSkillFlags | 2);
                    SKILLS_RunUsedSkillClientDoFunc(pUnit);
                    bDoFuncPending = FALSE;
                }

                nActionFrame = pUnit->nActionFrame;
                if (nActionFrame == 4 || (bDoFuncPending && !MONSTER_TestFlag(pUnit, UNITFLAG_SKSRVDOFUNC) && (nActionFrame == 1 || nActionFrame == 2 || nActionFrame == 3)))
                {
                    SKILLS_RunUsedSkillClientDoFunc(pUnit);
                }
                else
                {
                    switch ((int16_t)pSkillsTxt->wSrvDoFunc)
                    {
                    case 95:
                        pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord(pUnit->dwClassId);
                        if (pMonStats2Txt)
                        {
                            SKILLS_RunUsedSkillClientDoFunc(pUnit);
                            if ((pUnit->nSeqCurrentFramePrecise >> 8) > pMonStats2Txt->nInfernoAnim)
                                pUnit->nSeqCurrentFramePrecise = pMonStats2Txt->nInfernoRollback << 8;
                        }
                        break;

                    case 126:
                        SKILLS_RunUsedSkillClientDoFunc(pUnit);
                        if ((pUnit->nSeqCurrentFramePrecise & ~0xFF) >= (12 << 8))
                            pUnit->nSeqCurrentFramePrecise = 8 << 8;
                        break;

                    case 152:
                        pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord(pUnit->dwClassId);
                        if (pMonStats2Txt && D2COMMON_STATES_CheckState(pUnit, STATE_INFERNO))
                        {
                            nFrame = pUnit->nSeqCurrentFramePrecise >> 8;
                            if (nFrame >= pMonStats2Txt->nInfernoAnim)
                                SKILLS_RunUsedSkillClientDoFunc(pUnit);

                            //  `lea edx, [eax+ecx-4] / shl edx, 8`: written through unsigned, which shifts the same bits and is
                            //  defined below 0.
                            if (nFrame >= pMonStats2Txt->nInfernoRollback)
                            {
                                pUnit->nSeqCurrentFramePrecise = (int32_t)((uint32_t)(SEED_RollLimitedRandomNumber(&pUnit->pSeed, 4) + pMonStats2Txt->nInfernoRollback - 4) << 8);
                            }
                        }
                        break;
                    }
                }
            }
        }
    }

    bStepped = FALSE;
    if (pModeDesc->nPathStep == 1)
        bStepped = UNITMODE_StepUnitAlongPath(pUnit);

    if (nAnimMode == MONMODE_NEUTRAL)
    {
        pMonsterData = pUnit->dwUnitType == UNIT_MONSTER ? pUnit->pMonsterData : NULL;
        if (!pMonsterData || !(*(uint8_t *)&pMonsterData->pAiControl & 1))
        {
            pDynamicPath = pUnit->pDynamicPath;
            if (!pDynamicPath || !D2COMMON_PATH_GetTargetUnit(pDynamicPath))
            {
                D2COMMON_10213(pUnit);
            }
            else
            {
                D2COMMON_PATH_GetTargetTypeAndGUID(pDynamicPath, &nTargetType, &dwTargetGUID);
                pTarget = CUNIT_FindUnitByGuidAndType(dwTargetGUID, (D2C_UnitTypes)nTargetType);
                D2COMMON_PATH_SetTargetUnit(pDynamicPath, pTarget);
                if (pTarget)
                    D2COMMON_10213(pUnit);
            }
        }
    }

    pMonStatsTxt = DATATBLS_GetMonStatsTxtRecord(pUnit->dwClassId);
    if (!pMonStatsTxt)
        return;

    if (pMonStatsTxt->nMonStatsFlags[MONSTATSFLAGINDEX_INTERACT >> 3] & gdwBitMasks[MONSTATSFLAGINDEX_INTERACT & 7])
    {
        pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord(pUnit->dwClassId);
        if ((!pMonStats2Txt || !(pMonStats2Txt->nFlags[MONSTATS2FLAGINDEX_CRITTER >> 3] & gdwBitMasks[MONSTATS2FLAGINDEX_CRITTER & 7])) && pUnit->dwAnimMode == MONMODE_NEUTRAL && !(pUnit->dwFlags & UNITFLAG_ISMERC)
            && pUnit->dwClassId != MONSTER_ANCIENTSTATUE1 && pUnit->dwClassId != MONSTER_ANCIENTSTATUE2 && pUnit->dwClassId != MONSTER_ANCIENTSTATUE3)
        {
            pPlayer = CUNIT_GetPlayer();
            pSeed = &pUnit->pSeed;
            if (pPlayer && (uint32_t)D2COMMON_10399(pUnit, pPlayer) < 7 && pSeed && (int32_t)((uint32_t)SEED_RollRandomNumber(pSeed) % 100) < 10)
            {
                pDynamicPath = pUnit->pDynamicPath;
                if (pDynamicPath)
                {
                    nY = UNITS_GetYPosition(pPlayer);
                    nX = UNITS_GetXPosition(pPlayer);
                    D2COMMON_PATH_FaceCoords(pDynamicPath, nX, nY, 0);
                }
            }
        }
    }

    bAnimEnded = MONSTER_AdvanceAnimForMode(pUnit, pModeDesc);

    //  `dec ecx / cmp ecx, 3 / ja`: 1..4, unsigned.
    bFireSplEnd = FALSE;
    switch (pModeDesc->nSplEndTrigger)
    {
    case 1:
        bFireSplEnd = bStepped;
        break;

    case 2:
    case 3:
        bFireSplEnd = bAnimEnded;
        break;

    case 4:
        if (bStepped && D2COMMON_UNITS_IsAtEndOfFrameCycle(pUnit))
            bFireSplEnd = TRUE;
        break;
    }

    MONSTER_SpawnAttackMissile(pUnit);
    MONUNIQUE_MonUModRunCltFuncsCol2(pUnit);
    if (pUnit->pDynamicPath)
        D2COMMON_10193_PATH_AdjustDirection(pUnit->pDynamicPath);

    if (bFireSplEnd)
        MONSTER_DoClientSplEndEffect(pUnit);

    if (pUnit->dwAnimMode == MONMODE_DEATH && pUnit->dwClassId == MONSTER_EVILHUT && (pUnit->dwFlags & UNITFLAG_DOUPDATE))
    {
        pUnit->dwFlags &= ~UNITFLAG_DOUPDATE;
        nY = UNITS_GetYPosition(pUnit);
        nX = UNITS_GetXPosition(pUnit);
        CUNIT_CreateClientOnlyUnit(OBJECT_CLIENTSMOKE, nX, nY, UNIT_OBJECT, 0);
    }

    if (D2COMMON_STATES_CheckStateMaskActiveOnUnit(pUnit))
        SKILLS_StatesRunActiveStateFuncs(pUnit);

    UNITSND_UpdateUnitModeSound(pUnit);
    UNITSND_PlayFootstep(pUnit);
}


//1.10f: D2Client.0x6FB42420
//  The missile of a monster's attack frame. The one caller is MONSTER_UpdateAnimMode. D2Common #11050 with 1 (the `push eax` pushes the
//  unit type the test above found equal to 1) answers -1 unless this frame crossed an attack event frame, and otherwise the mode's missile
//  column of monstats.txt: MissA1, MissA2, MissC, MissS1..MissS4 or MissSQ by the mode. The original inlines UNITS_GetXPosition,
//  UNITS_GetYPosition, MONSTER_SetFlagEx and DATATBLS_GetMonStatsTxtRecord here.
//
//  The spawn block is 23 dwords zeroed (`mov ecx, 17h / rep stosd`): MISSILESPAWN_TARGETPOS, the unit its owner and origin, skill level 1.
//  The mummies' missiles -- MISSILE_MUMMY1..4, monstats MissA2 of unraveler1..5 and radament -- add MISSILESPAWN_VELOCITY and
//  MISSILESPAWN_VELOCITYSCALED with nVelocity 2; that range is tested signed and the enumerators are cast for it. The target is the path's
//  target unit's position or, with none, the path's first point; a unit with neither spawns nothing.
//
//  Then by the missile:
//      238, 239  the panther pots (slinger5's and slinger6's MissA1): MISSILE_ClientSpawnSkillEffect at skill level 1, and only when
//                there is a target unit
//      406       catapultchargedball on (catapult1..4's MissA1): the reload. The unit's owner -- the ball MONSTER_LinkOwnedUnits linked
//                both ways -- must be a missile (an assert) and goes to MISSILE_ClientOnHit; then one of gnCatapultMissiles' five is
//                rolled from the unit's seed -- 406, 410, 415, 418, 421, the charged, spike, cold, plague and meteor balls "on" --
//                spawned at the unit, linked both ways, given particles, attached and made UNITFLAGEX_NODRAW. The same block is
//                MONSTER_DoClientSplEndEffect's, and from the roll on LinkOwnedUnits' catapult arm
//      others    MISSILE_ClientSpawn of the block
//
//  The quill rat. Whatever the arm, a monster whose base id is quillrat1 (MONSTER_GetBaseId, then bounded by monstats' count) fires
//  monstats wAiParam[2][difficulty] more of the same missile -- its own class's record, not the base's -- from its position at the target
//  plus or minus 5 on each axis, each sign flipped by the low bit of a roll of a seed of its own: SEED_InitSeed, SEED_InitLowSeed
//  0x53454953 ('SEIS'), the high half the unit's id. The same seed every shot, so the same spread.
void __fastcall MONSTER_SpawnAttackMissile(D2UnitStrc *pUnit)
{
    D2MissileStrc tParams;
    D2SeedStrc tSeed;
    D2UnitStrc *pTarget;
    D2UnitStrc *pOwner;
    D2UnitStrc *pMissile;
    D2DynamicPathStrc *pDynamicPath;
    D2MonStatsTxt *pMonStatsTxt;
    int32_t nMissileId;
    int32_t nMissileIndex;
    int32_t nCatapultMissileId;
    int32_t nSpawnX;
    int32_t nSpawnY;
    int32_t nTargetXSaved;
    int32_t nTargetYSaved;
    int32_t nBaseId;
    int32_t nSpreadX;
    int32_t nSpreadY;
    int32_t nExtraLeft;

    if (!pUnit || pUnit->dwUnitType != UNIT_MONSTER)
        return;

    nMissileId = D2COMMON_11050(pUnit, 1);
    if (nMissileId == -1)
        return;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_TARGETPOS;
    if (nMissileId >= (int32_t)MISSILE_MUMMY1 && nMissileId <= (int32_t)MISSILE_MUMMY4)
    {
        tParams.dwFlags = (MISSILESPAWN_VELOCITY | MISSILESPAWN_VELOCITYSCALED | MISSILESPAWN_TARGETPOS);
        tParams.nVelocity = 2;
    }
    tParams.pOwner = pUnit;
    tParams.pOrigin = pUnit;
    tParams.nMissile = nMissileId;
    tParams.nSkillLevel = 1;

    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (pTarget)
    {
        tParams.nTargetX = UNITS_GetXPosition(pTarget);
        tParams.nTargetY = UNITS_GetYPosition(pTarget);
    }
    else
    {
        pDynamicPath = pUnit->pDynamicPath;
        if (!pDynamicPath)
            return;

        tParams.nTargetX = D2COMMON_10175_PathGetFirstPointX(pDynamicPath);
        tParams.nTargetY = D2COMMON_10176_PathGetFirstPointY(pDynamicPath);
    }

    if (nMissileId == MISSILE_PANTHERPOTORANGE || nMissileId == MISSILE_PANTHERPOTGREEN)
    {
        if (pTarget)
            MISSILE_ClientSpawnSkillEffect(pUnit, nMissileId, 0, 0, 0, 0, SKILL_ATTACK, 1, 0);
    }
    else if (nMissileId == MISSILE_CATAPULTCHARGEDBALL_ON)
    {
        pOwner = CUNIT_GetOwnerUnit(pUnit);
        if (pOwner)
        {
            if (pOwner->dwUnitType != UNIT_MISSILE)
            {
                FOG_DisplayAssert("UnitGetType (hMissile) == UNIT_MISSILE", szMonsterCpp, 353);
                exit(-1);
            }
            MISSILE_ClientOnHit(pOwner, NULL, FALSE);
        }

        nMissileIndex = (int32_t)((uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) % scnCatapultMissiles);
        if (nMissileIndex >= scnCatapultMissiles)
        {
            FOG_DisplayAssert("nMissileIndex < scnCatapultMissiles", szMonsterCpp, 372);
            exit(-1);
        }

        nCatapultMissileId = gnCatapultMissiles[nMissileIndex];
        nSpawnY = UNITS_GetYPosition(pUnit);
        nSpawnX = UNITS_GetXPosition(pUnit);
        pMissile = MISSILE_ClientSpawnSimple(pUnit, nCatapultMissileId, nSpawnX, nSpawnY, SKILL_ATTACK, 1);
        if (pMissile)
        {
            D2COMMON_UNITS_StoreOwner(pMissile, pUnit);
            D2COMMON_UNITS_StoreOwner(pUnit, pMissile);
            PARTICLE_AllocUnitParticles(pMissile);
            PARTICLE_SetUnitAttached(pMissile, 1);
            MONSTER_SetFlagEx(pMissile, UNITFLAGEX_NODRAW, 1);
        }
    }
    else
    {
        MISSILE_ClientSpawn(&tParams);
    }

    nTargetYSaved = tParams.nTargetY;
    nTargetXSaved = tParams.nTargetX;
    if (pUnit->dwUnitType != UNIT_MONSTER)
        return;

    nBaseId = MONSTER_GetBaseId(pUnit->dwClassId);
    if (nBaseId < 0 || nBaseId >= sgptDataTables->nMonStatsTxtRecordCount || nBaseId != MONSTER_QUILLRAT1)
        return;

    pMonStatsTxt = DATATBLS_GetMonStatsTxtRecord(pUnit->dwClassId);
    if (!pMonStatsTxt)
        return;

    nSpreadX = 5;
    nSpreadY = 5;
    D2COMMON_SEED_InitSeed(&tSeed);
    D2COMMON_SEED_InitLowSeed(&tSeed, 0x53454953);
    tSeed.nHighSeed = pUnit->dwUnitId;

    nExtraLeft = pMonStatsTxt->wAiParam[2][GAME_GetDifficulty()];
    //  BUG: the quill volley's count is 0 for every quill rat in the stock tables (wAiParam[2] is monstats' aip3, empty for quillrat1..8),
    //  so the volley below never fires in a stock client. UNREACHABLE with the stock tables.
    if (nExtraLeft <= 0)
        return;

    do
    {
        if ((uint32_t)SEED_RollRandomNumber(&tSeed) & 1)
            nSpreadX = -nSpreadX;
        if ((uint32_t)SEED_RollRandomNumber(&tSeed) & 1)
            nSpreadY = -nSpreadY;

        MISSILE_ClientSpawnForOwner(pUnit, nMissileId, nSpreadX + nTargetXSaved, nSpreadY + nTargetYSaved, SKILL_ATTACK, 1);
    }
    while (--nExtraLeft);
}


//1.10f: D2Client.0x6FB42750
//  What a monster's animation end does: the special end MONSTER_UpdateAnimMode fires by its descriptor's nSplEndTrigger, its one caller.
//  Nothing for NULL, a unit that is not a monster or a mode of 16 or more (`jnb`, unsigned: the field is unsigned, so the C compares as the
//  bytes do). The base monster is MONSTER_GetBaseMonsterId's answer, -1 outside monstats' count. The original inlines
//  MONSTER_GetBaseMonsterId, DATATBLS_GetMonStatsTxtRecord, UNITS_GetYPosition and MONSTER_SetFlagEx here.
//
//  Knockback ends here for anything but the sand leaper: with a path, the room recached, D2Common #10208's flag cleared, the path type put
//  back (PATH_ResetToPreviousType) and MONMODE_GETHIT set by a tail jump; with none, nothing. It never runs in a stock client (see the BUG
//  note of MONSTER_AdvanceAnimForMode): a knocked-back monster freezes on its last frame until the server's next action, whose
//  MONSTER_DoClientAction recaches the room and puts the path type back for a unit still in KNOCKBACK.
//
//  Otherwise the unit faces its path target when it has one, and the BASE monster's monstats nSplClientEnd (+1A7h, a byte) decides. Set,
//  the mode and the base class:
//      WALK             iceglobe, willowisp1: the mode stands
//      ATTACK1          trappedsoul1 and 2: MONMODE_SKILL1 from its first frame; catapult1: the reload of MONSTER_SpawnAttackMissile's
//                       406 arm, then the end below
//      SKILL1           vulture1: the mode stands; trappedsoul1 and 2 as in ATTACK1
//      ATTACK2, SKILL2  trappedsoul1 and 2 as in ATTACK1
//      SKILL3           batdemon1: MONMODE_SKILL4 from its first frame
//      SKILL4           batdemon1: the mode stands
//      DEAD             maggotqueen1: the mode stands
//      SEQUENCE         frogdemon1: the mode stands
//  Everything else (nSplClientEnd clear, another mode or class, the catapult after its reload) ends in MONMODE_NEUTRAL from its first
//  frame, unless the unit is not UNITFLAG_ISASYNC and has UNITFLAGEX_UNK_PATH_RELATED. The ids are hcIdx (D2C_MonsterIds).
void __fastcall MONSTER_DoClientSplEndEffect(D2UnitStrc *pMonster)
{
    D2MonStatsTxt *pBaseMonStatsTxt;
    D2DynamicPathStrc *pDynamicPath;
    D2UnitStrc *pOwner;
    D2UnitStrc *pMissile;
    uint32_t nAnimMode;
    int32_t nBaseMonsterId;
    int32_t nMissileIndex;
    int32_t nCatapultMissileId;
    int32_t nSpawnX;
    int32_t nSpawnY;

    if (!pMonster || pMonster->dwUnitType != UNIT_MONSTER)
        return;

    nAnimMode = pMonster->dwAnimMode;
    if (nAnimMode >= NUMBER_OF_MONMODES)
        return;

    nBaseMonsterId = MONSTER_GetBaseMonsterId(pMonster);
    if (nBaseMonsterId < 0 || nBaseMonsterId >= sgptDataTables->nMonStatsTxtRecordCount)
        nBaseMonsterId = -1;

    if (nAnimMode == MONMODE_KNOCKBACK && nBaseMonsterId != MONSTER_SANDLEAPER1)
    {
        pDynamicPath = pMonster->pDynamicPath;
        if (!pDynamicPath)
            return;

        D2COMMON_PATH_RecacheRoomIfNeeded(pDynamicPath);
        D2COMMON_10208_PathSetPathingFlag(pDynamicPath, FALSE);
        D2COMMON_PATH_ResetToPreviousType(pDynamicPath);
        UNITMODE_SetAnimMode(pMonster, MONMODE_GETHIT);
        return;
    }

    if (CUNIT_GetPathTargetUnit(pMonster))
        CUNIT_FaceUnitAtPathTarget(pMonster, FALSE);

    pBaseMonStatsTxt = DATATBLS_GetMonStatsTxtRecord(nBaseMonsterId);
    if (!pBaseMonStatsTxt)
        return;

    if (pBaseMonStatsTxt->nSplClientEnd)
    {
        switch (pMonster->dwAnimMode)
        {
        case MONMODE_WALK:
            if (nBaseMonsterId == MONSTER_ICEGLOBE || nBaseMonsterId == MONSTER_WILLOWISP1)
                return;
            break;

        case MONMODE_ATTACK1:
            if (nBaseMonsterId == MONSTER_TRAPPEDSOUL1 || nBaseMonsterId == MONSTER_TRAPPEDSOUL2)
            {
                //  The block the arms below jump to too.
                UNITMODE_SetAnimMode(pMonster, MONMODE_SKILL1);
                D2COMMON_UNITS_SetAnimStartFrame(pMonster);
                return;
            }

            if (nBaseMonsterId == MONSTER_CATAPULT1)
            {
                pOwner = CUNIT_GetOwnerUnit(pMonster);
                if (pOwner)
                {
                    if (pOwner->dwUnitType != UNIT_MISSILE)
                    {
                        FOG_DisplayAssert("UnitGetType (hMissile) == UNIT_MISSILE", szMonsterCpp, 353);
                        exit(-1);
                    }
                    MISSILE_ClientOnHit(pOwner, NULL, FALSE);
                }

                nMissileIndex = (int32_t)((uint32_t)SEED_RollRandomNumber(&pMonster->pSeed) % scnCatapultMissiles);
                if (nMissileIndex >= scnCatapultMissiles)
                {
                    FOG_DisplayAssert("nMissileIndex < scnCatapultMissiles", szMonsterCpp, 372);
                    exit(-1);
                }

                nCatapultMissileId = gnCatapultMissiles[nMissileIndex];
                nSpawnY = UNITS_GetYPosition(pMonster);
                nSpawnX = UNITS_GetXPosition(pMonster);
                pMissile = MISSILE_ClientSpawnSimple(pMonster, nCatapultMissileId, nSpawnX, nSpawnY, SKILL_ATTACK, 1);
                if (pMissile)
                {
                    D2COMMON_UNITS_StoreOwner(pMissile, pMonster);
                    D2COMMON_UNITS_StoreOwner(pMonster, pMissile);
                    PARTICLE_AllocUnitParticles(pMissile);
                    PARTICLE_SetUnitAttached(pMissile, 1);
                    MONSTER_SetFlagEx(pMissile, UNITFLAGEX_NODRAW, 1);
                }
            }
            break;

        case MONMODE_SKILL1:
            if (nBaseMonsterId == MONSTER_VULTURE1)
                return;
            //  Into the next arms' test.
        case MONMODE_ATTACK2:
        case MONMODE_SKILL2:
            if (nBaseMonsterId == MONSTER_TRAPPEDSOUL1 || nBaseMonsterId == MONSTER_TRAPPEDSOUL2)
            {
                UNITMODE_SetAnimMode(pMonster, MONMODE_SKILL1);
                D2COMMON_UNITS_SetAnimStartFrame(pMonster);
                return;
            }
            break;

        case MONMODE_SKILL3:
            if (nBaseMonsterId == MONSTER_BATDEMON1)
            {
                UNITMODE_SetAnimMode(pMonster, MONMODE_SKILL4);
                D2COMMON_UNITS_SetAnimStartFrame(pMonster);
                return;
            }
            break;

        case MONMODE_SKILL4:
            if (nBaseMonsterId == MONSTER_BATDEMON1)
                return;
            break;

        case MONMODE_DEAD:
            if (nBaseMonsterId == MONSTER_MAGGOTQUEEN1)
                return;
            break;

        case MONMODE_SEQUENCE:
            if (nBaseMonsterId == MONSTER_FROGDEMON1)
                return;
            break;
        }
    }

    //  `shr eax, 15h / and eax, 1` and `shr ecx, 0Dh / and ecx, 1`: each bit is also stored into a frame slot that is never read.
    if ((pMonster->dwFlags & UNITFLAG_ISASYNC) || !(pMonster->dwFlagEx & UNITFLAGEX_UNK_PATH_RELATED))
    {
        UNITMODE_SetAnimMode(pMonster, MONMODE_NEUTRAL);
        D2COMMON_UNITS_SetAnimStartFrame(pMonster);
    }
}


//1.10f: D2Client.0x6FB42A70
//  Blood off a unit: nCount splashes, or STAT_ITEM_EXTRABLOOD's value when the unit has it (D2Common #10520, layer 0). pUnit in ecx, nCount
//  in edx, the third argument on the stack (`retn 4`). The original inlines SEED_RollRandomNumber, DATATBLS_GetMonStats2TxtRecord,
//  UNITS_GetXPosition and SEED_RollLimitedRandomNumber here.
//  bByChance: non-zero, a roll of the unit's seed must come out 0 mod 25 first, one chance in 25; zero, no roll. Then a monster in
//  MONMODE_GETHIT rolls again, 1 in 3. The slot of the third argument holds the count from then on.
//  What is thrown: the class's monstats2 nBleed (a byte) is the number of blood-missile pairs -- 0, nothing; else MISSILE_BLOOD1 plus a
//  roll below 2 * nBleed, blood1, blood2, bigblood1 and bigblood2 being Missiles.txt 18 to 21.
//  Where: a moving unit (PATH_GetVelocity) aims within three of its eight facings -- D2Common's direction byte `shr 3`, plus a roll mod 7,
//  less 3, `and 7`; a standing one at any of eight (`and eax, 7`). From there it walks the ring gnBloodSpawnOffsetX/Y gives for a cell
//  COLLISION_CheckMask passes under COLLIDE_WALL.
void __fastcall MONSTER_SpawnBlood(D2UnitStrc *pUnit, int32_t nCount, int32_t bByChance)
{
    D2MonStats2Txt *pMonStats2Txt;
    D2ActiveRoomStrc *pRoom;
    uint32_t nFacing;
    int32_t nBloodCount;
    int32_t nExtraBlood;
    int32_t nBlood;
    int32_t nDirection;
    int32_t nStartDirection;
    int32_t nWalk;
    int32_t nSpawnX;
    int32_t nSpawnY;
    int32_t nVariant;

    if (bByChance && (uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) % 25 != 0)
        return;

    if (pUnit && pUnit->dwUnitType == UNIT_MONSTER && pUnit->dwAnimMode == MONMODE_GETHIT && (uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) % 3 != 0)
    {
        return;
    }

    nBloodCount = nCount;
    //  BUG: a NULL unit faults on every path: with bByChance its seed is read first; without, D2Common #10520 reads pUnit->pStatListEx
    //  untested, so neither of this function's own tests of pUnit saves it. Nor is the path tested before PATH_GetVelocity, which reads it
    //  untested: a unit with no dynamic path would fault there. LATENT for the NULL unit: each call site passes a unit it has tested or read.
    nExtraBlood = D2COMMON_STATLIST_UnitGetItemStatOrSkillStatValue(pUnit, STAT_ITEM_EXTRABLOOD, 0);
    if (nExtraBlood)
        nBloodCount = nExtraBlood;

    if (!pUnit)
        return;

    pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord(pUnit->dwClassId);
    if (!pMonStats2Txt)
        return;

    //  `test cl, cl / jbe`: a byte.
    if (!pMonStats2Txt->nBleed)
        return;

    for (nBlood = 0; nBlood < nBloodCount; ++nBlood)
    {
        if (D2COMMON_PATH_GetVelocity(pUnit->pDynamicPath))
        {
            nFacing = (uint32_t)D2COMMON_UNITS_GetDirection(pUnit) >> 3;
            nDirection = (int32_t)(((uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) % 7 + nFacing - 3) & 7);
        }
        else
        {
            nDirection = (int32_t)((uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) & 7);
        }

        pRoom = D2COMMON_UNITS_GetRoom(pUnit);
        nWalk = nDirection;
        nStartDirection = (nDirection - 1) & 7;
        if (nDirection == nStartDirection)
            return;

        //  BUG: the walk stops on coming round to the direction BEFORE the first, so it tests seven of the eight, and when all seven are
        //  walls the function RETURNS, so the splashes still to come are not thrown either; the test of the first direction against that
        //  one cannot succeed. NOT ESTABLISHED: whether a bleeding unit can stand with seven of the ring's cells walls was not measured.
        while (1)
        {
            nSpawnX = UNITS_GetXPosition(pUnit) + gnBloodSpawnOffsetX[nWalk];
            nSpawnY = UNITS_GetYPosition(pUnit) + gnBloodSpawnOffsetY[nWalk];
            if (!D2COMMON_COLLISION_CheckMask(pRoom, nSpawnX, nSpawnY, COLLIDE_WALL))
                break;

            nWalk = (nWalk + 1) & 7;
            if (nWalk == nStartDirection)
                return;
        }

        nVariant = SEED_RollLimitedRandomNumber(&pUnit->pSeed, 2 * pMonStats2Txt->nBleed);
        MISSILE_ClientSpawnForOwner(pUnit, (int32_t)MISSILE_BLOOD1 + nVariant, nSpawnX, nSpawnY, SKILL_ATTACK, 1);
    }
}


//1.10f: D2Client.0x6FB42D40
//  A monster takes a client action: the monster arm of the client action dispatch, PLAYER_DoClientAction's twin. nAction in ecx, pUnit in
//  edx, pParams on the stack (`retn 4`). The original inlines DATATBLS_GetMonStatsTxtRecord, DATATBLS_GetMonStats2TxtRecord,
//  MONSTER_GetBaseMonsterId, MONSTER_GetBaseId, MONSTER_IsMonModeEnabled, UNITS_GetXPosition and UNITS_GetYPosition here, and writes the
//  return to neutral and the ai-control test (`mov al, [reg+28h] / and eax, 1`) out in place.
//
//  The parameter block is seven ints. Read by the arms below: [0] and [1] a target's type and id, or a point; [2] GETHIT's hit points with
//  the aura bit, KNOCKBACK's distance byte, the prologue's step byte and NEUTRAL's hit points; [3] a path type; [4] a skill's level and the
//  tail's velocity; [5] KNOCKBACK's hit points and the prologue's distance byte; [6] the hit class. HIT takes its hit class from [0] and
//  its hit points from [1].
//
//  Before the switch, in order:
//    * a unit in MONMODE_KNOCKBACK has its path's room recached and its path type put back (PATH_ResetToPreviousType);
//    * a dead unit (CUNIT_IsUnitDead) taking anything but DEATH or DEAD gets its alive collision back (bForGameLogic FALSE), its light, and
//      loses UNITFLAGEX_NODRAW; a live one has its collision footprint added;
//    * the block is applied for every action but HIT, SKILL_UNIT and SKILL_XY, when there is one: [3] above 0 (signed) is set as the path
//      type, and for KNOCKBACK the low bytes of [5] and [2] become the path's new distance and step count; the used skill is cleared and
//      monumod.txt's column-0 client functions run. bParamBlockApplied remembers it for the tail;
//    * the class's monstats and monstats2 records: either missing returns at once, past the tail;
//    * the base monster: the record's nBaseId for a monster, -1 outside monstats' count or for any other type.
//
//  The switch is `cmp ebx, 1Dh / ja` (unsigned) over a byte table. The arms, in the order of their assert lines (the asserts name "ptParams"):
//    WALKTOXY, RUNTOXY      move flags set; refused for an ai-controlled monster or a move not started; MONMODE_WALK, MONMODE_RUN for
//                           RUNTOXY
//    WALKTOUNIT, RUNTOUNIT  the path target set by [0] and [1] first, then as above, the move to that unit
//    the _UNIT casts        (MONCAST, MONATTACK1, MONATTACK2 and MONSKILL1..4) the ice globe straight to the tail; otherwise the target set
//                           and faced, the finger mage's flames on its target for MONATTACK1, the vulture's flight state, five turns of the
//                           path of 64 directions (charsi 56, jamella 52 and 48 -- by class, not base -- warriv1 52, fara 4); the mode of
//                           gMonActionModeTable's row; and when that row moves the unit and monstats2 enables the mode while moving, the
//                           move to the target
//    the _XY casts          the path's target unit cleared; the unit turned toward a nonzero point it is not on; the mode and the move as
//                           above, to the point; then the vulture
//    GETHIT                 with no block, straight to the tail. Otherwise the aura ring by bit 7 of [2], whose low seven bits -- plus one
//                           above 1 -- are written back; nonzero, the hit points set to them << 8, or no blood when they already are; move
//                           flags cleared; the hit class from [6]; the room recached; MONMODE_GETHIT; one splash of blood; the component
//                           draw count; monumod column 3
//    HIT                    the aura ring and the seven bits as GETHIT's, on [1], the hit points set whenever they are nonzero; the hit
//                           class from [0]; one splash of blood by chance (1 in 25), the hit overlay and the mode sound of MONMODE_KNOCKBACK,
//                           monumod column 3. No move flags, and the mode is not changed
//    KNOCKBACK              hit points from [5], the hit class from [6]; the path's room recached, the path made PATHTYPE_KNOCKBACK_CLIENT,
//                           distance the low byte of [2], target the low words of [0] and [1], started (bAllowInTown FALSE);
//                           MONMODE_KNOCKBACK
//    BLOCK                  the room recached, the target set and faced, MONMODE_BLOCK
//    DEATH, DEAD            below
//    NEUTRAL                hit points from a nonzero [2]; TARGETABLE for a mercenary or a monstats2 ISSEL class, cleared otherwise;
//                           DRAWSHADOW by monstats2 SHADOW with the polarity MONSTER_InitClientUnit gives; alive again. With no block, the
//                           refusal; else the position synced to the low words of [0] and [1], and the refusal again for a unit within one
//                           of that point on both axes, an ai-controlled one, a move not started, a unit in STATE_ATTACHED or a class
//                           without MONMODE_WALK; else MONMODE_WALK
//    SKILL_UNIT, SKILL_XY   the skill [0] assigned at level [4]; its highest-level copy made the used skill, and its mode; for
//                           MONMODE_SEQUENCE every frame point of the sequence (the count `sar ebx, 8`) computed, its mode set and the unit's
//                           animation rebuilt for it; then [1] = -1, the mode, and the skill started on the unit or at the point. SKILL_XY
//                           clears the path's target unit first
//    INTERACT, OBJSETMODE, SKILL1MODE   move flags cleared, the refusal
//
//  The refusal (an ai-controlled monster, a move not started, NEUTRAL's tests, the default): a monster in a mode below 16 that is neither
//  DEATH nor DEAD turns to its path target, has its room recached and goes MONMODE_NEUTRAL. Fifteen jumps reach it from three arms (two from
//  each of the move arms, eleven from NEUTRAL), the default falls into it, and it ends in the tail.
//
//  The tail, every arm's end but the returns: with the block applied and a nonzero [4] that differs from STAT_VELOCITYPERCENT, the temporary
//  states deactivated, the stat moved to [4] and the animation rate updated; monumod column 1; and for an ai-controlled monster not in
//  MONMODE_NEUTRAL the refusal's block once more, then its path handed to D2Common #10153.
//
//  DEATH: move flags cleared; the hit class from [6]; the selection dropped as PLAYER_DoClientAction drops it; hit points 0. A unit under a
//  HIDE state loses TARGETABLE and gains DRAWSHADOW and NODRAW. A SHATTER: a monster throws the ice-break missile its monstats2 height
//  picks -- 271 plus the height, which the stock MonStats2.txt holds 0..4: MISSILE_MONGLACIALSPIKE for 0, ICEBREAKSMALL, MEDIUM and LARGE for
//  1..3, ICEBREAKSMOKE for 4 -- and MISSILE_ICEBREAKSMOKE, both from one zeroed D2MissileStrc, TARGETPOS and RANDOMDIR, owner and origin the
//  unit, level 1; and any unit's hit class takes the layer HITCLASS_Layer_SilentVoice (`and edx, 0Fh / or dl, 0A0h`). The dead collision is
//  cleared unless monstats2 says DEADCOL. Then the class's monstats record, again -- missing returns; nSplEndDeath 1 and a minion class in
//  range: a client-only copy of the unit's own class is made at its place, untargetable, given its hit class and MONMODE_DEATH, and the unit
//  becomes the minion (MONSTER_ChangeMonsterClass). Then by the base monster found before that change -- one switch, split at 356 in the
//  bytes (`cmp eax, 164h`, signed), so -1 is the default:
//      vulture1          its feathers overlay, DEATH; with a particle Z offset, the particle flagged, made to ignore its target, given the
//                        vector (0, 0, -16, scaled) and a zero particle vector
//      maggotegg1        in SEQUENCE: DEATH silently, at frame 14 (0xE00 in nSeqCurrentFramePrecise); else DEATH
//      maggotqueen1      SKILL_XY with SKILL_QUEENDEATH at level 1, through this function; the mode is not set
//      hydra1..3         every overlay removed, HYDRA_END1..3 by class, DEATH
//      dopplezon         MISSILE_DOPPLEZONEXPLODE at its target, DEATH
//      plaguepoppy, cycleoflife, vinecreature   MISSILE_VINE_BEAST_DEATH at its place, DEATH, sound 790
//      siegebeast1       turned round, `(direction + 32) % 64`, DEATH
//      minion1           a death sound of four by D2Common #11053 of its direction, `% 4`; DEATH
//      suicideminion1    its overlay and sound 2419, DEATH
//      catapult1         by class the S overlay for catapult1 and 3, the E one for catapult2, none for another; DEATH
//      baalclone, baalcrabstairs   MISSILE_BAALCLONEDEATH at its place, then as DRAWSHADOW, NODRAW, MONMODE_DEAD
//      willowisp1, thornhulk1, bonefetish1, cantor1, frogdemon1, tentacle1 and tentaclehead1, doomknight1..3, megademon1 and venomlord,
//      shadowwarrior and shadowmaster, succubus1 and succubuswitch1   their death overlay (bonefetish1 two), DEATH
//      any other         DEATH
//  and every arm then clears TARGETABLE (`and al, 0FDh`) and frees the light map.
//  The sound numbers are rows of the stock Sounds.txt: 790 druidpod_death_1, 1308/1311/1314/1317 minion_death_a1..d1, 2419
//  sorceress_fireball_impact_1.
//
//  DEAD: move flags cleared; the dead collision cleared unless DEADCOL; hit points 0; the local player's path target dropped when it is this
//  unit; the maggot queen in SEQUENCE takes SKILL_QUEENDEATH as its used skill and keeps its mode, anything else MONMODE_DEAD and the dead
//  flag; overlays removed but for duriel by class and catapult1, shadowwarrior and shadowmaster by base; the barricade tower's death
//  missile, marked dead (untested, which CUNIT_SetUnitDead tests itself); NODRAW for the bone wall, baalcrabstairs and the baal clone;
//  TARGETABLE cleared and DRAWSHADOW set; under a HIDE state CANBEATTACKED cleared (DEATH's hide clears TARGETABLE), DRAWSHADOW and NODRAW
//  set; the light map freed.
//
//  Four frame slots hold more than one value in the original (nAction's saved copy, the base id's, the frame's and the monstats2 record's);
//  the C keeps all of them apart.
void __fastcall MONSTER_DoClientAction(D2C_ClientActions nAction, D2UnitStrc *pUnit, int32_t *pParams)
{
    D2MonStatsTxt *pMonStatsTxt;
    D2MonStats2Txt *pMonStats2Txt;
    D2MonStats2Txt *pMoveMonStats2Txt;
    D2MonStats2Txt *pShatterMonStats2Txt;
    D2MonStatsTxt *pDeathMonStatsTxt;
    D2MonsterDataStrc *pMonsterData;
    D2DynamicPathStrc *pDynamicPath;
    D2UnitStrc *pPlayer;
    D2UnitStrc *pDeathCopy;
    D2SkillStrc *pSkill;
    D2AnimSeqTxt *pMonSeqTxt;
    D2MissileStrc tMissileParams;
    int32_t aActionParams[7];
    uint32_t nSeqMode;
    uint32_t nSeqFrame;
    int32_t nSeqDirection;
    int32_t nSeqEvent;
    BOOL bParamBlockApplied;
    BOOL bSpawnBlood;
    int32_t nBaseMonsterId;
    int32_t nTargetType;
    int32_t nMode;
    int32_t nSkillMode;
    int32_t nSkillId;
    int32_t nHitPoints;
    int32_t nShatterMissileId;
    int32_t nSeqFramePoints;
    int32_t iSeqFrame;
    int32_t nDeltaX;
    int32_t nDeltaY;
    int32_t nSpawnX;
    int32_t nSpawnY;
    int32_t nVelocityPercent;

    pDynamicPath = pUnit->pDynamicPath;
    if (pUnit->dwAnimMode == MONMODE_KNOCKBACK)
    {
        D2COMMON_PATH_RecacheRoomIfNeeded(pDynamicPath);
        D2COMMON_PATH_ResetToPreviousType(pDynamicPath);
    }

    if (CUNIT_IsUnitDead(pUnit))
    {
        if (nAction != CLTACTION_DEATH && nAction != CLTACTION_DEAD)
        {
            D2COMMON_PATH_SetUnitAliveCollision(pUnit, FALSE);
            MONSTER_UpdateClientLight(pUnit);
            pUnit->dwFlagEx &= ~(uint32_t)UNITFLAGEX_NODRAW;
        }
    }
    else
    {
        D2COMMON_PATH_AddCollisionFootprintForOptionalUnit(pUnit);
    }

    bParamBlockApplied = FALSE;
    if (nAction != CLTACTION_HIT && nAction != CLTACTION_SKILL_UNIT && nAction != CLTACTION_SKILL_XY && pParams)
    {
        bParamBlockApplied = TRUE;
        if (pParams[3] > 0)
        {
            D2COMMON_PATH_SetType(pDynamicPath, pParams[3]);
            if (nAction == CLTACTION_KNOCKBACK)
            {
                D2COMMON_PATH_SetNewDistance(pDynamicPath, (uint8_t)pParams[5]);
                D2COMMON_PATH_SetStepNum(pDynamicPath, (uint8_t)pParams[2]);
            }
        }

        D2COMMON_UNITS_SetUsedSkill(pUnit, NULL);
        MONUNIQUE_MonUModRunCltFuncsCol0(pUnit);
    }

    pMonStatsTxt = DATATBLS_GetMonStatsTxtRecord(pUnit->dwClassId);
    if (!pMonStatsTxt)
        return;

    pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord(pUnit->dwClassId);
    if (!pMonStats2Txt)
        return;

    if (pUnit->dwUnitType == UNIT_MONSTER)
    {
        nBaseMonsterId = MONSTER_GetBaseId(pUnit->dwClassId);
        if (nBaseMonsterId < 0 || nBaseMonsterId >= sgptDataTables->nMonStatsTxtRecordCount)
            nBaseMonsterId = -1;
    }
    else
    {
        nBaseMonsterId = -1;
    }

    switch (nAction)
    {
    case CLTACTION_WALKTOXY:
    case CLTACTION_RUNTOXY:
        if (!pParams)
        {
            FOG_DisplayAssert("ptParams", szMonsterCpp, 1737);
            exit(-1);
        }

        D2COMMON_10212_PATH_SetMoveFlags(pUnit, TRUE);
        pMonsterData = pUnit->dwUnitType == UNIT_MONSTER ? pUnit->pMonsterData : NULL;
        if (pMonsterData && (*(uint8_t *)&pMonsterData->pAiControl & 1))
            goto BackToNeutral;

        if (!UNITMODE_StartMoveToLocation(pUnit, pParams[0], pParams[1]))
            goto BackToNeutral;

        UNITMODE_SetAnimMode(pUnit, nAction == CLTACTION_WALKTOXY ? MONMODE_WALK : MONMODE_RUN);
        break;

    case CLTACTION_WALKTOUNIT:
    case CLTACTION_RUNTOUNIT:
        if (!pParams)
        {
            FOG_DisplayAssert("ptParams", szMonsterCpp, 1758);
            exit(-1);
        }

        D2COMMON_10212_PATH_SetMoveFlags(pUnit, TRUE);
        //  [0] is read once for both calls, [1] twice.
        nTargetType = pParams[0];
        CUNIT_SetUnitPathTargetById(pUnit, (D2C_UnitTypes)nTargetType, pParams[1]);
        pMonsterData = pUnit->dwUnitType == UNIT_MONSTER ? pUnit->pMonsterData : NULL;
        if (pMonsterData && (*(uint8_t *)&pMonsterData->pAiControl & 1))
            goto BackToNeutral;

        if (!UNITMODE_StartMoveToUnit(pUnit, nTargetType, pParams[1]))
            goto BackToNeutral;

        UNITMODE_SetAnimMode(pUnit, nAction == CLTACTION_WALKTOUNIT ? MONMODE_WALK : MONMODE_RUN);
        break;

    case CLTACTION_MONCAST_UNIT:
    case CLTACTION_MONATTACK1_UNIT:
    case CLTACTION_MONSKILL1_UNIT:
    case CLTACTION_MONSKILL2_UNIT:
    case CLTACTION_MONATTACK2_UNIT:
    case CLTACTION_MONSKILL3_UNIT:
    case CLTACTION_MONSKILL4_UNIT:
        if (!pParams)
        {
            FOG_DisplayAssert("ptParams", szMonsterCpp, 1782);
            exit(-1);
        }

        D2COMMON_10212_PATH_SetMoveFlags(pUnit, FALSE);
        //  Read before the ice globe's test.
        nTargetType = pParams[0];
        if (nBaseMonsterId == (int32_t)MONSTER_ICEGLOBE)
            break;

        CUNIT_SetUnitPathTargetById(pUnit, (D2C_UnitTypes)nTargetType, pParams[1]);
        if (nBaseMonsterId == (int32_t)MONSTER_FINGERMAGE1 && nAction == CLTACTION_MONATTACK1_UNIT)
            CUNIT_AddOverlay(CUNIT_GetPathTargetUnit(pUnit), OVERLAY_FINGERMAGEFLAMES, 0);

        CUNIT_FaceUnitAtPathTarget(pUnit, FALSE);
        if (nBaseMonsterId == (int32_t)MONSTER_VULTURE1)
            MONSTER_SetVultureFlightState(nAction, pUnit, pParams);
        else if (nBaseMonsterId == (int32_t)MONSTER_CHARSI && nAction == CLTACTION_MONSKILL1_UNIT)
            D2COMMON_10160_PathUpdateDirection(pUnit->pDynamicPath, 0x38);

        if (pUnit->dwClassId == MONSTER_JAMELLA && nAction == CLTACTION_MONSKILL1_UNIT)
            D2COMMON_10160_PathUpdateDirection(pUnit->pDynamicPath, 0x34);

        if (pUnit->dwClassId == MONSTER_JAMELLA && nAction == CLTACTION_MONSKILL2_UNIT)
            D2COMMON_10160_PathUpdateDirection(pUnit->pDynamicPath, 0x30);

        if (nBaseMonsterId == (int32_t)MONSTER_WARRIV1)
        {
            if (nAction == CLTACTION_MONSKILL1_UNIT)
                D2COMMON_10160_PathUpdateDirection(pUnit->pDynamicPath, 0x34);
        }
        else if (nBaseMonsterId == (int32_t)MONSTER_FARA && nAction == CLTACTION_MONSKILL1_UNIT)
        {
            D2COMMON_10160_PathUpdateDirection(pUnit->pDynamicPath, 4);
        }

        //  The row's mode, read again after the call and compared signed (`jge`), shifted `sar`.
        UNITMODE_SetAnimMode(pUnit, gMonActionModeTable[2 * nAction]);
        if (gMonActionModeTable[2 * nAction + 1])
        {
            nMode = gMonActionModeTable[2 * nAction];
            if (nMode < (int32_t)NUMBER_OF_MONMODES)
            {
                pMoveMonStats2Txt = DATATBLS_GetMonStats2TxtRecord(pUnit->dwClassId);
                if (pMoveMonStats2Txt && (pMoveMonStats2Txt->nModeEnabledWhenMovingFlags[nMode >> 3] & gdwBitMasks[nMode & 7]))
                {
                    UNITMODE_StartMoveToUnit(pUnit, nTargetType, pParams[1]);
                }
            }
        }
        break;

    case CLTACTION_MONCAST_XY:
    case CLTACTION_MONATTACK1_XY:
    case CLTACTION_MONSKILL1_XY:
    case CLTACTION_MONSKILL2_XY:
    case CLTACTION_MONATTACK2_XY:
    case CLTACTION_MONSKILL3_XY:
    case CLTACTION_MONSKILL4_XY:
        if (!pParams)
        {
            FOG_DisplayAssert("ptParams", szMonsterCpp, 1832);
            exit(-1);
        }

        D2COMMON_10212_PATH_SetMoveFlags(pUnit, FALSE);
        D2COMMON_UNITS_SetTargetUnitForDynamicUnit(pUnit, NULL);
        if (pParams[0] || pParams[1])
        {
            if (pParams[0] != UNITS_GetXPosition(pUnit) || pParams[1] != UNITS_GetYPosition(pUnit))
                D2COMMON_UNITS_UpdateDirectionAndSpeed(pUnit, pParams[0], pParams[1]);
        }

        UNITMODE_SetAnimMode(pUnit, gMonActionModeTable[2 * nAction]);
        if (gMonActionModeTable[2 * nAction + 1])
        {
            nMode = gMonActionModeTable[2 * nAction];
            if (nMode < (int32_t)NUMBER_OF_MONMODES)
            {
                pMoveMonStats2Txt = DATATBLS_GetMonStats2TxtRecord(pUnit->dwClassId);
                if (pMoveMonStats2Txt && (pMoveMonStats2Txt->nModeEnabledWhenMovingFlags[nMode >> 3] & gdwBitMasks[nMode & 7]))
                {
                    UNITMODE_StartMoveToLocation(pUnit, pParams[0], pParams[1]);
                }
            }
        }

        if (nBaseMonsterId == (int32_t)MONSTER_VULTURE1)
            MONSTER_SetVultureFlightState(nAction, pUnit, pParams);
        break;

    case CLTACTION_GETHIT:
        if (!pParams)
            break;

        MONSTER_SetAuraRingEnabled(pUnit, pParams[2] & 0x80);
        nHitPoints = pParams[2] & 0x7F;
        pParams[2] = nHitPoints;
        if (nHitPoints > 1)
            pParams[2] = nHitPoints + 1;

        bSpawnBlood = TRUE;
        if (pParams[2])
        {
            //  `sar eax, 8`: the stat's 8.8 fixed point, signed.
            if (D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_HITPOINTS, 0) >> 8 == pParams[2])
                bSpawnBlood = FALSE;
            else
                D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_HITPOINTS, pParams[2] << 8, 0);
        }

        D2COMMON_10212_PATH_SetMoveFlags(pUnit, FALSE);
        pUnit->dwLastHitClass = pParams[6];
        D2COMMON_PATH_RecacheRoomIfNeeded(pDynamicPath);
        UNITMODE_SetAnimMode(pUnit, MONMODE_GETHIT);
        if (bSpawnBlood)
            MONSTER_SpawnBlood(pUnit, 1, FALSE);

        CUNIT_SetUnitComponentDrawCount(pUnit);
        MONUNIQUE_MonUModRunCltFuncsCol3(pUnit);
        break;

    case CLTACTION_HIT:
        if (!pParams)
        {
            FOG_DisplayAssert("ptParams", szMonsterCpp, 1896);
            exit(-1);
        }

        MONSTER_SetAuraRingEnabled(pUnit, pParams[1] & 0x80);
        nHitPoints = pParams[1] & 0x7F;
        pParams[1] = nHitPoints;
        if (nHitPoints > 1)
            pParams[1] = nHitPoints + 1;

        if (pParams[1])
            D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_HITPOINTS, pParams[1] << 8, 0);

        pUnit->dwLastHitClass = pParams[0];
        MONSTER_SpawnBlood(pUnit, 1, TRUE);
        CUNIT_SetUnitComponentDrawCount(pUnit);
        UNITMODE_PlayHitOverlayForMode(pUnit, MONMODE_KNOCKBACK);
        UNITSND_PlayModeSound(pUnit, MONMODE_KNOCKBACK, TRUE);
        MONUNIQUE_MonUModRunCltFuncsCol3(pUnit);
        break;

    case CLTACTION_KNOCKBACK:
        if (!pParams)
        {
            FOG_DisplayAssert("ptParams", szMonsterCpp, 1921);
            exit(-1);
        }

        if (pParams[5])
            D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_HITPOINTS, pParams[5] << 8, 0);

        pUnit->dwLastHitClass = pParams[6];
        D2COMMON_10212_PATH_SetMoveFlags(pUnit, FALSE);
        D2COMMON_PATH_RecacheRoomIfNeeded(pDynamicPath);
        D2COMMON_PATH_SetType(pDynamicPath, PATHTYPE_KNOCKBACK_CLIENT);
        D2COMMON_10190_PATH_SetDistance(pDynamicPath, (uint8_t)pParams[2]);
        //  `xor ecx, ecx / xor edx, edx / mov cx, [edi+4] / mov dx, [edi]`: the low words, zero-extended.
        D2COMMON_10170_PathSetTargetPos(pDynamicPath, (uint16_t)pParams[0], (uint16_t)pParams[1]);
        D2COMMON_PATH_StartUnitMove(pDynamicPath, pUnit, FALSE);
        UNITMODE_SetAnimMode(pUnit, MONMODE_KNOCKBACK);
        break;

    case CLTACTION_BLOCK:
        if (!pParams)
        {
            FOG_DisplayAssert("ptParams", szMonsterCpp, 1943);
            exit(-1);
        }

        D2COMMON_10212_PATH_SetMoveFlags(pUnit, FALSE);
        D2COMMON_PATH_RecacheRoomIfNeeded(pDynamicPath);
        CUNIT_SetUnitPathTargetById(pUnit, (D2C_UnitTypes)pParams[0], pParams[1]);
        CUNIT_FaceUnitAtPathTarget(pUnit, FALSE);
        UNITMODE_SetAnimMode(pUnit, MONMODE_BLOCK);
        break;

    case CLTACTION_DEATH:
        D2COMMON_10212_PATH_SetMoveFlags(pUnit, FALSE);
        //  BUG: DEATH reads [6] with no NULL test of the block, where every other arm that reads it asserts or tests it. LATENT: every caller
        //  that can hand DEATH passes a block (the packet handlers' stack int[7], the critter ticks'); the two that pass NULL (Item.cpp,
        //  Skills.cpp) pass CLTACTION_NEUTRAL.
        pUnit->dwLastHitClass = pParams[6];
        if (SELECT_GetSelectedUnit() == pUnit)
        {
            //  As PLAYER_DoClientAction's DEATH.
            if ((GAME_GetLeftClickModifierFlags() & 8) || (GAME_GetRightClickModifierFlags() & 8))
            {
                GAME_ResetClickState();
                SELECT_SetSelectedUnit(NULL);
            }
            else
            {
                SELECT_ClearSelectedUnit();
            }
        }

        D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_HITPOINTS, 0, 0);
        if (D2COMMON_STATES_CheckStateMaskHideOnUnit(pUnit))
        {
            pUnit->dwFlags = (pUnit->dwFlags & ~(uint32_t)UNITFLAG_TARGETABLE) | UNITFLAG_DRAWSHADOW;
            pUnit->dwFlagEx |= UNITFLAGEX_NODRAW;
        }

        if (D2COMMON_STATES_CheckStateMaskShatterOnUnit(pUnit))
        {
            if (pUnit->dwUnitType == UNIT_MONSTER)
            {
                pShatterMonStats2Txt = DATATBLS_GetMonStats2TxtRecord(pUnit->dwClassId);
                //  BUG: the shatter's monstats2 lookup answers NULL (`xor eax, eax`) and the height is read off it at once, zero-extended
                //  (`xor edx, edx / mov dl`), before the clear -- a fault at 0x0B. UNREACHABLE: the prologue has already returned for a class
                //  with no record, and no call between is one of D2Client's three writers of a unit's class (CUNIT_InitUnit,
                //  CUNIT_CreateMonsterUnit, MONSTER_ChangeMonsterClass); whether a stat-list remove callback can fire there was not looked for.
                nShatterMissileId = (int32_t)MISSILE_ICEBREAKSMALL - 1 + pShatterMonStats2Txt->nHeight;
                memset(&tMissileParams, 0, sizeof(tMissileParams));
                tMissileParams.dwFlags = (MISSILESPAWN_TARGETPOS | MISSILESPAWN_RANDOMDIR);
                tMissileParams.pOwner = pUnit;
                tMissileParams.pOrigin = pUnit;
                tMissileParams.nSkillLevel = 1;
                tMissileParams.nMissile = nShatterMissileId;
                MISSILE_ClientSpawn(&tMissileParams);
                tMissileParams.nMissile = MISSILE_ICEBREAKSMOKE;
                MISSILE_ClientSpawn(&tMissileParams);
            }

            pUnit->dwLastHitClass = (pUnit->dwLastHitClass & 0xF) | HITCLASS_Layer_SilentVoice;
        }

        if (!(pMonStats2Txt->nFlags[MONSTATS2FLAGINDEX_DEADCOL >> 3] & gdwBitMasks[MONSTATS2FLAGINDEX_DEADCOL & 7]))
            D2COMMON_PATH_SetUnitDeadCollision(pUnit, FALSE);

        pDeathMonStatsTxt = DATATBLS_GetMonStatsTxtRecord(pUnit->dwClassId);
        if (!pDeathMonStatsTxt)
            return;

        //  `test ax, ax / jl`, then `movsx` against the count: wMinion[0] is signed.
        if (pDeathMonStatsTxt->nSplEndDeath == 1 && pDeathMonStatsTxt->wMinion[0] >= 0 && pDeathMonStatsTxt->wMinion[0] < sgptDataTables->nMonStatsTxtRecordCount)
        {
            nSpawnY = UNITS_GetYPosition(pUnit);
            nSpawnX = UNITS_GetXPosition(pUnit);
            pDeathCopy = CUNIT_CreateClientOnlyUnit(pUnit->dwClassId, nSpawnX, nSpawnY, UNIT_MONSTER, 0);
            if (pDeathCopy)
                pDeathCopy->dwFlags &= ~(uint32_t)UNITFLAG_TARGETABLE;

            //  BUG: the minion's client-only copy is tested for NULL before its flag is cleared and not before its hit class is written and
            //  its mode set. UNREACHABLE: CUNIT_CreateMonsterUnit answers NULL only for a class whose lookups refuse, and the copy's class is
            //  pUnit's own, which the prologue's lookups kept.
            pDeathCopy->dwLastHitClass = pUnit->dwLastHitClass;
            UNITMODE_SetAnimMode(pDeathCopy, MONMODE_DEATH);
            MONSTER_ChangeMonsterClass(pUnit, (D2C_MonsterIds)pDeathMonStatsTxt->wMinion[0]);
        }

        switch (nBaseMonsterId)
        {
        case MONSTER_VULTURE1:
            CUNIT_AddOverlay(pUnit, OVERLAY_VULTUREDEMONFEATHERS, 0);
            UNITMODE_SetAnimMode(pUnit, MONMODE_DEATH);
            if (PARTICLE_GetUnitParticleOffsetZ(pUnit))
            {
                PARTICLE_SetUnitParticleFlag(pUnit);
                PARTICLE_SetUnitRisesToTarget(pUnit, FALSE);
                PARTICLE_SetUnitGfxVector(pUnit, 0, 0, -16, TRUE);
                PARTICLE_SetUnitParticleVector(pUnit, 0, 0, 0, FALSE);
            }
            break;

        case MONSTER_WILLOWISP1:
            CUNIT_AddOverlay(pUnit, OVERLAY_WILLOWISPDEATH, 0);
            UNITMODE_SetAnimMode(pUnit, MONMODE_DEATH);
            break;

        case MONSTER_THORNHULK1:
            CUNIT_AddOverlay(pUnit, OVERLAY_THORNEDHULK_DEATH, 0);
            UNITMODE_SetAnimMode(pUnit, MONMODE_DEATH);
            break;

        case MONSTER_MAGGOTEGG1:
            if (pUnit->dwAnimMode == MONMODE_SEQUENCE)
            {
                UNITMODE_SetAnimModeSilent(pUnit, MONMODE_DEATH);
                pUnit->nSeqCurrentFramePrecise = 14 << 8;
            }
            else
            {
                UNITMODE_SetAnimMode(pUnit, MONMODE_DEATH);
            }
            break;

        case MONSTER_BONEFETISH1:
            CUNIT_AddOverlay(pUnit, OVERLAY_FETISHDEATH1, 0);
            CUNIT_AddOverlay(pUnit, OVERLAY_FETISHDEATH2, 0);
            UNITMODE_SetAnimMode(pUnit, MONMODE_DEATH);
            break;

        case MONSTER_CANTOR1:
            CUNIT_AddOverlay(pUnit, OVERLAY_ZAKARUMPRIEST_DEATH, 0);
            UNITMODE_SetAnimMode(pUnit, MONMODE_DEATH);
            break;

        case MONSTER_FROGDEMON1:
            CUNIT_AddOverlay(pUnit, OVERLAY_FROGDEMON_DEATH, 0);
            UNITMODE_SetAnimMode(pUnit, MONMODE_DEATH);
            break;

        case MONSTER_TENTACLE1:
        case MONSTER_TENTACLEHEAD1:
            CUNIT_AddOverlay(pUnit, OVERLAY_TENTACLEHEADBLOODRIPPLE, 0);
            UNITMODE_SetAnimMode(pUnit, MONMODE_DEATH);
            break;

        case MONSTER_MAGGOTQUEEN1:
            memset(aActionParams, 0, sizeof(aActionParams));
            aActionParams[0] = SKILL_QUEENDEATH;
            aActionParams[4] = 1;
            MONSTER_DoClientAction(CLTACTION_SKILL_XY, pUnit, aActionParams);
            break;

        case MONSTER_DOOMKNIGHT1:
        case MONSTER_DOOMKNIGHT2:
        case MONSTER_DOOMKNIGHT3:
            CUNIT_AddOverlay(pUnit, OVERLAY_UNDEADHORRORDEATH, 0);
            UNITMODE_SetAnimMode(pUnit, MONMODE_DEATH);
            break;

        case MONSTER_HYDRA1:
        case MONSTER_HYDRA2:
        case MONSTER_HYDRA3:
            GFX_RemoveAllOverlays(pUnit);
            //  `lea edx, [eax-0ADh]`, by class.
            CUNIT_AddOverlay(pUnit, pUnit->dwClassId - ((int32_t)MONSTER_HYDRA1 - (int32_t)OVERLAY_HYDRA_END1), 0);
            UNITMODE_SetAnimMode(pUnit, MONMODE_DEATH);
            break;

        case MONSTER_DOPPLEZON:
            SKILLS_MissileClientSpawnAimedAtTargetUnit(pUnit, MISSILE_DOPPLEZONEXPLODE, SKILL_ATTACK, 1);
            UNITMODE_SetAnimMode(pUnit, MONMODE_DEATH);
            break;

        case MONSTER_MEGADEMON1:
        case MONSTER_VENOMLORD:
            CUNIT_AddOverlay(pUnit, OVERLAY_MEGADEMONDEATH, 0);
            UNITMODE_SetAnimMode(pUnit, MONMODE_DEATH);
            break;

        case MONSTER_SHADOWWARRIOR:
        case MONSTER_SHADOWMASTER:
            CUNIT_AddOverlay(pUnit, OVERLAY_SHADOWWARRIORDEATH, 0);
            UNITMODE_SetAnimMode(pUnit, MONMODE_DEATH);
            break;

        case MONSTER_PLAGUEPOPPY:
        case MONSTER_CYCLEOFLIFE:
        case MONSTER_VINECREATURE:
            nSpawnY = UNITS_GetYPosition(pUnit);
            nSpawnX = UNITS_GetXPosition(pUnit);
            MISSILE_ClientSpawnSimple(pUnit, MISSILE_VINE_BEAST_DEATH, nSpawnX, nSpawnY, SKILL_ATTACK, 1);
            UNITMODE_SetAnimMode(pUnit, MONMODE_DEATH);
            //  Sounds.txt 790, druidpod_death_1.
            SOUNDINST_PlaySound(SOUND_DRUIDPOD_DEATH_1, pUnit, 0, 0, 0);
            break;

        case MONSTER_SIEGEBEAST1:
            //  `and eax, 0FFh / add eax, 20h / and eax, 8000003Fh` and the signed remainder's adjust, on the path the prologue read.
            D2COMMON_10160_PathUpdateDirection(pDynamicPath, (uint8_t)(((int32_t)D2COMMON_PATH_GetDirection(pDynamicPath) + 32) % PATH_NB_DIRECTIONS));
            UNITMODE_SetAnimMode(pUnit, MONMODE_DEATH);
            break;

        case MONSTER_MINION1:
            //  `and eax, 0FFh / and eax, 80000003h` and the signed remainder's adjust. Sounds.txt 1308, 1311, 1314, 1317: minion_death_a1,
            //  b1, c1, d1.
            switch ((int32_t)D2COMMON_11053(D2COMMON_UNITS_GetDirection(pUnit)) % 4)
            {
            case 0:
                SOUNDINST_PlaySound(SOUND_MINION_DEATH_A1, pUnit, 0, 0, 0);
                break;

            case 1:
                SOUNDINST_PlaySound(SOUND_MINION_DEATH_B1, pUnit, 0, 0, 0);
                break;

            case 2:
                SOUNDINST_PlaySound(SOUND_MINION_DEATH_C1, pUnit, 0, 0, 0);
                break;

            case 3:
                SOUNDINST_PlaySound(SOUND_MINION_DEATH_D1, pUnit, 0, 0, 0);
                break;
            }
            UNITMODE_SetAnimMode(pUnit, MONMODE_DEATH);
            break;

        case MONSTER_SUICIDEMINION1:
            CUNIT_AddOverlay(pUnit, OVERLAY_SUICIDE_DEATH, 0);
            //  Sounds.txt 2419, sorceress_fireball_impact_1.
            SOUNDINST_PlaySound(SOUND_SORCERESS_FIREBALL_IMPACT_1, pUnit, 0, 0, 0);
            UNITMODE_SetAnimMode(pUnit, MONMODE_DEATH);
            break;

        case MONSTER_SUCCUBUS1:
        case MONSTER_SUCCUBUSWITCH1:
            CUNIT_AddOverlay(pUnit, OVERLAY_SUCCUBAE_DEATH, 0);
            UNITMODE_SetAnimMode(pUnit, MONMODE_DEATH);
            break;

        case MONSTER_CATAPULT1:
            //  `sub eax, 1F1h / jz`, `dec eax / jz`, `dec eax / jnz`, by class.
            switch (pUnit->dwClassId)
            {
            case MONSTER_CATAPULT1:
            case MONSTER_CATAPULT3:
                CUNIT_AddOverlay(pUnit, OVERLAY_CATAPULT_DEATH_S, 0);
                break;

            case MONSTER_CATAPULT2:
                CUNIT_AddOverlay(pUnit, OVERLAY_CATAPULT_DEATH_E, 0);
                break;
            }
            UNITMODE_SetAnimMode(pUnit, MONMODE_DEATH);
            break;

        case MONSTER_BAALCLONE:
            nSpawnY = UNITS_GetYPosition(pUnit);
            nSpawnX = UNITS_GetXPosition(pUnit);
            MISSILE_ClientSpawnSimple(pUnit, MISSILE_BAALCLONEDEATH, nSpawnX, nSpawnY, SKILL_ATTACK, 1);
            //  Into the next arm.
        case MONSTER_BAALCRABSTAIRS:
            pUnit->dwFlags |= UNITFLAG_DRAWSHADOW;
            pUnit->dwFlagEx |= UNITFLAGEX_NODRAW;
            UNITMODE_SetAnimMode(pUnit, MONMODE_DEAD);
            break;

        default:
            UNITMODE_SetAnimMode(pUnit, MONMODE_DEATH);
            break;
        }

        //  `and al, 0FDh` on the whole dword.
        pUnit->dwFlags &= ~(uint32_t)UNITFLAG_TARGETABLE;
        CUNIT_GfxFreeUnitLightMap(pUnit);
        break;

    case CLTACTION_DEAD:
        D2COMMON_10212_PATH_SetMoveFlags(pUnit, FALSE);
        pPlayer = CUNIT_GetPlayer();
        if (!(pMonStats2Txt->nFlags[MONSTATS2FLAGINDEX_DEADCOL >> 3] & gdwBitMasks[MONSTATS2FLAGINDEX_DEADCOL & 7]))
            D2COMMON_PATH_SetUnitDeadCollision(pUnit, FALSE);

        D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_HITPOINTS, 0, 0);
        if (CUNIT_GetPathTargetUnit(pPlayer) == pUnit)
            D2COMMON_UNITS_SetTargetUnitForDynamicUnit(pPlayer, NULL);

        if (nBaseMonsterId == (int32_t)MONSTER_MAGGOTQUEEN1 && pUnit->dwAnimMode == MONMODE_SEQUENCE)
        {
            D2COMMON_UNITS_SetUsedSkill(pUnit, D2COMMON_SKILLS_GetHighestLevelSkillFromUnitAndId(pUnit, SKILL_QUEENDEATH));
        }
        else
        {
            UNITMODE_SetAnimMode(pUnit, MONMODE_DEAD);
            CUNIT_SetUnitDead(pUnit);
        }

        if (pUnit->dwClassId != MONSTER_DURIEL && nBaseMonsterId != (int32_t)MONSTER_CATAPULT1 && nBaseMonsterId != (int32_t)MONSTER_SHADOWWARRIOR && nBaseMonsterId != (int32_t)MONSTER_SHADOWMASTER)
        {
            GFX_RemoveAllOverlays(pUnit);
        }

        if (nBaseMonsterId == (int32_t)MONSTER_BARRICADETOWER)
        {
            nSpawnY = UNITS_GetYPosition(pUnit);
            nSpawnX = UNITS_GetXPosition(pUnit);
            //  The missile is passed untested: CUNIT_SetUnitDead tests it.
            CUNIT_SetUnitDead(MISSILE_ClientSpawnSimple(pUnit, MISSILE_TOWER_DEATH, nSpawnX, nSpawnY, SKILL_ATTACK, 1));
        }

        //  `sub eax, 158h / jz`, `sub eax, 0D7h / jz`, `sub eax, 0Bh / jnz`.
        if (nBaseMonsterId == (int32_t)MONSTER_BONEWALL || nBaseMonsterId == (int32_t)MONSTER_BAALCRABSTAIRS || nBaseMonsterId == (int32_t)MONSTER_BAALCLONE)
        {
            pUnit->dwFlagEx |= UNITFLAGEX_NODRAW;
        }

        //  `and al, 0FDh / or al, 20h`.
        pUnit->dwFlags = (pUnit->dwFlags & ~(uint32_t)UNITFLAG_TARGETABLE) | UNITFLAG_DRAWSHADOW;
        if (D2COMMON_STATES_CheckStateMaskHideOnUnit(pUnit))
        {
            //  `and ecx, 0FFFFFFFBh / or ecx, 20h`: CANBEATTACKED, where DEATH's hide clears TARGETABLE.
            pUnit->dwFlags = (pUnit->dwFlags & ~(uint32_t)UNITFLAG_CANBEATTACKED) | UNITFLAG_DRAWSHADOW;
            pUnit->dwFlagEx |= UNITFLAGEX_NODRAW;
        }

        CUNIT_GfxFreeUnitLightMap(pUnit);
        break;

    case CLTACTION_NEUTRAL:
        D2COMMON_10212_PATH_SetMoveFlags(pUnit, FALSE);
        if (pParams && pParams[2])
            D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_HITPOINTS, pParams[2] << 8, 0);

        //  `shr edx, 9 / and edx, 1`: the bit is stored into nAction's slot, which nothing reads after.
        if (pUnit->dwFlags & UNITFLAG_ISMERC)
            pUnit->dwFlags |= UNITFLAG_TARGETABLE;
        else if (pMonStats2Txt->nFlags[MONSTATS2FLAGINDEX_ISSEL >> 3] & gdwBitMasks[MONSTATS2FLAGINDEX_ISSEL & 7])
            pUnit->dwFlags |= UNITFLAG_TARGETABLE;
        else
            pUnit->dwFlags &= ~(uint32_t)UNITFLAG_TARGETABLE;

        if (pMonStats2Txt->nFlags[MONSTATS2FLAGINDEX_SHADOW >> 3] & gdwBitMasks[MONSTATS2FLAGINDEX_SHADOW & 7])
            pUnit->dwFlags &= ~(uint32_t)UNITFLAG_DRAWSHADOW;
        else
            pUnit->dwFlags |= UNITFLAG_DRAWSHADOW;

        CUNIT_SetUnitAlive(pUnit);
        if (!pParams)
            goto BackToNeutral;

        //  `mov dx, [edi+4]`, `mov dx, [edi]` with no `xor` before them: the callee's parameters are words.
        UNITMODE_SyncUnitPosition(pUnit, (uint16_t)pParams[0], (uint16_t)pParams[1], 0, 0, 0);

        nDeltaX = UNITS_GetXPosition(pUnit) - pParams[0];
        if (nDeltaX < 0)
            nDeltaX = -nDeltaX;

        if (nDeltaX <= 1)
        {
            nDeltaY = UNITS_GetYPosition(pUnit) - pParams[1];
            if (nDeltaY < 0)
                nDeltaY = -nDeltaY;

            if (nDeltaY <= 1)
                goto BackToNeutral;
        }

        pMonsterData = pUnit->dwUnitType == UNIT_MONSTER ? pUnit->pMonsterData : NULL;
        if (pMonsterData && (*(uint8_t *)&pMonsterData->pAiControl & 1))
            goto BackToNeutral;

        if (!UNITMODE_StartMoveToLocation(pUnit, pParams[0], pParams[1]))
            goto BackToNeutral;

        if (D2COMMON_STATES_CheckState(pUnit, STATE_ATTACHED))
            goto BackToNeutral;

        if (!MONSTER_IsMonModeEnabled((D2C_MonsterIds)pUnit->dwClassId, MONMODE_WALK))
            goto BackToNeutral;

        UNITMODE_SetAnimMode(pUnit, MONMODE_WALK);
        break;

    case CLTACTION_SKILL_UNIT:
        if (!pParams)
        {
            FOG_DisplayAssert("ptParams", szMonsterCpp, 2296);
            exit(-1);
        }

        D2COMMON_10212_PATH_SetMoveFlags(pUnit, FALSE);
        nSkillId = pParams[0];
        D2COMMON_SKILLS_AssignSkill(pUnit, (D2C_Skills)nSkillId, pParams[4], FALSE, szMonsterCpp, 2299);
        pSkill = D2COMMON_SKILLS_GetHighestLevelSkillFromUnitAndId(pUnit, (D2C_Skills)nSkillId);
        D2COMMON_UNITS_SetUsedSkill(pUnit, pSkill);
        nSkillMode = D2COMMON_SKILLS_GetSkillModeFromUnit(pUnit, pSkill);
        if (nSkillMode == MONMODE_SEQUENCE)
        {
            pMonSeqTxt = D2COMMON_DATATBLS_GetMonSeqTxtRecordFromUnit(pUnit);
            nSeqFramePoints = D2COMMON_DATATBLS_GetSeqFramePointsCount(pUnit) >> 8;
            for (iSeqFrame = 0; iSeqFrame < nSeqFramePoints; ++iSeqFrame)
            {
                D2COMMON_DATATBLS_ComputeSequenceAnimation(pMonSeqTxt, iSeqFrame << 8, iSeqFrame << 8, &nSeqMode, &nSeqFrame, &nSeqDirection, &nSeqEvent);
                pUnit->dwAnimMode = nSeqMode;
                GFX_RebuildUnitAnimForMode(pUnit, 0);
            }
        }

        pParams[1] = -1;
        UNITMODE_SetAnimMode(pUnit, nSkillMode);
        SKILLS_CltStartSkillOnUnit(pUnit, pParams);
        break;

    case CLTACTION_SKILL_XY:
        if (!pParams)
        {
            FOG_DisplayAssert("ptParams", szMonsterCpp, 2315);
            exit(-1);
        }

        D2COMMON_10212_PATH_SetMoveFlags(pUnit, FALSE);
        D2COMMON_UNITS_SetTargetUnitForDynamicUnit(pUnit, NULL);
        nSkillId = pParams[0];
        D2COMMON_SKILLS_AssignSkill(pUnit, (D2C_Skills)nSkillId, pParams[4], FALSE, szMonsterCpp, 2320);
        pSkill = D2COMMON_SKILLS_GetHighestLevelSkillFromUnitAndId(pUnit, (D2C_Skills)nSkillId);
        D2COMMON_UNITS_SetUsedSkill(pUnit, pSkill);
        nSkillMode = D2COMMON_SKILLS_GetSkillModeFromUnit(pUnit, pSkill);
        if (nSkillMode == MONMODE_SEQUENCE)
        {
            pMonSeqTxt = D2COMMON_DATATBLS_GetMonSeqTxtRecordFromUnit(pUnit);
            nSeqFramePoints = D2COMMON_DATATBLS_GetSeqFramePointsCount(pUnit) >> 8;
            for (iSeqFrame = 0; iSeqFrame < nSeqFramePoints; ++iSeqFrame)
            {
                D2COMMON_DATATBLS_ComputeSequenceAnimation(pMonSeqTxt, iSeqFrame << 8, iSeqFrame << 8, &nSeqMode, &nSeqFrame, &nSeqDirection, &nSeqEvent);
                pUnit->dwAnimMode = nSeqMode;
                GFX_RebuildUnitAnimForMode(pUnit, 0);
            }
        }

        pParams[1] = -1;
        UNITMODE_SetAnimMode(pUnit, nSkillMode);
        SKILLS_CltStartSkillAtCoords(pUnit, pParams);
        break;

    default:
        D2COMMON_10212_PATH_SetMoveFlags(pUnit, FALSE);
        //  Into the refusal, which fifteen jumps in three arms reach.
    BackToNeutral:
        if (pUnit->dwUnitType == UNIT_MONSTER && pUnit->dwAnimMode < NUMBER_OF_MONMODES && pUnit->dwAnimMode != MONMODE_DEATH && pUnit->dwAnimMode != MONMODE_DEAD)
        {
            CUNIT_FaceUnitAtPathTarget(pUnit, FALSE);
            UNITMODE_RecacheUnitRoom(pUnit);
            UNITMODE_SetAnimMode(pUnit, MONMODE_NEUTRAL);
        }
        break;
    }

    //  The tail.
    if (bParamBlockApplied && pParams && pParams[4])
    {
        nVelocityPercent = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_VELOCITYPERCENT, 0);
        if (nVelocityPercent != pParams[4])
        {
            D2COMMON_STATLIST_DeactivateTemporaryStates(pUnit);
            D2COMMON_STATLIST_AddUnitStat(pUnit, STAT_VELOCITYPERCENT, pParams[4] - nVelocityPercent, 0);
            D2COMMON_UNITS_UpdateAnimRateAndVelocity(pUnit, szMonsterCpp, 2349);
        }
    }

    MONUNIQUE_MonUModRunCltFuncsCol1(pUnit);
    pMonsterData = pUnit->dwUnitType == UNIT_MONSTER ? pUnit->pMonsterData : NULL;
    if (pMonsterData && (*(uint8_t *)&pMonsterData->pAiControl & 1))
    {
        if (pUnit->dwAnimMode != MONMODE_NEUTRAL)
        {
            if (pUnit->dwUnitType == UNIT_MONSTER && pUnit->dwAnimMode < NUMBER_OF_MONMODES && pUnit->dwAnimMode != MONMODE_DEATH && pUnit->dwAnimMode != MONMODE_DEAD)
            {
                CUNIT_FaceUnitAtPathTarget(pUnit, FALSE);
                UNITMODE_RecacheUnitRoom(pUnit);
                UNITMODE_SetAnimMode(pUnit, MONMODE_NEUTRAL);
            }
        }

        if (pDynamicPath)
            D2COMMON_10153(pDynamicPath);
    }
}


static_assert(offsetof(D2MonsterDataStrc, wNameSeed) == 0x14 && offsetof(D2MonsterDataStrc, nTypeFlag) == 0x16
                  && sizeof(D2MonsterDataStrc::nTypeFlag) == 2 && offsetof(D2MonsterDataStrc, nMonUmod) == 0x1C
                  && offsetof(D2MonsterDataStrc, wBossHcIdx) == 0x26
                  && offsetof(D2MonsterDataStrc, unk0x3C) + offsetof(D2UnkMonsterDataStrc, nMinionClassId) == 0x40,
              "the monster-data accessors, MONSTER_GetMonUmodArray to MONSTER_SetAuraRingEnabled: "
              "`mov [ecx+14h], dx` (0x6FB44980), `mov ax, [ecx+16h]` (0x6FB449E3), `or byte ptr "
              "[ecx+17h], 1` (0x6FB44A44), `lea eax, [ecx+1Ch]` (0x6FB44963), `mov [ecx+26h], dx` (0x6FB449A0); "
              "MONSTER_SetMinionSpawnClassId: `mov [eax+40h], ecx` (0x6FB448B6). The packet's members are "
              "pinned in SCmd.cpp");
static_assert(offsetof(D2HirelingTxt, nHirelingLevel) == 0x1C && offsetof(D2HirelingTxt, dwSkill) == 0x78
                  && offsetof(D2HirelingTxt, nMode) == 0xC0 && offsetof(D2HirelingTxt, nLevel) == 0xC6
                  && offsetof(D2HirelingTxt, nSLvlPerLvl) == 0xCC
                  && offsetof(D2HirelingInitStrc, nId) == 0 && offsetof(D2HirelingInitStrc, nLevel) == 4,
              "MONSTER_InitHireling: `sub esi, [ebx+1Ch]` (0x6FB4474A), `lea ecx, [ebx+78h]` (0x6FB4475B), the "
              "`movsx` of [esi+ebx+0C0h], [..+0C6h], [..+0CCh] (0x6FB447A1, 0x6FB4478A, 0x6FB4477D); the init "
              "block's nLevel and nId loaded at 0x6FB4472B and 0x6FB4472F for the txt lookup");


//1.10f: D2Client.0x6FB44460
//  The vulture takes off and lands. Both callers are MONSTER_DoClientAction's, each behind `nBaseMonsterId == MONSTER_VULTURE1` (vulture2..5
//  share that base): its unit-target arm and its point-target arm. Of the codes those arms take, three act here: CLTACTION_MONSKILL1_XY, 12,
//  lifts the bird; CLTACTION_MONSKILL2_XY, 14, and CLTACTION_MONATTACK1_UNIT, 10, put it down. Any other code does nothing. The original
//  inlines NPCMENU_SetUnitFlag and UNITS_GetXPosition / UNITS_GetYPosition here.
//  `retn 4`: one stack argument after ecx and edx. Both callers push their action block there and nothing here reads it (pUnusedParams).
//  Taking off: the three targeting flags off; COLLIDE_NO_PATH and then COLLIDE_MONSTER taken off the cell under the unit; the small
//  no-presence collision pattern and an empty move-test mask; then, only while the unit's particle offset is still 0, its particles, the
//  particle flag, "ignores target", a gfx vector of (0, 0, 2) scaled and a particle vector of (0, 0, 160). Landing: the flags back; both
//  bits set again with D2Common's COLLISION_SetMask; the small unit-presence pattern and COLLIDE_MASK_MONSTER_PATH; then, only for a unit
//  with an offset, the particle flag, "ignores target" off, a gfx vector of (0, 0, -8) scaled and a particle vector of zeros.
//  The bytes compute each cell's Y, then its X, then the room, then call: MSVC's right-to-left order for one call whose arguments are
//  calls, which is how each of the four lines is written. In three of them the two position accessors are inlined; in the landing's second
//  they are called.
//  BUG: the landing sets COLLIDE_NO_PATH as well as COLLIDE_MONSTER on the cell and asks nothing, where the server's landing first asks
//  whether the bird may land and marks the cell with COLLIDE_MONSTER alone. REACHABLE: the client lands on the server's action code, so the
//  question was asked there and the NO_PATH bit is the client's own; stock levels.txt places vulture1..5 in eight act 2, 3 and 5 levels.
//  BUG: the flag setter's NULL test is the setter's own; the unit is then read with none. UNREACHABLE: both callers pass
//  MONSTER_DoClientAction's unit, which that function reads at its fourth instruction.
void __fastcall MONSTER_SetVultureFlightState(D2C_ClientActions nAction, D2UnitStrc *pUnit, int32_t *pUnusedParams)
{
    if (nAction == CLTACTION_MONSKILL1_XY)
    {
        NPCMENU_SetUnitFlag(pUnit, UNITFLAG_TARGETABLE | UNITFLAG_CANBEATTACKED | UNITFLAG_ISVALIDTARGET, FALSE);
        D2COMMON_COLLISION_ResetMask(D2COMMON_UNITS_GetRoom(pUnit), UNITS_GetXPosition(pUnit), UNITS_GetYPosition(pUnit), COLLIDE_NO_PATH);
        D2COMMON_COLLISION_ResetMask(D2COMMON_UNITS_GetRoom(pUnit), UNITS_GetXPosition(pUnit), UNITS_GetYPosition(pUnit), COLLIDE_MONSTER);
        D2COMMON_PATH_SetUnitCollisionPattern(pUnit, COLLISION_PATTERN_SMALL_NO_PRESENCE);
        D2COMMON_PATH_SetMoveTestCollisionMask(pUnit->pDynamicPath, COLLIDE_NONE);
        if (!PARTICLE_GetUnitParticleOffsetZ(pUnit))
        {
            PARTICLE_AllocUnitParticles(pUnit);
            PARTICLE_SetUnitParticleFlag(pUnit);
            PARTICLE_SetUnitRisesToTarget(pUnit, TRUE);
            PARTICLE_SetUnitGfxVector(pUnit, 0, 0, 2, TRUE);
            PARTICLE_SetUnitParticleVector(pUnit, 0, 0, 160, FALSE);
        }
    }
    else if (nAction == CLTACTION_MONSKILL2_XY || nAction == CLTACTION_MONATTACK1_UNIT)
    {
        NPCMENU_SetUnitFlag(pUnit, UNITFLAG_TARGETABLE | UNITFLAG_CANBEATTACKED | UNITFLAG_ISVALIDTARGET, TRUE);
        D2COMMON_COLLISION_SetMask(D2COMMON_UNITS_GetRoom(pUnit), UNITS_GetXPosition(pUnit), UNITS_GetYPosition(pUnit), COLLIDE_NO_PATH);
        D2COMMON_COLLISION_SetMask(D2COMMON_UNITS_GetRoom(pUnit), UNITS_GetXPosition(pUnit), UNITS_GetYPosition(pUnit), COLLIDE_MONSTER);
        D2COMMON_PATH_SetUnitCollisionPattern(pUnit, COLLISION_PATTERN_SMALL_UNIT_PRESENCE);
        D2COMMON_PATH_SetMoveTestCollisionMask(pUnit->pDynamicPath, COLLIDE_MASK_MONSTER_PATH);
        if (PARTICLE_GetUnitParticleOffsetZ(pUnit))
        {
            PARTICLE_SetUnitParticleFlag(pUnit);
            PARTICLE_SetUnitRisesToTarget(pUnit, FALSE);
            PARTICLE_SetUnitGfxVector(pUnit, 0, 0, -8, TRUE);
            PARTICLE_SetUnitParticleVector(pUnit, 0, 0, 0, FALSE);
        }
    }
}


//1.10f: D2Client.0x6FB446B0
//  A hireling built on the client, from the eight bytes its caller fills (D2ClientHirelingInitStrc: the name's tbl index at +0, the server's
//  low seed at +4). Two callers: CUNIT_CreateMonsterUnit and ROSTERPETS_OnPetPacket81. Nothing for a NULL unit or block.
//  ISMERC and TARGETABLE go on the unit (`or edx, 202h` on the whole dword). D2Common's MONSTERS_GetActFromHirelingTxt finds the act by the
//  name, and MONSTERS_HirelingInit picks a record and a level from the seed the caller's block carries, the act, the game's difficulty and
//  the PLAYER, whose level the hireling's follows. The hireling.txt record of that id and level gives the skills: up to six, each at its txt
//  level plus its gain per level above the record's, in 1/32nds (`imul` / `sar eax, 5`), capped at 32, in its txt mode. The walk stops at
//  the first id that is 0 or less or past skills.txt (signed compares); a NULL record ends the function before the name. Then the name, and
//  the active items.
//  The name: the tbl index 1057 is replaced by 11021 (`cmp cx, 421h` / `mov ecx, 2B0Dh`): both are the key merca239, 1057 in string.tbl and
//  11021 in patchstring.tbl, so the client takes the patched text of one hireling's name. Any other index goes in cx with the high half of
//  the block's pointer above it; D2Lang #10004 takes a `short`, so the high half is not an argument.
//  The three bytes nMode, nLevel and nSLvlPerLvl are read signed (`movsx`), as int8_t[6]. Inert with the stock table (120 rows, Mode 1..14,
//  Level 1..27, LvlPerLvl 0..10).
//  BUG: the skills' level is clamped to 32 and nothing more, where the server clamps to 0..32, ends the walk at a mode of 16 or more, skips a
//  skill above the hireling's level and does not assign a level of 0. NOT ESTABLISHED: stock hireling.txt has no Mode of 16 or more and no
//  skill Level below 1, so at a level at or above its record's the two agree; but D2Common takes an id's first record even above the level
//  asked for, the first records start at Level 3, and whether a lower player's hireling comes out at 0 or below here was not traced.
//  BUG: CUNIT_GetPlayer's answer goes to MONSTERS_HirelingInit with no test, and that reads the level through STATLIST_UnitGetStatValue.
//  LATENT: both callers act on the local player's own hireling, a pet-roster record of type 7 that a server packet of a loaded game makes.
void __fastcall MONSTER_InitHireling(D2UnitStrc *pMonster, const D2ClientHirelingInitStrc *pHirelingInfo)
{
    D2HirelingInitStrc tHirelingInit;
    D2HirelingTxt *pHirelingTxt;
    uint32_t *pdwSkill;
    D2C_Acts nAct;
    D2C_StringIndices nNameStrId;
    int32_t nLevel;
    int32_t nLevelsAbove;
    int32_t iSkillSlot;
    int32_t nSkillId;
    int32_t nSkillLevel;
    int32_t nSkillMode;

    if (!pMonster || !pHirelingInfo)
        return;

    pMonster->dwFlags |= UNITFLAG_ISMERC | UNITFLAG_TARGETABLE;
    nAct = (D2C_Acts)D2COMMON_MONSTERS_GetActFromHirelingTxt(GAME_IsExpansionGame(), 0, pHirelingInfo->wNameId);
    if (!D2COMMON_MONSTERS_HirelingInit(GAME_IsExpansionGame(), CUNIT_GetPlayer(), pHirelingInfo->nLowSeed, nAct, (D2C_Difficulties)GAME_GetDifficulty(), &tHirelingInit))
        return;

    nLevel = tHirelingInit.nLevel;
    pHirelingTxt = D2COMMON_DATATBLS_GetHirelingTxtRecordFromIdAndLevel(GAME_IsExpansionGame(), tHirelingInit.nId, nLevel);
    if (!pHirelingTxt)
        return;

    //  `sub esi, [ebx+1Ch]` / `jns`: signed.
    nLevelsAbove = nLevel - pHirelingTxt->nHirelingLevel;
    if (nLevelsAbove < 0)
        nLevelsAbove = 0;

    pdwSkill = pHirelingTxt->dwSkill;
    for (iSkillSlot = 0; iSkillSlot < 6; ++iSkillSlot, ++pdwSkill)
    {
        nSkillId = (int32_t)*pdwSkill;
        if (nSkillId <= 0 || nSkillId >= sgptDataTables->nSkillsTxtRecordCount)
            break;

        nSkillLevel = pHirelingTxt->nLevel[iSkillSlot] + ((nLevelsAbove * pHirelingTxt->nSLvlPerLvl[iSkillSlot]) >> 5);
        if (nSkillLevel >= 32)
            nSkillLevel = 32;

        //  Read before the assignment, as the bytes read it (`movsx ebp`): the call could have changed the table for all the compiler knew.
        nSkillMode = pHirelingTxt->nMode[iSkillSlot];
        D2COMMON_SKILLS_AssignSkill(pMonster, (D2C_Skills)nSkillId, nSkillLevel, FALSE, szMonsterCpp, 2479);
        D2COMMON_SKILLS_SetSkillMode(D2COMMON_SKILLS_GetHighestLevelSkillFromUnitAndId(pMonster, (D2C_Skills)nSkillId), nSkillMode);
    }

    //  The index read again after the walk (`mov cx, [ecx]`).
    nNameStrId = (D2C_StringIndices)pHirelingInfo->wNameId;
    if (pHirelingInfo->wNameId == STR_IDX_1057_merca239)
        nNameStrId = STR_IDX_11021_merca239;

    D2COMMON_MONSTERS_SetMonsterNameInMonsterData(pMonster, D2LANG_GetStringFromTblIndex(nNameStrId));
    j_ITEM_RefreshActiveItems(pMonster);
}


//1.10f: D2Client.0x6FB44820
//  May this corpse be revived. FALSE for NULL, for a unit that is not a monster, and for a class D2Common's UNITS_CanSwitchAI refuses (whose 0
//  is returned as it came); then FALSE for a unique and for a superunique, TRUE for the rest. Two callers, each testing the answer:
//  SKILLS_CltStFunc24 and CUNIT_SkillsSelectProc3_Revive. The original inlines MONSTER_IsUnique here.
BOOL __fastcall MONSTER_CanBeRevived(D2UnitStrc *pUnit)
{
    if (!pUnit)
        return FALSE;

    if (pUnit->dwUnitType != UNIT_MONSTER)
        return FALSE;

    if (!D2COMMON_UNITS_CanSwitchAI(pUnit->dwClassId))
        return FALSE;

    if (MONSTER_IsUnique(pUnit))
        return FALSE;

    if (MONSTER_IsSuperUnique(pUnit))
        return FALSE;

    return TRUE;
}


//1.10f: D2Client.0x6FB44880
//  Packet 0x98: the class an evil hut spawns. The one caller is SCMD_HandleSrv98_MinionSpawnClass, with its stack copy of the seven bytes. The word
//  at +5 is the class of the monster the hut spawns, 0xFFFF for -1, stored on the hut at pMonsterData +0x40; the slot is a union with the life
//  percentage the server keeps there, and the store below names the client's arm. The dword store is -1 or the zero-extended word (`or ecx,
//  0FFFFFFFFh` / `and ecx, 0FFFFh`). No reader of it was found in D2Client.
void __fastcall MONSTER_SetMinionSpawnClassId(const D2GSPacketSrv98 *pPacket)
{
    D2UnitStrc *pMonster;
    int32_t nMinionClassId;

    pMonster = CUNIT_FindUnitByGuidAndType(pPacket->dwUnitGUID, UNIT_MONSTER);
    if (!pMonster)
        return;

    nMinionClassId = pPacket->nMinionClassId == 0xFFFF ? -1 : pPacket->nMinionClassId;
    if (pMonster->dwUnitType == UNIT_MONSTER && pMonster->pMonsterData)
        pMonster->pMonsterData->unk0x3C.nMinionClassId = nMinionClassId;
}


//1.10f: D2Client.0x6FB448C0
//  `pUnit->dwFlags & dwMask` as 0 or 1 (`neg / sbb / neg`), FALSE for NULL.
BOOL __fastcall MONSTER_TestFlag(D2UnitStrc *pUnit, uint32_t dwMask)
{
    if (!pUnit)
        return FALSE;

    return (pUnit->dwFlags & dwMask) != 0;
}


//1.10f: D2Client.0x6FB448E0
//  Sets or clears bits of dwFlagEx (+0xC8), nothing for NULL; `retn 4`, bSet on the stack.
void __fastcall MONSTER_SetFlagEx(D2UnitStrc *pUnit, int32_t nFlagEx, int32_t bSet)
{
    if (!pUnit)
        return;

    if (bSet)
        pUnit->dwFlagEx |= nFlagEx;
    else
        pUnit->dwFlagEx &= ~nFlagEx;
}


//1.10f: D2Client.0x6FB44910
//  A monster class's base class (monstats.txt's BaseId, read `movsx`), or the class itself when there is no record. MONSTER_InitClientUnit
//  and MONSTER_DoClientAction carry it inlined.
int32_t __fastcall MONSTER_GetBaseId(int32_t nMonClassId)
{
    D2MonStatsTxt *pMonStatsTxt;

    pMonStatsTxt = DATATBLS_GetMonStatsTxtRecord(nMonClassId);
    if (pMonStatsTxt)
        return pMonStatsTxt->nBaseId;

    return nMonClassId;
}


//1.10f: D2Client.0x6FB44950
//  The address of the ten monumod.txt ids at +0x1C. Callers: CUNIT_CreateMonsterUnit and SCMD_HandleSrv57_MonsterEnchants. This and the monster-data
//  accessors below have one shape: a NULL unit, one that is not UNIT_MONSTER or one with no pMonsterData gets nothing (a getter answers 0
//  or NULL); any other, one field. dwUnitType is the discriminant of the unit's data union, and the C takes the arm the test selects,
//  pMonsterData.
uint8_t *__fastcall MONSTER_GetMonUmodArray(D2UnitStrc *pUnit)
{
    if (pUnit && pUnit->dwUnitType == UNIT_MONSTER && pUnit->pMonsterData)
        return pUnit->pMonsterData->nMonUmod;

    return NULL;
}


//1.10f: D2Client.0x6FB44970
//  The name seed at +0x14 (`mov [ecx+14h], dx`). Callers: CUNIT_CreateMonsterUnit and SCMD_HandleSrv57_MonsterEnchants.
void __fastcall MONSTER_SetNameSeed(D2UnitStrc *pUnit, uint16_t wNameSeed)
{
    if (pUnit && pUnit->dwUnitType == UNIT_MONSTER && pUnit->pMonsterData)
        pUnit->pMonsterData->wNameSeed = wNameSeed;
}


//1.10f: D2Client.0x6FB44990
//  The superuniques.txt row at +0x26 (`mov [ecx+26h], dx`). The one caller is CUNIT_CreateMonsterUnit.
void __fastcall MONSTER_SetBossHcIdx(D2UnitStrc *pUnit, uint16_t wBossHcIdx)
{
    if (pUnit && pUnit->dwUnitType == UNIT_MONSTER && pUnit->pMonsterData)
        pUnit->pMonsterData->wBossHcIdx = wBossHcIdx;
}


//1.10f: D2Client.0x6FB449B0
//  The superuniques.txt row at +0x26, or 0 (`xor ax, ax`). Callers: CUNIT_CreateMonsterUnit and UNITSND_GetMonSoundsRecordForUnit, each
//  sign-extending the answer for DATATBLS_GetSuperUniquesTxtRecord.
//  BUG: the server's twin answers -1 instead of 0 for no data. UNREACHABLE: both callers ask MONSTER_TestTypeFlag's superunique bit first,
//  FALSE for exactly the units the 0 arm serves.
int16_t __fastcall MONSTER_GetBossHcIdx(D2UnitStrc *pUnit)
{
    if (pUnit && pUnit->dwUnitType == UNIT_MONSTER && pUnit->pMonsterData)
        return (int16_t)pUnit->pMonsterData->wBossHcIdx;

    return 0;
}


//1.10f: D2Client.0x6FB449D0
//  wMask against the sixteen bits at +0x16, as 0 or 1 (`mov ax, [ecx+16h] / and ax, dx / neg ax / sbb eax, eax / neg eax`). The word holds
//  nTypeFlag (+0x16) and the byte at +0x17, the aura-ring bit MONSTER_SetAuraRingEnabled writes (bit 0x100 of the word, MONTYPEFLAG_LIGHTNINGUNIQUE);
//  both are read as one word, so the C reads the word. wMask is D2C_MonTypeFlags, two bytes wide.
BOOL __fastcall MONSTER_TestTypeFlag(D2UnitStrc *pUnit, D2C_MonTypeFlags wMask)
{
    if (pUnit && pUnit->dwUnitType == UNIT_MONSTER && pUnit->pMonsterData)
        return (*(const uint16_t *)&pUnit->pMonsterData->nTypeFlag & wMask) != 0;

    return FALSE;
}


//1.10f: D2Client.0x6FB44A00
//  Sets or clears bits of the same word (`or [eax+16h], dx` / `not edx / and [eax+16h], dx`); `retn 4`, bSet on the stack. Callers:
//  CUNIT_CreateMonsterUnit (bSet 1 and the packet's flag word) and SCMD_HandleSrv57_MonsterEnchants.
void __fastcall MONSTER_SetTypeFlag(D2UnitStrc *pUnit, D2C_MonTypeFlags wMask, int32_t bSet)
{
    if (pUnit && pUnit->dwUnitType == UNIT_MONSTER && pUnit->pMonsterData)
    {
        uint16_t *pwTypeFlags = (uint16_t *)&pUnit->pMonsterData->nTypeFlag;
        if (bSet)
            *pwTypeFlags |= wMask;
        else
            *pwTypeFlags &= ~wMask;
    }
}


//1.10f: D2Client.0x6FB44A30
//  Bit 0 of the byte at +0x17 (`or byte ptr [ecx+17h], 1` / `and byte ptr [ecx+17h], 0FEh`) -- bit 8 of the word MONSTER_TestTypeFlag
//  reads. Both call sites are in MONSTER_DoClientAction, with the 0x80 bit of a parameter-block byte; the reader is
//  MONUNIQUE_MonUModSpawnAuraMissileRing. The byte is the high byte of nTypeFlag: the C writes that byte, as the bytes do, rather than
//  MONTYPEFLAG_LIGHTNINGUNIQUE on the word.
void __fastcall MONSTER_SetAuraRingEnabled(D2UnitStrc *pUnit, int32_t bEnabled)
{
    if (pUnit && pUnit->dwUnitType == UNIT_MONSTER && pUnit->pMonsterData)
    {
        if (bEnabled)
            ((uint8_t *)&pUnit->pMonsterData->nTypeFlag)[1] |= 1;
        else
            ((uint8_t *)&pUnit->pMonsterData->nTypeFlag)[1] &= ~1;
    }
}

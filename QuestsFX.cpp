#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Game.h"
#include "CoreArchive.h"
#include "QuestsFX.h"

//  QuestsFX.cpp -- the quest-effect code of D2Client.dll 1.10: the quest trigger handlers fired by server packet 0x89 (gQuestTriggerFuncs), the
//  death effects of the quest monsters (the arms of MONUNIQUE_MonUModCltQuestComplete), the quest-effect screen fade with the light overrides it
//  drives, the Worldstone shake and the game-end countdown. Address range 0x6FAB8F60..0x6FABA5DD, 35 functions.
//  The original's source file is QUEST\QuestsFX.cpp (named by the asserts in this unit).

//  The original's source path, as QUESTSFX_StartScreenFade and QUESTSFX_FireTriggerById hand it to FOG_DisplayAssert, and the assert texts
//  "nFadeOut", "eTrigger < NUM_QUEST_TRIGGERS" and "eTrigger < 32".
static const char szSourceFile[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\QUEST\\QuestsFX.cpp";

//  The unit fields the position accessors and the handlers read.
static_assert(offsetof(D2UnitStrc, dwUnitType) == 0x00
              && offsetof(D2UnitStrc, dwClassId) == 0x04
              && offsetof(D2UnitStrc, dwUnitId) == 0x0C
              && offsetof(D2UnitStrc, pStaticPath) == 0x2C
              && offsetof(D2UnitStrc, pDynamicPath) == 0x2C
              && offsetof(D2UnitStrc, dwFlagEx) == 0xC8,
              "QuestsFX: mov eax, [esi] / [ebx+4] / [esi+0Ch] / [esi+2Ch] / [esi+0C8h]");
static_assert(offsetof(D2StaticPathStrc, tGameCoords) == 0x0C
              && offsetof(D2CoordStrc, nX) == 0 && offsetof(D2CoordStrc, nY) == 4,
              "QuestsFX: the static arm reads [eax+0Ch] for X and [ecx+10h] for Y");

//1.10f: D2Client.0x6FAB8F60
//  Packet 0x89: a quest trigger fired on the server. Reads the event byte at +1 (zero-extended) and tail-jumps to QUESTSFX_FireTriggerById. The id
//  is a byte, so it is never negative.
void __fastcall QUESTSFX_FireTrigger(D2GSPacketSrv89 *pPacket)
{
    QUESTSFX_FireTriggerById(pPacket->nEvent);
}

//  Zeroes the thirteen fade globals from gbQuestFxFadeActive to gbQuestFxFadeLocked, by name and in address order.
//  DIFFERENCE: the original clears an 11-dword block (44 bytes from gbQuestFxFadeActive) with one `rep stosd` in QUESTSFX_StartScreenFade,
//  QUESTSFX_TriggerCairnStones, QUESTSFX_TickScreenFade and QUESTSFX_Reset. Ours are separate objects, so a memset from the first would write past
//  it; the thirteen end with the same values, and the one unreferenced byte of the block (0x6FBAF953) is not written. A helper of ours.
static inline void QUESTSFX_ClearFadeState()
{
    gbQuestFxFadeActive = 0;
    gnQuestFxIntensity = 0;
    gnQuestFxLightRed = 0;
    gnQuestFxLightGreen = 0;
    gnQuestFxLightBlue = 0;
    gnQuestFxFadeOutFrames = 0;
    gnQuestFxFadeInFrames = 0;
    gnQuestFxDarkHoldFrames = 0;
    gnQuestFxFadeTotalFrames = 0;
    gnQuestFxFadeFrame = 0;
    gnQuestFxFadeCosPhase = 0;
    gnQuestFxFadeLevelId = 0;
    gbQuestFxFadeLocked = 0;
}

//1.10f: D2Client.0x6FAB8F70
//  Starts the quest-effect screen fade that QUESTSFX_TickScreenFade runs: clears the fade state block, raises the active flag and stores the three
//  phase lengths, their sum and the level. A zero nFadeIn becomes 25 (after the assert); nDarkHold is taken as given. nFadeOut is Blizzard's name,
//  from the assert. Called from STATES_SetFunc2_CloakOfShadows and QUESTSFX_TriggerCairnStones. Arguments: ecx, edx and three stack slots (retn 0Ch).
//  void: the original leaves eax as nLevelId (or bForce on the early out), which no caller reads.
//  The guard `if (!gbQuestFxFadeLocked || bForce)` is vestigial: nothing writes gbQuestFxFadeLocked, so the fade always starts and bForce
//  never matters.
void __fastcall QUESTSFX_StartScreenFade(int32_t nFadeOut, int32_t nDarkHold, int32_t nFadeIn, int32_t nLevelId, int32_t bForce)
{
    int32_t nFadeInFrames;

    if (gbQuestFxFadeLocked && !bForce)
        return;

    QUESTSFX_ClearFadeState();
    gbQuestFxFadeActive = 1;
    if (!nFadeOut)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR) "nFadeOut", (uint32_t)(UINT_PTR)szSourceFile, 103);
        exit(-1);
    }
    nFadeInFrames = nFadeIn;
    gnQuestFxFadeOutFrames = nFadeOut;
    gnQuestFxDarkHoldFrames = nDarkHold;
    if (!nFadeIn)
        nFadeInFrames = 25;
    gnQuestFxFadeInFrames = nFadeInFrames;
    gnQuestFxFadeTotalFrames = nFadeOut + nDarkHold + nFadeInFrames;
    gnQuestFxFadeLevelId = nLevelId;
}

//  The missiles.txt fields QUESTSFX_TriggerCairnStones reads: the record count (+0xB6C) and table (+0xB64), the 420-byte stride, dwParam[0] (+0x38)
//  and wRange (+0x96).
static_assert(offsetof(D2DataTablesStrc, pMissilesTxt) == 0xB64
              && offsetof(D2DataTablesStrc, nMissilesTxtRecordCount) == 0xB6C
              && sizeof(D2MissilesTxt) == 420 && 288 * 420 == 0x1D880
              && offsetof(D2MissilesTxt, dwParam) == 0x38
              && offsetof(D2MissilesTxt, wRange) == 0x96,
              "QUESTSFX_TriggerCairnStones: 0x6FAB90A4..0x6FAB90C7");

//1.10f: D2Client.0x6FAB9000
//  Quest trigger 1, the cairn stones. Finds object 17 (StoneAlpha) in the player's neighbouring rooms, spawns missile 288 (cairnstones) there and
//  starts the screen fade with the level set to 4 (Act 1 - Wilderness 3). The fade lengths come from the cairnstones record: out and in are its
//  dwParam[0], the hold is its wRange less both.
//  The original inlines UNITS_GetXPosition, UNITS_GetYPosition, the missiles.txt record lookup and QUESTSFX_StartScreenFade here.
void __cdecl QUESTSFX_TriggerCairnStones()
{
    D2UnitStrc *pPlayer;
    D2ActiveRoomStrc *pRoom;
    D2UnitStrc *pStone;
    int32_t nStoneX;
    int32_t nStoneY;
    D2MissilesTxt *pMissilesTxt;
    int32_t nFadeFrames;
    int32_t nDarkHold;

    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
        return;
    pRoom = D2COMMON_UNITS_GetRoom(pPlayer);
    if (!pRoom)
        return;
    pStone = D2COMMON_UNITFINDS_FindUnitInNeighboredRooms(pRoom, UNIT_OBJECT, 17);
    if (!pStone)
        return;

    nStoneX = UNITS_GetXPosition(pStone);

    nStoneY = UNITS_GetYPosition(pStone);

    MISSILE_ClientSpawnSimple(pPlayer, 288, nStoneX, nStoneY, 0, 1);

    if (sgptDataTables->nMissilesTxtRecordCount <= 288)
        pMissilesTxt = NULL;
    else
        pMissilesTxt = &sgptDataTables->pMissilesTxt[288];

    //  BUG: the missiles.txt record is not tested for NULL (0x6FAB90BF), so a table of 288 rows or fewer would fault here. LATENT: the stock
    //  Missiles.txt has 684 rows.
    nFadeFrames = pMissilesTxt->dwParam[0];
    nDarkHold = (int16_t)pMissilesTxt->wRange - 2 * nFadeFrames;   //  movsx

    QUESTSFX_StartScreenFade(nFadeFrames, nDarkHold, nFadeFrames, 4, TRUE);
}

//1.10f: D2Client.0x6FAB9140
//  Quest trigger 0, the Den of Evil: sets gnDenOfEvilPulseFrame to 0, which starts the pulse that QUESTSFX_TickScreenFade advances and
//  QUESTSFX_GetQuestLightOverride reads.
void __cdecl QUESTSFX_TriggerDenOfEvil()
{
    gnDenOfEvilPulseFrame = 0;
}

static_assert(OVERLAY_LIGHT_JET == 181, "QUESTSFX_TriggerHoradricStaff: `mov edx, 0B5h` (0x6FAB9251)");

//1.10f: D2Client.0x6FAB9150
//  Quest trigger 3, the Horadric staff placed in the Orifice. Nothing happens without object 152 (orifice) in the neighbouring rooms. Plays the
//  sting 4690 (music_quest_horadric, held 725 frames), spawns missile 338 (horadricstaff) at the orifice, lifted 40 and given a vector of +512 and
//  a particle vector of 70 so that it rises out of the orifice, told to ignore its target, puts overlay 181 (light_jet) on the orifice, and
//  preloads Duriel's COF files (monster 211) by a tail jump.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
void __cdecl QUESTSFX_TriggerHoradricStaff()
{
    D2UnitStrc *pPlayer;
    D2ActiveRoomStrc *pRoom;
    D2UnitStrc *pOrifice;
    int32_t nOrificeX;
    int32_t nOrificeY;
    D2UnitStrc *pMissile;

    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
        return;
    pRoom = D2COMMON_UNITS_GetRoom(pPlayer);
    if (!pRoom)
        return;
    pOrifice = D2COMMON_UNITFINDS_FindUnitInNeighboredRooms(pRoom, UNIT_OBJECT, 152);
    if (!pOrifice)
        return;

    SOUNDSONG_ScheduleSting(SOUND_MUSIC_QUEST_HORADRIC, 0, 725, 1, SOUND_NONE, 0, 0);

    nOrificeX = UNITS_GetXPosition(pOrifice);

    nOrificeY = UNITS_GetYPosition(pOrifice);

    pMissile = MISSILE_ClientSpawnSimple(pPlayer, 338, nOrificeX, nOrificeY, 0, 1);
    if (pMissile)
    {
        PARTICLE_SetUnitGfxCoords(pMissile, 0, 0, 40);
        PARTICLE_SetUnitGfxVector(pMissile, 0, 0, 512, 0);
        PARTICLE_SetUnitParticleVector(pMissile, 0, 0, 70, 0);
        PARTICLE_SetUnitRisesToTarget(pMissile, 1);
    }
    GFX_AddOverlay(pOrifice, OVERLAY_LIGHT_JET, OVERLAYTYPE_ONCE, 0, 0, 0, 0, 0);
    GFX_AddMonsterCofFiles(211, 1);
}

//1.10f: D2Client.0x6FAB9280
//  Quest trigger 6, the tainted sun. Finds object 149 (taintedsunaltar) and spawns missile 350 (taintedsuncontrol) at it, then hands the missile the
//  altar's unit id through MISSILE_SetTargetY (an id in a setter named for a coordinate, as the bytes have it). The Y is read before the X.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
void __cdecl QUESTSFX_TriggerTaintedSun()
{
    D2UnitStrc *pPlayer;
    D2UnitStrc *pAltar;
    int32_t nAltarX;
    int32_t nAltarY;
    D2UnitStrc *pMissile;

    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
        return;
    //  BUG: the player's room is not tested before the search, which asserts its room and exits (0x6FAB9292); the six other triggers that fetch the
    //  player's room test it first. NOT ESTABLISHED: whether packet 0x89 can arrive while the player has no room is open.
    pAltar = D2COMMON_UNITFINDS_FindUnitInNeighboredRooms(D2COMMON_UNITS_GetRoom(pPlayer), UNIT_OBJECT, 149);
    if (!pAltar)
        return;

    nAltarY = UNITS_GetYPosition(pAltar);

    nAltarX = UNITS_GetXPosition(pAltar);

    pMissile = MISSILE_ClientSpawnSimple(pPlayer, 350, nAltarX, nAltarY, 0, 1);
    if (pMissile)
        D2COMMON_MISSILE_SetTargetY(pMissile, (int32_t)pAltar->dwUnitId);
}

//1.10f: D2Client.0x6FAB9330
//  Quest trigger 12, Diablo appears. Sets gbDiabloAppearsTintActive to 1 first, before the player is fetched (0x6FAB9331), so the flag records the
//  trigger and not the effect. The effect (missile 372 `diablo appears` at the player, and Diablo's COF files, monster 243, preloaded) happens only
//  in level 108 (Act 4 - Diablo 1), the one trigger gated on a level rather than on an object.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
void __cdecl QUESTSFX_TriggerDiabloAppears()
{
    D2UnitStrc *pPlayer;
    D2ActiveRoomStrc *pRoom;
    int32_t nPlayerX;
    int32_t nPlayerY;

    gbDiabloAppearsTintActive = 1;
    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
        return;
    pRoom = D2COMMON_UNITS_GetRoom(pPlayer);
    if (!pRoom)
        return;
    if (D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom) != 108)
        return;

    nPlayerX = UNITS_GetXPosition(pPlayer);

    nPlayerY = UNITS_GetYPosition(pPlayer);

    MISSILE_ClientSpawnSimple(pPlayer, 372, nPlayerX, nPlayerY, 0, 1);
    GFX_AddMonsterCofFiles(243, 1);
}

//1.10f: D2Client.0x6FAB93E0
//  Quest trigger 14, the Hellforge. Finds object 376 (Hellforge); plays the sting 4689 (music_quest_forge, held 350 frames) and sound 35
//  (quest_hellforge_smash) on the forge itself, so it sits in the world and not at the listener; spawns missile 373 (hfcontrol) at the forge, given
//  the forge's unit id as its target X and 0 as its target Y.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
void __cdecl QUESTSFX_TriggerHellforge()
{
    D2UnitStrc *pPlayer;
    D2ActiveRoomStrc *pRoom;
    D2UnitStrc *pForge;
    int32_t nForgeX;
    int32_t nForgeY;
    D2UnitStrc *pMissile;

    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
        return;
    pRoom = D2COMMON_UNITS_GetRoom(pPlayer);
    if (!pRoom)
        return;
    pForge = D2COMMON_UNITFINDS_FindUnitInNeighboredRooms(pRoom, UNIT_OBJECT, 376);
    if (!pForge)
        return;

    SOUNDSONG_ScheduleSting(SOUND_MUSIC_QUEST_FORGE, 0, 350, 0, SOUND_NONE, 0, 0);
    SOUNDINST_PlaySound(SOUND_QUEST_HELLFORGE_SMASH, pForge, 0, 0, 0);

    nForgeX = UNITS_GetXPosition(pForge);

    nForgeY = UNITS_GetYPosition(pForge);

    pMissile = MISSILE_ClientSpawnSimple(pPlayer, 373, nForgeX, nForgeY, 0, 1);
    if (pMissile)
    {
        D2COMMON_MISSILE_SetTargetX(pMissile, (int32_t)pForge->dwUnitId);
        D2COMMON_MISSILE_SetTargetY(pMissile, 0);
    }
}

//1.10f: D2Client.0x6FAB94D0
//  Quest trigger 13: draws nothing. Sets gnDiabloAppearsTintFrame to 0 (off the -1 that QUESTSFX_GetQuestLightOverride compares it with and that
//  QUESTSFX_TickScreenFade counts from) and then gnGameEndDeadline to the sync clock plus 90 seconds, which QUESTSFX_DrawGameEndCountdown reads.
void __cdecl QUESTSFX_ArmTriggerTimer()
{
    int32_t nSyncTime;

    nSyncTime = FOG_10055_GetSyncTime();
    gnDiabloAppearsTintFrame = 0;
    gnGameEndDeadline = nSyncTime + 90;
}

//1.10f: D2Client.0x6FAB94F0
//  The Overseer's death: the arm of MONUNIQUE_MonUModCltQuestComplete for class 479 (overseer1). Spawns missile 602 at the dying monster,
//  owned by it, with target X 25. The Y is read before the X. void: the dispatcher discards the return value.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall QUESTSFX_OnOverseerDeath(D2UnitStrc *pUnit)
{
    int32_t nX;
    int32_t nY;
    D2UnitStrc *pMissile;

    if (!pUnit)
        return;

    nY = UNITS_GetYPosition(pUnit);

    nX = UNITS_GetXPosition(pUnit);

    pMissile = MISSILE_ClientSpawnSimple(pUnit, 602, nX, nY, 0, 1);
    if (pMissile)
        D2COMMON_MISSILE_SetTargetX(pMissile, 25);
}

//1.10f: D2Client.0x6FAB9570
//  Quest trigger 16, Anya freed. The anchor is object 460 (dummy), a marker the level places; spawns missile 528 (anya center) there with
//  target X 25. The Y is read before the X. The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
void __cdecl QUESTSFX_TriggerAnya()
{
    D2UnitStrc *pPlayer;
    D2ActiveRoomStrc *pRoom;
    D2UnitStrc *pAnchor;
    int32_t nAnchorX;
    int32_t nAnchorY;
    D2UnitStrc *pMissile;

    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
        return;
    pRoom = D2COMMON_UNITS_GetRoom(pPlayer);
    if (!pRoom)
        return;
    pAnchor = D2COMMON_UNITFINDS_FindUnitInNeighboredRooms(pRoom, UNIT_OBJECT, 460);
    if (!pAnchor)
        return;

    nAnchorY = UNITS_GetYPosition(pAnchor);

    nAnchorX = UNITS_GetXPosition(pAnchor);

    pMissile = MISSILE_ClientSpawnSimple(pPlayer, 528, nAnchorX, nAnchorY, 0, 1);
    if (pMissile)
        D2COMMON_MISSILE_SetTargetX(pMissile, 25);
}

//1.10f: D2Client.0x6FAB9620
//  Quest trigger 17, Nihlathak, the one trigger anchored on a monster: unit type 1, class 526 (nihlathakboss). Spawns missiles 606 (nehlithakcontrol)
//  and 603 (nihlithak1) at him, then stops him being drawn with dwFlagEx |= 0x40000 (UNITFLAGEX_NODRAW), a read-modify-write of the whole dword.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
void __cdecl QUESTSFX_TriggerNihlathak()
{
    D2UnitStrc *pPlayer;
    D2ActiveRoomStrc *pRoom;
    D2UnitStrc *pBoss;
    int32_t nBossX;
    int32_t nBossY;

    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
        return;
    pRoom = D2COMMON_UNITS_GetRoom(pPlayer);
    if (!pRoom)
        return;
    pBoss = D2COMMON_UNITFINDS_FindUnitInNeighboredRooms(pRoom, UNIT_MONSTER, 526);
    if (!pBoss)
        return;

    nBossX = UNITS_GetXPosition(pBoss);

    nBossY = UNITS_GetYPosition(pBoss);

    MISSILE_ClientSpawnSimple(pPlayer, 606, nBossX, nBossY, 0, 1);
    MISSILE_ClientSpawnSimple(pPlayer, 603, nBossX, nBossY, 0, 1);
    pBoss->dwFlagEx |= UNITFLAGEX_NODRAW;
}

//1.10f: D2Client.0x6FAB96F0
//  Quest trigger 19: sets gbWorldstoneShakeArmed to 1 and nothing else. QUESTSFX_OnTyrael3Created and QUESTSFX_SpawnWorldstoneShake read the flag.
void __cdecl QUESTSFX_TriggerWorldstoneShake()
{
    gbWorldstoneShakeArmed = 1;
}

//  The spawn record QUESTSFX_OnAncientDeath fills: 0x17 dwords cleared, then +0 (0x420), +0x10 (541), +4 and +8 (the dying Ancient), +0x1C and +0x20
//  (the statue's position) and +0x30 (1).
static_assert(sizeof(D2MissileStrc) == 0x17 * 4
              && offsetof(D2MissileStrc, dwFlags) == 0x00
              && offsetof(D2MissileStrc, pOwner) == 0x04
              && offsetof(D2MissileStrc, pOrigin) == 0x08
              && offsetof(D2MissileStrc, nMissile) == 0x10
              && offsetof(D2MissileStrc, nTargetX) == 0x1C
              && offsetof(D2MissileStrc, nTargetY) == 0x20
              && offsetof(D2MissileStrc, nSkillLevel) == 0x30,
              "QUESTSFX_OnAncientDeath: 0x6FAB9750..0x6FAB97D6");

//1.10f: D2Client.0x6FAB9700
//  The Ancients' death: the arm of MONUNIQUE_MonUModCltQuestComplete for classes 540..542 (ancientbarb1..3), each drawn on its own statue: object
//  475 (Ancient Statue 1), 476 (Ancient Statue 2) and 474 (Ancient Statue 3); any other class returns. The statue is found in the dying Ancient's
//  neighbouring rooms; missile 541 (ancient death center) is spawned through the full record, owned by and originating at the Ancient and aimed at
//  the statue, then given the statue's unit id as its target X.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall QUESTSFX_OnAncientDeath(D2UnitStrc *pUnit)
{
    int32_t nStatueClassId;
    D2UnitStrc *pStatue;
    D2MissileStrc tParams;
    D2UnitStrc *pMissile;

    if (!pUnit)
        return;

    switch (pUnit->dwClassId)
    {
    case 540:
        nStatueClassId = 475;
        break;
    case 541:
        nStatueClassId = 476;
        break;
    case 542:
        nStatueClassId = 474;
        break;
    default:
        return;
    }

    //  BUG: the dying Ancient's room is not tested before the search (0x6FAB973A), as in QUESTSFX_TriggerTaintedSun. NOT ESTABLISHED.
    pStatue = D2COMMON_UNITFINDS_FindUnitInNeighboredRooms(D2COMMON_UNITS_GetRoom(pUnit), UNIT_OBJECT, nStatueClassId);
    if (!pStatue)
        return;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_TARGETPOS | MISSILESPAWN_FRAMESTOTARGET;
    tParams.nMissile = 541;
    tParams.pOwner = pUnit;
    tParams.pOrigin = pUnit;

    tParams.nTargetX = UNITS_GetXPosition(pStatue);

    tParams.nTargetY = UNITS_GetYPosition(pStatue);

    tParams.nSkillLevel = 1;
    pMissile = MISSILE_ClientSpawn(&tParams);
    if (pMissile)
        D2COMMON_MISSILE_SetTargetX(pMissile, (int32_t)pStatue->dwUnitId);
}


//1.10f: D2Client.0x6FAB9800
//  Fires one quest trigger: asserts the id is under 32 and under NUM_QUEST_TRIGGERS (20), sets its bit in gdwQuestTriggersFired and runs its slot of
//  gQuestTriggerFuncs if the slot is not empty. The original reaches the slot by a tail jump; here it is a call. Both compares are signed, which is
//  enough because the one caller zero-extends a byte. gdwQuestTriggersFired is written and never read.
void __fastcall QUESTSFX_FireTriggerById(int32_t eTrigger)
{
    uint32_t dwTriggerBit;
    void (__cdecl *pfnTrigger)();

    if (eTrigger >= 32)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR) "eTrigger < 32", (uint32_t)(UINT_PTR)szSourceFile, 473);
        exit(-1);
    }
    if (eTrigger >= 20)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR) "eTrigger < NUM_QUEST_TRIGGERS", (uint32_t)(UINT_PTR)szSourceFile, 474);
        exit(-1);
    }
    dwTriggerBit = (uint32_t)1 << eTrigger;
    pfnTrigger = gQuestTriggerFuncs[eTrigger];
    gdwQuestTriggersFired |= dwTriggerBit;
    if (pfnTrigger)
        pfnTrigger();
}

//  The quest monsters' effects. Each is an arm of MONUNIQUE_MonUModCltQuestComplete (the client function of monumod 22, questcomplete), which tests
//  pUnit for NULL, switches on dwClassId and, in every arm, tests dwAnimMode (mode 0 is death, 14 is sequence) before calling the arm:
//
//      class           mode  arm
//       45 corruptrogue3  0  QUESTSFX_SpawnTowerChestEffect
//      156 andariel       0  QUESTSFX_OnAndarielDeath
//      211 duriel         0  QUESTSFX_OnDurielDeath
//      229 radament       0  QUESTSFX_OnRadamentDeath
//      242 mephisto       0  QUESTSFX_OnMephistoDeath
//      243 diablo         0  QUESTSFX_SetWatchedUnit
//      256 izual          0  QUESTSFX_OnIzualDeath
//      267 bloodraven     0  QUESTSFX_OnBloodRavenDeath
//      284 maggotqueen1  14  QUESTSFX_OnMaggotQueenDeath
//      366 compellingorb  0  QUESTSFX_OnCompellingOrbDeath
//      479 overseer1      0  QUESTSFX_OnOverseerDeath
//      540..542 ancientbarb1..3  0  QUESTSFX_OnAncientDeath
//      544 baalcrab       0  QUESTSFX_OnBaalDeath
//
//  Every arm is void: the dispatcher discards eax. pUnit is never NULL in any of them, because the dispatcher returns first.

//1.10f: D2Client.0x6FAB9870
//  Blood Raven (class 267, mode 0). Spawns three missiles through the full record, owned by and starting at her: 284 (brdeathcontrol, target X 50),
//  296 (brdeathspirits1) and 293 (brdeathsmokes1). The third reuses the second's record with only nMissile changed (no clear between the calls).
//  Not a defect: MISSILE_ClientSpawn only reads its record, so the third spawn gets a clean record.
void __fastcall QUESTSFX_OnBloodRavenDeath(D2UnitStrc *pUnit)
{
    D2MissileStrc tParams;
    D2UnitStrc *pMissile;

    memset(&tParams, 0, sizeof(tParams));
    tParams.nMissile = 284;
    tParams.pOwner = pUnit;
    tParams.pOrigin = pUnit;
    tParams.nSkillLevel = 1;
    pMissile = MISSILE_ClientSpawn(&tParams);
    if (pMissile)
        D2COMMON_MISSILE_SetTargetX(pMissile, 50);

    memset(&tParams, 0, sizeof(tParams));
    tParams.nMissile = 296;
    tParams.pOwner = pUnit;
    tParams.pOrigin = pUnit;
    tParams.nSkillLevel = 1;
    MISSILE_ClientSpawn(&tParams);

    tParams.nMissile = 293;
    MISSILE_ClientSpawn(&tParams);
}

//1.10f: D2Client.0x6FAB9900
//  The Forgotten Tower's chest (class 45, corruptrogue3, mode 0; the Countess's base class). Looks for object 371 (chest) near the dying monster and
//  aims there; with none in range it aims at the monster's own position less 15 on both axes, so the effect always appears. Spawns missile 291
//  (towermist) through the full record (flags 0x420, owned by and starting at the monster) and 332 (towerchestspawner) through
//  MISSILE_ClientSpawnSimple, owned by the monster, whose target Y is set to 0.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall QUESTSFX_SpawnTowerChestEffect(D2UnitStrc *pUnit)
{
    D2UnitStrc *pChest;
    int32_t nSpawnX;
    int32_t nSpawnY;
    int32_t nX;
    int32_t nY;
    D2MissileStrc tParams;
    D2UnitStrc *pMissile;

    //  BUG: the dying monster's room is not tested before the search (0x6FAB9911), as in QUESTSFX_TriggerTaintedSun. NOT ESTABLISHED.
    pChest = D2COMMON_UNITFINDS_FindUnitInNeighboredRooms(D2COMMON_UNITS_GetRoom(pUnit), UNIT_OBJECT, 371);
    if (pChest)
    {
        nSpawnX = UNITS_GetXPosition(pChest);

        nSpawnY = UNITS_GetYPosition(pChest);
    }
    else
    {
        nX = UNITS_GetXPosition(pUnit);
        nSpawnX = nX - 15;

        nY = UNITS_GetYPosition(pUnit);
        nSpawnY = nY - 15;
    }

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_TARGETPOS | MISSILESPAWN_FRAMESTOTARGET;
    tParams.nMissile = 291;
    tParams.pOwner = pUnit;
    tParams.pOrigin = pUnit;
    tParams.nTargetX = nSpawnX;
    tParams.nTargetY = nSpawnY;
    tParams.nSkillLevel = 1;
    MISSILE_ClientSpawn(&tParams);

    pMissile = MISSILE_ClientSpawnSimple(pUnit, 332, nSpawnX, nSpawnY, 0, 1);
    if (pMissile)
        D2COMMON_MISSILE_SetTargetY(pMissile, 0);
}

//1.10f: D2Client.0x6FAB9A40
//  The Compelling Orb (class 366, compellingorb, mode 0). Looks for object 386 near the orb; spawns missile 368 (orbmist) with flags 0x420 and the
//  object's unit id as its target X, and plays the sting 4687 (music_quest_compelling, held 350 frames). With no object in range it aims at the orb's
//  own position less 15 on both axes, as QUESTSFX_SpawnTowerChestEffect does, and skips the sting and the target id.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall QUESTSFX_OnCompellingOrbDeath(D2UnitStrc *pUnit)
{
    D2UnitStrc *pStairs;
    int32_t nSpawnX;
    int32_t nSpawnY;
    int32_t nX;
    int32_t nY;
    D2MissileStrc tParams;
    D2UnitStrc *pMissile;

    //  BUG: the orb's room is not tested before the search (0x6FAB9A50), as in QUESTSFX_TriggerTaintedSun. NOT ESTABLISHED.
    pStairs = D2COMMON_UNITFINDS_FindUnitInNeighboredRooms(D2COMMON_UNITS_GetRoom(pUnit), UNIT_OBJECT, 386);
    if (pStairs)
    {
        SOUNDSONG_ScheduleSting(SOUND_MUSIC_QUEST_COMPELLING, 0, 350, 0, SOUND_NONE, 0, 0);

        nSpawnX = UNITS_GetXPosition(pStairs);

        nSpawnY = UNITS_GetYPosition(pStairs);
    }
    else
    {
        nX = UNITS_GetXPosition(pUnit);
        nSpawnX = nX - 15;

        nY = UNITS_GetYPosition(pUnit);
        nSpawnY = nY - 15;
    }

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_TARGETPOS | MISSILESPAWN_FRAMESTOTARGET;
    tParams.nMissile = 368;
    tParams.pOwner = pUnit;
    tParams.pOrigin = pUnit;
    tParams.nTargetX = nSpawnX;
    tParams.nTargetY = nSpawnY;
    tParams.nSkillLevel = 1;
    pMissile = MISSILE_ClientSpawn(&tParams);
    if (pMissile && pStairs)
        D2COMMON_MISSILE_SetTargetX(pMissile, (int32_t)pStairs->dwUnitId);
}

//1.10f: D2Client.0x6FAB9B90
//  Mephisto (class 242, mode 0). Spawns three missiles through the full record, owned by and starting at him, each record cleared first: 299
//  (mephistodeathcontrol, target X and Y 75), 358 (mephistoholedelay) and 306 (mephistodonotdraw).
void __fastcall QUESTSFX_OnMephistoDeath(D2UnitStrc *pUnit)
{
    D2MissileStrc tParams;
    D2UnitStrc *pMissile;

    memset(&tParams, 0, sizeof(tParams));
    tParams.nMissile = 299;
    tParams.pOwner = pUnit;
    tParams.pOrigin = pUnit;
    tParams.nSkillLevel = 1;
    pMissile = MISSILE_ClientSpawn(&tParams);
    if (pMissile)
    {
        D2COMMON_MISSILE_SetTargetX(pMissile, 75);
        D2COMMON_MISSILE_SetTargetY(pMissile, 75);
    }

    memset(&tParams, 0, sizeof(tParams));
    tParams.nMissile = 358;
    tParams.pOwner = pUnit;
    tParams.pOrigin = pUnit;
    tParams.nSkillLevel = 1;
    MISSILE_ClientSpawn(&tParams);

    memset(&tParams, 0, sizeof(tParams));
    tParams.nMissile = 306;
    tParams.pOwner = pUnit;
    tParams.pOrigin = pUnit;
    tParams.nSkillLevel = 1;
    MISSILE_ClientSpawn(&tParams);
}

//1.10f: D2Client.0x6FAB9C40
//  Andariel (class 156, mode 0). Spawns missile 307 (andycontrol0), owned by and starting at her.
void __fastcall QUESTSFX_OnAndarielDeath(D2UnitStrc *pUnit)
{
    D2MissileStrc tParams;

    memset(&tParams, 0, sizeof(tParams));
    tParams.nMissile = 307;
    tParams.pOwner = pUnit;
    tParams.pOrigin = pUnit;
    tParams.nSkillLevel = 1;
    MISSILE_ClientSpawn(&tParams);
}

//1.10f: D2Client.0x6FAB9C80
//  Radament (class 229, mode 0). Spawns missile 347 (radamentdeath) at his position, owned by the player: CUNIT_GetPlayer's result goes to
//  MISSILE_ClientSpawnSimple untested. The Y is read before the X. The same shape as QUESTSFX_OnMaggotQueenDeath and QUESTSFX_OnDurielDeath.
//  Not a defect, for all three: MISSILE_ClientSpawnSimple tests its owner itself and returns 0 for none.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall QUESTSFX_OnRadamentDeath(D2UnitStrc *pUnit)
{
    D2UnitStrc *pPlayer;
    int32_t nX;
    int32_t nY;

    pPlayer = CUNIT_GetPlayer();

    nY = UNITS_GetYPosition(pUnit);

    nX = UNITS_GetXPosition(pUnit);

    MISSILE_ClientSpawnSimple(pPlayer, 347, nX, nY, 0, 1);
}

//1.10f: D2Client.0x6FAB9D00
//  The Maggot Queen (class 284, maggotqueen1), the one arm on mode 14 (sequence) and not on her death. Spawns missile 353 (queendeathcenter) at her
//  position, owned by the player, untested -- not a defect, as in QUESTSFX_OnRadamentDeath.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall QUESTSFX_OnMaggotQueenDeath(D2UnitStrc *pUnit)
{
    D2UnitStrc *pPlayer;
    int32_t nX;
    int32_t nY;

    pPlayer = CUNIT_GetPlayer();

    nY = UNITS_GetYPosition(pUnit);

    nX = UNITS_GetXPosition(pUnit);

    MISSILE_ClientSpawnSimple(pPlayer, 353, nX, nY, 0, 1);
}

//1.10f: D2Client.0x6FAB9D80
//  Duriel (class 211, mode 0). Spawns missile 363 (durieldeathcontrol) at his position, owned by the player, untested -- not a defect, as in
//  QUESTSFX_OnRadamentDeath. The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall QUESTSFX_OnDurielDeath(D2UnitStrc *pUnit)
{
    D2UnitStrc *pPlayer;
    int32_t nX;
    int32_t nY;

    pPlayer = CUNIT_GetPlayer();

    nY = UNITS_GetYPosition(pUnit);

    nX = UNITS_GetXPosition(pUnit);

    MISSILE_ClientSpawnSimple(pPlayer, 363, nX, nY, 0, 1);
}

//1.10f: D2Client.0x6FAB9E00
//  Izual (class 256, mode 0). Plays the sting 4691 (music_quest_izual, held 250 frames), then spawns missile 379 (izual control) through the full
//  record, owned by and starting at him.
void __fastcall QUESTSFX_OnIzualDeath(D2UnitStrc *pUnit)
{
    D2MissileStrc tParams;

    SOUNDSONG_ScheduleSting(SOUND_MUSIC_QUEST_IZUAL, 0, 250, 0, SOUND_NONE, 0, 0);
    memset(&tParams, 0, sizeof(tParams));
    tParams.nMissile = 379;
    tParams.pOwner = pUnit;
    tParams.pOrigin = pUnit;
    tParams.nSkillLevel = 1;
    MISSILE_ClientSpawn(&tParams);
}

//1.10f: D2Client.0x6FAB9E60
//  Diablo (class 243, mode 0): records his unit id in gdwQuestFxWatchedUnitId, or -1 for no unit. The dispatcher has already returned on a NULL
//  unit, so from its one caller the -1 arm is dead. QUESTSFX_DrawGameEndCountdown reads the id.
void __fastcall QUESTSFX_SetWatchedUnit(D2UnitStrc *pUnit)
{
    if (!pUnit)
    {
        gdwQuestFxWatchedUnitId = (uint32_t)-1;
        return;
    }
    gdwQuestFxWatchedUnitId = pUnit->dwUnitId;
}

//1.10f: D2Client.0x6FAB9E80
//  Baal (class 544, baalcrab, mode 0), the one arm that takes no unit: the dispatcher calls it with the dying monster in ecx and nothing reads it.
//  Searches the player's neighbouring rooms for a monster of class 544 and spawns missile 625 (baalfx control) there, owned by the player. The Y is
//  read before the X. UNITS_GetRoom takes one argument (the player; D2Common #10342) and the search three (the room, 1 and 544; #11087).
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
void __cdecl QUESTSFX_OnBaalDeath()
{
    D2UnitStrc *pPlayer;
    D2UnitStrc *pBaalCrab;
    int32_t nX;
    int32_t nY;

    pPlayer = CUNIT_GetPlayer();
    //  BUG: the player is not tested; a NULL player would end the process in UNITS_GetRoom's own ptUnit assert (0x6FAB9E82..0x6FAB9E97). LATENT.
    //  BUG: the player's room is not tested before the search. NOT ESTABLISHED.
    pBaalCrab = D2COMMON_UNITFINDS_FindUnitInNeighboredRooms(D2COMMON_UNITS_GetRoom(pPlayer), UNIT_MONSTER, 544);
    if (!pBaalCrab)
        return;

    nY = UNITS_GetYPosition(pBaalCrab);

    nX = UNITS_GetXPosition(pBaalCrab);

    MISSILE_ClientSpawnSimple(pPlayer, 625, nX, nY, 0, 1);
}

//1.10f: D2Client.0x6FAB9F10
//  Tyrael descends after Baal: the monster-creation hook, called from CUNIT_CreateMonsterUnit's arm for class 521 (tyrael3). Nothing happens unless
//  missile 625 (baalfx control), the one QUESTSFX_OnBaalDeath spawns on the baalcrab, is in a neighbouring room. With it, Tyrael gets particles and
//  falls in from 300 up with a vector of -2, and the marker's target Y is set to 1. Then, from the player's room, in levels 131..132 the armed
//  Worldstone shake fires once; anywhere else the latch clears.
//  The original inlines QUESTSFX_SpawnWorldstoneShake here.
void __fastcall QUESTSFX_OnTyrael3Created(D2UnitStrc *pMonster)
{
    D2UnitStrc *pMarker;

    //  BUG: the monster's room is not tested before the search (0x6FAB9F1C), as in QUESTSFX_TriggerTaintedSun. NOT ESTABLISHED: a tyrael3 created at
    //  coordinates outside every loaded room would carry a NULL room into this search; whether a stock server ever announces one there is open.
    pMarker = D2COMMON_UNITFINDS_FindUnitInNeighboredRooms(D2COMMON_UNITS_GetRoom(pMonster), UNIT_MISSILE, 625);
    if (!pMarker)
        return;

    PARTICLE_AllocUnitParticles(pMonster);
    PARTICLE_SetUnitGfxCoords(pMonster, 0, 0, 300);
    PARTICLE_SetUnitGfxVector(pMonster, 0, 0, -2, 1);
    PARTICLE_SetUnitParticleVector(pMonster, 0, 0, 5, 0);
    PARTICLE_SetUnitRisesToTarget(pMonster, 0);
    D2COMMON_MISSILE_SetTargetY(pMarker, 1);

    QUESTSFX_SpawnWorldstoneShake(CUNIT_GetPlayerRoom());
}

//1.10f: D2Client.0x6FAB9FE0
//  The quest override on a room's ambient light. Returns the intensity and the colour as three bytes; a zero colour (the callers clear it first)
//  means no override. By the room's level:
//
//      8, Act 1 - Cave 1 (the Den of Evil): nothing once gbDenOfEvilLightDone is set or while QUESTS_GetQuestState(0, 1) is 0; otherwise the red
//        (255, 64, 48) at 80, or, once the pulse counter runs, at cos((frame << 7) / 30) * 80 (a signed divide by 30)
//      107..108, Act 4 - Lava 1 and Act 4 - Diablo 1: the same red at 160 while gbDiabloAppearsTintActive is set and its frame is still -1;
//        otherwise nothing
//      any other: gnQuestFxLightRed/Green/Blue and gnQuestFxIntensity, as QUESTSFX_TickScreenFade left them
//
//  The cosine is FOG #10083's, which answers on the x87 stack; the original multiplies it by the 4-byte float gfConst80 (80.0) and truncates.
//  Arguments: ecx, edx and two stack slots (retn 8).
int32_t __fastcall QUESTSFX_GetQuestLightOverride(D2ActiveRoomStrc *pRoom, uint8_t *pnRed, uint8_t *pnGreen, uint8_t *pnBlue)
{
    int32_t nLevelId;

    nLevelId = D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom);
    if (nLevelId == 8)
    {
        if (gbDenOfEvilLightDone || !QUESTS_GetQuestState(0, QUEST_A1Q1_DENOFEVIL))
            return 0;
        *pnRed = 0xFF;
        *pnGreen = 64;
        *pnBlue = 48;
        if (gnDenOfEvilPulseFrame == -1)
            return 80;
        return (int32_t)(FOG_10083_Cos_LUT((uint32_t)((int32_t)((uint32_t)gnDenOfEvilPulseFrame << 7) / 30)) * gfConst80);
    }
    if (nLevelId <= 106 || nLevelId > 108)
    {
        *pnRed = gnQuestFxLightRed;
        *pnGreen = gnQuestFxLightGreen;
        *pnBlue = gnQuestFxLightBlue;
        return gnQuestFxIntensity;
    }
    if (gbDiabloAppearsTintActive && gnDiabloAppearsTintFrame == -1)
    {
        *pnRed = 0xFF;
        *pnGreen = 64;
        *pnBlue = 48;
        return 160;
    }
    return 0;
}

//1.10f: D2Client.0x6FABA0D0
//  A room became active: called once per room from GAME_UpdateFrame. In the Den of Evil (level 8), after its light has latched, the new room gets
//  its ambient lights too, by a tail jump to QUESTSFX_SpawnAmbientMissiles. Asserts the room ("hRoom").
//  A `this`-only function, written __fastcall with one argument (the same ABI as __thiscall).
void __fastcall QUESTSFX_OnRoomActivated(D2ActiveRoomStrc *pRoom)
{
    if (!pRoom)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR) "hRoom", (uint32_t)(UINT_PTR)szSourceFile, 836);
        exit(-1);
    }
    if (D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom) != 8)
        return;
    if (!gbDenOfEvilLightDone)
        return;
    QUESTSFX_SpawnAmbientMissiles(pRoom);
}

//  The fields QUESTSFX_SpawnAmbientMissiles reads and writes: the player's seed at +0x20 and +0x24, the record's nX, nY and nSkill, the room's
//  coordinates.
static_assert(offsetof(D2UnitStrc, pSeed) == 0x20
              && offsetof(D2MissileStrc, nX) == 0x14 && offsetof(D2MissileStrc, nY) == 0x18
              && offsetof(D2MissileStrc, nSkill) == 0x2C
              && offsetof(D2DrlgCoordsStrc, nSubtileWidth) == 8
              && offsetof(D2DrlgCoordsStrc, nSubtileHeight) == 0xC,
              "QUESTSFX_SpawnAmbientMissiles: [esi+20h]/[esi+24h], var_48/var_44/var_30, pCoords");

//1.10f: D2Client.0x6FABA120
//  The Den of Evil's ambient lights for one room: up to three missiles 287 (denofevillight) at random subtiles inside the room's own coordinates
//  (DUNGEON_GetRoomCoordinates), drawn with the player's seed. A position is refused if COLLISION_CheckMask (mask 5) blocks it, and a refusal
//  does not count towards the three spawns; at most 25 attempts. The record: flags 0x2003, owner the player, nSkill 0, level 1.
//  A `this`-only function, written __fastcall as QUESTSFX_OnRoomActivated is.
//  The original inlines SEED_RollLimitedRandomNumber for the two draws here.
void __fastcall QUESTSFX_SpawnAmbientMissiles(D2ActiveRoomStrc *pRoom)
{
    D2UnitStrc *pPlayer;
    D2MissileStrc tParams;
    D2DrlgCoordsStrc tCoords;
    int32_t nMissilesSpawned;
    int32_t nAttempts;
    int32_t nRandX;
    int32_t nRandY;
    int32_t nX;
    int32_t nY;

    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
        return;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETOFFSET | MISSILESPAWN_RANDOMDIR;
    tParams.pOwner = pPlayer;
    tParams.nSkill = 0;
    tParams.nSkillLevel = 1;
    tParams.nMissile = 287;
    D2COMMON_DUNGEON_GetRoomCoordinates(pRoom, &tCoords);

    nMissilesSpawned = 0;
    nAttempts = 0;
    do
    {
        if (nAttempts >= 25)
            break;

        nRandX = SEED_RollLimitedRandomNumber(&pPlayer->pSeed, tCoords.nSubtileWidth);
        nX = nRandX + tCoords.nSubtileX;

        nRandY = SEED_RollLimitedRandomNumber(&pPlayer->pSeed, tCoords.nSubtileHeight);
        nY = nRandY + tCoords.nSubtileY;

        tParams.nX = nX;
        tParams.nY = nY;
        if (D2COMMON_COLLISION_CheckMask(pRoom, nX, nY, 5))
            --nMissilesSpawned;
        else
            MISSILE_ClientSpawn(&tParams);
        ++nMissilesSpawned;
        ++nAttempts;
    } while (nMissilesSpawned < 3);
}

//  The room chain QUESTSFX_TickScreenFade walks (pRoomNext at +0x7C).
static_assert(offsetof(D2ActiveRoomStrc, pRoomNext) == 0x7C,
              "QUESTSFX_TickScreenFade: mov esi, [esi+7Ch] at 0x6FABA3FA");

//1.10f: D2Client.0x6FABA270
//  The quest-effect screen fade, one frame, called from GAME_UpdateFrame. While gbQuestFxFadeActive is set: the player's room colour goes into
//  gnQuestFxLightRed/Green/Blue and its intensity into a local (DLIGHTMAP_GetRoomLightColor); then, over gnQuestFxFadeFrame:
//
//      frame < out              intensity = cos(phase) * room intensity, then
//                               phase = (frame << 7) / out
//      frame < total - in       intensity = 0 -- the hold
//      otherwise                intensity = cos(phase) * room intensity, then
//                               phase = 128 - ((in - total + frame) << 7) / in,
//                               and a zero `in` becomes 25 first
//
//  Each cosine is taken at the previous frame's phase, then the phase moves; every compare is signed and every divide is `cdq / idiv`. The frame
//  advances; the block is cleared once the frame passes the total or the player's level is no longer the one the fade started on.
//  Then the two tint timers, whether or not a fade runs: the Den of Evil pulse counts up to 30, then latches gbDenOfEvilLightDone and spawns the
//  ambient lights in every level-8 room of the act; the Diablo tint counts up to 30, then clears itself back to -1.
//  The intensity is FOG #10083's answer times the int intensity, truncated.
//  The shifts of a frame count are written on an unsigned copy: the same bits, without C's undefined left shift of a negative int.
void __cdecl QUESTSFX_TickScreenFade()
{
    int32_t nIntensity;
    int32_t nFrame;
    int32_t nFadeIn;
    int32_t nElapsed;
    int32_t nLevelId;
    int32_t nPulse;
    int32_t nTint;
    D2ActiveRoomStrc *pRoom;

    if (gbQuestFxFadeActive)
    {
        nIntensity = 0;
        DLIGHTMAP_GetRoomLightColor(CUNIT_GetPlayerRoom(), &nIntensity, &gnQuestFxLightRed, &gnQuestFxLightGreen, &gnQuestFxLightBlue, 0);
        nFrame = gnQuestFxFadeFrame;
        if (nFrame < gnQuestFxFadeOutFrames)
        {
            gnQuestFxIntensity = (int32_t)(FOG_10083_Cos_LUT(gnQuestFxFadeCosPhase) * (double)nIntensity);
            nFrame = gnQuestFxFadeFrame;
            gnQuestFxFadeCosPhase = (int32_t)((uint32_t)nFrame << 7) / gnQuestFxFadeOutFrames;
        }
        else if (nFrame < gnQuestFxFadeTotalFrames - gnQuestFxFadeInFrames)
        {
            gnQuestFxIntensity = 0;
        }
        else
        {
            gnQuestFxIntensity = (int32_t)(FOG_10083_Cos_LUT(gnQuestFxFadeCosPhase) * (double)nIntensity);
            nFadeIn = gnQuestFxFadeInFrames;
            nFrame = gnQuestFxFadeFrame;
            nElapsed = nFadeIn - gnQuestFxFadeTotalFrames + nFrame;
            if (!nFadeIn)
            {
                nFadeIn = 25;
                gnQuestFxFadeInFrames = 25;
            }
            gnQuestFxFadeCosPhase = 128 - (int32_t)((uint32_t)nElapsed << 7) / nFadeIn;
        }
        gnQuestFxFadeFrame = nFrame + 1;

        pRoom = CUNIT_GetPlayerRoom();
        nLevelId = 0;
        if (pRoom)
            nLevelId = D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom);
        if (gnQuestFxFadeFrame > gnQuestFxFadeTotalFrames || nLevelId != gnQuestFxFadeLevelId)
        {
            QUESTSFX_ClearFadeState();
        }
    }

    if (!gbDenOfEvilLightDone && gnDenOfEvilPulseFrame != -1)
    {
        nPulse = gnDenOfEvilPulseFrame + 1;
        gnDenOfEvilPulseFrame = nPulse;
        if (nPulse >= 30)
        {
            gbDenOfEvilLightDone = 1;
            for (pRoom = D2COMMON_DUNGEON_GetRoomFromAct(gpAct); pRoom; pRoom = pRoom->pRoomNext)
            {
                if (D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom) == 8)
                    QUESTSFX_SpawnAmbientMissiles(pRoom);
            }
        }
    }

    if (gbDiabloAppearsTintActive && gnDiabloAppearsTintFrame != -1)
    {
        nTint = gnDiabloAppearsTintFrame + 1;
        gnDiabloAppearsTintFrame = nTint;
        if (nTint >= 30)
        {
            gbDiabloAppearsTintActive = 0;
            gnDiabloAppearsTintFrame = -1;
        }
    }
}

static_assert(MISSILE_WORLDSTONE_SHAKE == 639 && SKILL_ATTACK == 0,
              "QUESTSFX_SpawnWorldstoneShake passes missile 639, skill 0 (in the binary its inlined copy too)");

//1.10f: D2Client.0x6FABA440
//  The Worldstone shake, from the player's room each frame (called from PLAYER_AdvanceAnimMode). In levels 131 (Act 5 - Throne Room) and 132
//  (Act 5 - World Stone), once QUESTSFX_TriggerWorldstoneShake has armed it and it has not yet fired: missile 639 (worldstone shake) aimed at the
//  player, and the latch set. Anywhere else the latch clears.
void __fastcall QUESTSFX_SpawnWorldstoneShake(D2ActiveRoomStrc *pRoom)
{
    int32_t nLevelId;
    D2UnitStrc *pPlayer;

    if (!pRoom)
        return;
    nLevelId = D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom);
    if (nLevelId < 131 || nLevelId > 132)
    {
        gbWorldstoneShakeSpawned = 0;
        return;
    }
    if (gbWorldstoneShakeSpawned || !gbWorldstoneShakeArmed)
        return;
    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
        return;
    SKILLS_MissileClientSpawnAimedAtTargetUnit(pPlayer, MISSILE_WORLDSTONE_SHAKE, SKILL_ATTACK, 1);
    gbWorldstoneShakeSpawned = 1;
}

static_assert(D2FONT_FONTINGAMECHAT == 13,
              "QUESTSFX_DrawGameEndCountdown: mov ecx, 0Dh at 0x6FABA52C");

//1.10f: D2Client.0x6FABA4A0
//  The game-end countdown, drawn from UIPANELS_DrawInGamePanels: tbl string 5097 (LordofTerrordied) formatted with the deadline less the sync clock,
//  centred on the screen's width at y 375 in font 13 (D2FONT_FONTINGAMECHAT), colour 9. Nothing while no deadline is set or once it has passed
//  (signed), and nothing while the watched unit (Diablo, from QUESTSFX_SetWatchedUnit) is still found as a monster, is not in mode 12 (dead) and more
//  than 70 remain.
//  The 256-unit Unicode buffer is constructed where the original constructs it, after the three tests: `Unicode wszText[256]` is declared in the
//  block that follows them.
void __cdecl QUESTSFX_DrawGameEndCountdown()
{
    int32_t nSyncTime;
    D2UnitStrc *pWatched;
    enum Font nPrevFont;
    int32_t nTextWidth;

    nSyncTime = FOG_10055_GetSyncTime();
    if (!gnGameEndDeadline)
        return;
    if (gnGameEndDeadline <= nSyncTime)
        return;
    pWatched = CUNIT_FindUnitByGuidAndType(gdwQuestFxWatchedUnitId, UNIT_MONSTER);
    if (pWatched && pWatched->dwAnimMode != 12 && gnGameEndDeadline - 70 > nSyncTime)
    {
        return;
    }

    {
        Unicode wszText[256];

        Unicode::sprintf(256, wszText, D2LANG_GetStringFromTblIndex(STR_IDX_5097_LordofTerrordied), gnGameEndDeadline - nSyncTime);
        nPrevFont = D2WIN_10127_SetFont(D2FONT_FONTINGAMECHAT);
        nTextWidth = D2WIN_10121_GetTextWidth(wszText);
        D2WIN_10117_DrawText(wszText, gnScreenWidth / 2 - nTextWidth / 2, 375, 9, 1);
        D2WIN_10127_SetFont(nPrevFont);
    }
}

//1.10f: D2Client.0x6FABA580
//  Clears the quest-effect state at the start of a game: gnDenOfEvilPulseFrame and gdwQuestFxWatchedUnitId to -1 (0 is a legal frame number and a
//  legal unit id), the eleven-dword fade block and four more flags to zero. The order is the original's: gbDenOfEvilLightDone is stored between
//  the fill's setup and the fill itself. The fill stops four bytes short of gbDenOfEvilLightDone; those four are gdwQuestTriggersFired, which
//  nothing clears and nothing reads. gbWorldstoneShakeSpawned is not cleared here either: its own readers clear it on leaving the Worldstone
//  levels. The static_asserts pin the original's fill (eleven dwords, 0x2C bytes, from gbQuestFxFadeActive); QUESTSFX_ClearFadeState zeroes the
//  globals by name.
static_assert(11 * sizeof(uint32_t) == 0x2C,
              "the quest-fx fade block: the rep stosd at 0x6FABA5A1 fills 0x2C bytes");
static_assert(0x10F948 + 0x2C == 0x10F974 && 0x10F974 < 0x10F978,
              "the fill stops short of gbDenOfEvilLightDone, leaving four bytes");

void __cdecl QUESTSFX_Reset()
{
    gnDenOfEvilPulseFrame = -1;
    gdwQuestFxWatchedUnitId = (uint32_t)-1;
    gbDenOfEvilLightDone = 0;
    QUESTSFX_ClearFadeState();
    gbDiabloAppearsTintActive = 0;
    gnGameEndDeadline = 0;
    gbWorldstoneShakeArmed = 0;
}

//1.10f: D2Client.0x6FABA5C0
//  Arms the game-end deadline from a packet: the sync clock plus the delay, when the delay is positive (signed) and no deadline is set. The delay is
//  in seconds, the unit of the clock (FOG_10055_GetSyncTime) and of the string QUESTSFX_DrawGameEndCountdown prints. Its one caller is
//  QUESTLOG_QuestOnQuestStatePacket's tail jump. QUESTSFX_ArmTriggerTimer writes the same global unconditionally.
void __fastcall QUESTSFX_ArmGameEndDeadline(int32_t nDelaySeconds)
{
    if (nDelaySeconds <= 0)
        return;
    if (gnGameEndDeadline)
        return;
    gnGameEndDeadline = FOG_10055_GetSyncTime() + nDelaySeconds;
}

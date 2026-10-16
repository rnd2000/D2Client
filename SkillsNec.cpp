#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"

//  SkillsNec.cpp -- the client skill code of the necromancer in D2Client.dll 1.10: the curses, Teeth and Bone Spear's cast, raising skeletons and
//  mages, the two corpse explosions, the bone wall and prison, the iron golem and Revive, with the classless monster rows that borrow the same
//  slots. The original source file is Skills\SkillsNec.cpp (the assert in SKILLS_CltStFunc23 names it). Address range 0x6FAC7F90..0x6FAC8ED6.
//  Every start and do function is reached by one slot of the client skill tables and nothing else: a `cltstfunc` slot N is SKILLS_CltStFuncN and a
//  `cltdofunc` slot N is SKILLS_CltDoFuncN. A skill id is a D2C_Skills and a missile id a D2C_MissilesHcIdx.


//1.10f: D2Client.0x6FAC7F90
//  The curse's cast: `cltstfunc` 18, the start of the thirteen curse rows (Amplify Damage, Dim Vision, Weaken, Iron Maiden, Terror, Confuse, Life
//  Tap, Attract, Decrepify, Lower Resist, and the classless MonCurseCast, Defense Curse and Blood Mana). The record, or the answer is 0. Then
//  `cltmissilec` (`cursecast` for every stock row), as a SIGNED word inside [0, missiles.txt's count) (test / jl, cmp / jge), is made by
//  SKILLS_MissileClientSpawnAimedAtTargetUnit with the skill and level, its answer not read. The answer is 1 with or without the missile.
//  The original inlines DATATBLS_GetSkillsTxtRecordById here.
int32_t __fastcall SKILLS_CltStFunc18(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileId;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = (int16_t)pSkillsTxt->wCltMissileC;
    if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
        SKILLS_MissileClientSpawnAimedAtTargetUnit(pUnit, (D2C_MissilesHcIdx)nMissileId, nSkillId, nSkillLevel);

    return 1;
}


//1.10f: D2Client.0x6FAC7FF0
//  The curse itself: `cltdofunc` 30, the do of the same thirteen curse rows. The client draws all thirteen with this one, where the server has dos
//  of its own for Confuse, Attract and MonCurseCast.
//    * The record, and `cltmissilea` (the curse's own missile: `curseamplifydamage` and its siblings, `curseeffectred` for the three classless
//      rows) as a SIGNED word inside [0, missiles.txt's count), or the answer is 0. The word is kept as a dword in a frame slot.
//    * UNITFLAG_SKSRVDOFUNC on a non-NULL unit.
//    * SKILLS_GetPathTargetPos's point; its answer is tested and never 0.
//    * `aurarangecalc` evaluated, less 3 (sub ebx, 3), at least 2 SIGNED (cmp ebx, 2 / jge): the radius.
//    * The block, cleared (rep stosd, 17h dwords), with flags 1, the owner, the point as the ORIGIN, the skill, the level and missile 191,
//      MISSILE_CURSECENTER. No missile, 0.
//    * The centre's own light is freed (CUNIT_GfxFreeUnitLightMap) and DLIGHTMAP_AddLight gives it a red one -- type 1, radius 1, intensity 255,
//      red 255, green and blue 0 -- whose TARGET radius is set to the radius (DLIGHTMAP_SetTargetRadius), so it grows there; stored as the
//      missile's pLight only when made.
//    * D2Common's MISSILE_SetStreamMissile with `cltmissilea` and MISSILE_SetStreamRange with the radius, and 1.
//  Both setters read a WORD off the stack, so the full dwords pushed here reach them as the uint16_t and __int16 their prototypes take; the C casts.
//  The original inlines DATATBLS_GetSkillsTxtRecordById here.
int32_t __fastcall SKILLS_CltDoFunc30(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nStreamMissile;
    int32_t nTargetX;
    int32_t nTargetY;
    int32_t nRadius;
    D2UnitStrc *pMissile;
    D2ClientLightStrc *pLight;
    D2MissileStrc tParams;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nStreamMissile = (int16_t)pSkillsTxt->wCltMissileA;
    if (nStreamMissile < 0 || nStreamMissile >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    //  BUG: pUnit is tested here only for the flag, then used untested (SKILLS_GetPathTargetPos, SKILLS_EvaluateSkillFormula, the block's owner).
    //  UNREACHABLE.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    //  BUG: the refusal is dead. UNREACHABLE: SKILLS_GetPathTargetPos answers 1 on every path.
    if (!SKILLS_GetPathTargetPos(pUnit, &nTargetX, &nTargetY))
        return 0;

    nRadius = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwAuraRangeCalc, nSkillId, nSkillLevel) - 3;
    if (nRadius < 2)
        nRadius = 2;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS;
    tParams.pOwner = pUnit;
    tParams.nX = nTargetX;
    tParams.nY = nTargetY;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;
    tParams.nMissile = MISSILE_CURSECENTER;
    pMissile = MISSILE_ClientSpawn(&tParams);
    if (!pMissile)
        return 0;

    CUNIT_GfxFreeUnitLightMap(pMissile);
    pLight = DLIGHTMAP_AddLight(pMissile, 1, 1, 0xFF, 0xFF, 0, 0);
    if (pLight)
    {
        DLIGHTMAP_SetTargetRadius(pLight, nRadius);
        pMissile->pLight = pLight;
    }

    D2COMMON_MISSILE_SetStreamMissile(pMissile, (uint16_t)nStreamMissile);
    D2COMMON_MISSILE_SetStreamRange(pMissile, (int16_t)nRadius);
    return 1;
}


//1.10f: D2Client.0x6FAC8150
//  Teeth and Bone Spear's cast: `cltstfunc` 19.
//    * The record, and `cltmissilec` (`bonecast` for both rows) as a SIGNED word inside [0, missiles.txt's count), or the answer is 0.
//    * SKILLS_GetPathTargetPos's point; its answer is tested and never 0.
//    * The offset from the unit's position to the point, and D2Common's UNITS_GetDistanceToCoordinates to it, at least 1 SIGNED (cmp edi, 1 / jg).
//    * The block, cleared, with flags 3, the owner, the start about two subtiles along the offset -- the unit's position, read afresh, plus twice
//      the offset over the distance, SIGNED divisions (cdq / idiv); the distance is the larger axis plus half the smaller, so the step is two on
//      the larger axis at most -- five times the offset as the target, the skill, the level and the missile. No missile, 0.
//    * The new missile's path turned to the unit's direction to the point (UNITS_GetDirectionToCoords, handed on to #10160 as the full dword;
//      #10160 reads the byte), and 1.
//  UNITS_GetDistanceToCoordinates is declared `unsigned int` and its answer is compared and divided by SIGNED here (jg, idiv); the C casts.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall SKILLS_CltStFunc19(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileId;
    int32_t nTargetX;
    int32_t nTargetY;
    int32_t nDeltaX;
    int32_t nDeltaY;
    int32_t nDistance;
    int32_t nStartX;
    int32_t nUnitY;
    D2UnitStrc *pMissile;
    D2MissileStrc tParams;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = (int16_t)pSkillsTxt->wCltMissileC;
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    //  BUG: the refusal is dead. UNREACHABLE: SKILLS_GetPathTargetPos answers 1 on every path.
    if (!SKILLS_GetPathTargetPos(pUnit, &nTargetX, &nTargetY))
        return 0;

    nDeltaX = nTargetX - UNITS_GetXPosition(pUnit);

    nDeltaY = nTargetY - UNITS_GetYPosition(pUnit);

    nDistance = (int32_t)D2COMMON_UNITS_GetDistanceToCoordinates(pUnit, nTargetX, nTargetY);
    if (nDistance <= 1)
        nDistance = 1;

    nStartX = UNITS_GetXPosition(pUnit) + 2 * nDeltaX / nDistance;

    nUnitY = UNITS_GetYPosition(pUnit);

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETOFFSET;
    tParams.pOwner = pUnit;
    tParams.nX = nStartX;
    tParams.nY = nUnitY + 2 * nDeltaY / nDistance;
    tParams.nTargetX = 5 * nDeltaX;
    tParams.nTargetY = 5 * nDeltaY;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;
    tParams.nMissile = nMissileId;
    pMissile = MISSILE_ClientSpawn(&tParams);
    if (!pMissile)
        return 0;

    //  BUG: the new missile's path goes untested into #10160, which reads it (mov edx, [ecx+30h]). UNREACHABLE.
    D2COMMON_10160_PathUpdateDirection(pMissile->pDynamicPath, (uint8_t)D2COMMON_UNITS_GetDirectionToCoords(pUnit, nTargetX, nTargetY));
    return 1;
}


//1.10f: D2Client.0x6FAC8370
//  Raising, the corpse's position: `cltstfunc` 20, Raise Skeleton's and Raise Skeletal Mage's start, reading neither nSkillId nor nSkillLevel and
//  no record. D2Common's UNITS_GetUsedSkill, CUNIT_GetPathTargetUnit and D2Common's #11017 on that unit with 0 -- true for a monster in its dead
//  mode, under no state of the udead mask, whose monstats2 row sets `corpseSel` and whose monstats row has a non-zero `Velocity` -- each 0 is
//  the answer. Then the corpse's position goes into the used skill's two parameters, SKILLS_SetParam1 its X and SKILLS_SetParam2 its Y, and the
//  answer is 1. `cltdofunc` 31, below, reads them back.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall SKILLS_CltStFunc20(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillStrc *pSkill;
    D2UnitStrc *pCorpse;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return 0;

    pCorpse = CUNIT_GetPathTargetUnit(pUnit);
    if (!pCorpse)
        return 0;

    if (!D2COMMON_11017_CheckUnitIfConsumeable(pCorpse, 0))
        return 0;

    //  BUG: no town test, where the server's SKILLS_SrvSt15_RaiseSkeleton_Mage refuses a corpse whose room is in town. NOT ESTABLISHED.
    D2COMMON_SKILLS_SetParam1(pSkill, UNITS_GetXPosition(pCorpse));

    D2COMMON_SKILLS_SetParam2(pSkill, UNITS_GetYPosition(pCorpse));
    return 1;
}


//1.10f: D2Client.0x6FAC8440
//  Raising, the corpse bursts: `cltdofunc` 31, Raise Skeleton's and Raise Skeletal Mage's do.
//    * The record, or the answer is 0; it is only tested.
//    * SKILLS_GetClientMissileForSkill -- `cltmissilea`, `corpseexplosion`, for both rows, neither of them progressive -- as an int inside
//      [0, missiles.txt's count), or 0.
//    * D2Common's UNITS_GetUsedSkill, or 0.
//    * The block, cleared, with flags 2001h, the owner, the skill, the level, the missile, and the used skill's two parameters as the position
//      (SKILLS_GetParam1 and SKILLS_GetParam2) -- the corpse's, which `cltstfunc` 20 wrote at the cast's start. MISSILE_ClientSpawn, its answer
//      not read; then both parameters set to 0, and 1.
//  So the two parameters carry one position from this skill's start to its do, on the client's own copy of the skill: the server does not write
//  them, and its own do, SKILLS_SrvDo031_RaiseSkeleton_Mage, takes the server's target unit again. The client draws the corpse bursting, which the
//  server does not make.
//  The original inlines DATATBLS_GetSkillsTxtRecordById here.
int32_t __fastcall SKILLS_CltDoFunc31(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    int32_t nMissileId;
    D2SkillStrc *pSkill;
    D2MissileStrc tParams;

    if (!DATATBLS_GetSkillsTxtRecordById(nSkillId))
        return 0;

    nMissileId = SKILLS_GetClientMissileForSkill(pUnit, nSkillId);
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return 0;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_RANDOMDIR;
    tParams.pOwner = pUnit;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;
    tParams.nMissile = nMissileId;
    tParams.nX = D2COMMON_SKILLS_GetParam1(pSkill);
    tParams.nY = D2COMMON_SKILLS_GetParam2(pSkill);
    MISSILE_ClientSpawn(&tParams);

    D2COMMON_SKILLS_SetParam1(pSkill, 0);
    D2COMMON_SKILLS_SetParam2(pSkill, 0);
    return 1;
}


//  The path literal SKILLS_CltStFunc23 passes as __FILE__ (in the original it is one object).
static const char szSkillsNecCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\Skills\\SkillsNec.cpp";
static_assert(sizeof(szSkillsNecCpp) == 64 + 1,
              "the path at 0x6FB779A0, 64 characters (`cstr_at.py`, scratch)");
static_assert(MONSTATS2FLAGINDEX_REVIVE == 8,
              "SKILLS_CltStFunc24: monstats2 byte 1 (`mov dl, [eax+5]`, 0x6FAC8DBE) against gdwBitMasks[0] "
              "(0x6FAC8DC6)");
static_assert(offsetof(D2MonStats2Txt, nOverlayHeight) == 0x0C,
              "SKILLS_CltStFunc24: `mov cl, [eax+0Ch]` (0x6FAC8E58), OverlayHeight");


//1.10f: D2Client.0x6FAC8530
//  The corpse to explode: `cltstfunc` 21, the start of the two corpse explosions and Nihlathak's (Corpse Explosion, Poison Explosion,
//  NihlathakCorpseExplosion), reading neither nSkillId nor nSkillLevel and no record.
//    * D2Common's UNITS_GetUsedSkill and CUNIT_GetPathTargetUnit; each 0 is the answer.
//    * When the caster is the local player (CUNIT_GetPlayer), D2Common's #11021 on the target -- SKILLS_CanUnitCorpseBeSelected: a monster in its
//      dead mode under no udead state whose monstats2 row sets `corpseSel` -- and its 0 is the answer. Another caster's target is not tested.
//    * The target's position into the used skill's Param1 and Param2 and its dwClassId into Param3 (SKILLS_SetParam3), and 1.
//      `cltdofunc` 32 and 33, below, read the three back.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall SKILLS_CltStFunc21(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillStrc *pSkill;
    D2UnitStrc *pCorpse;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return 0;

    pCorpse = CUNIT_GetPathTargetUnit(pUnit);
    if (!pCorpse)
        return 0;

    //  BUG: the corpse is tested only for the local player's cast, and no cast tests the town, where the server's
    //  SKILLS_SrvSt17_Poison_CorpseExplosion refuses a corpse whose room is in town and asks #11021 for every caster. NOT ESTABLISHED.
    if (pUnit == CUNIT_GetPlayer() && !D2COMMON_11021(pCorpse))
        return 0;

    D2COMMON_SKILLS_SetParam1(pSkill, UNITS_GetXPosition(pCorpse));

    D2COMMON_SKILLS_SetParam2(pSkill, UNITS_GetYPosition(pCorpse));

    D2COMMON_SKILLS_SetParam3(pSkill, pCorpse->dwClassId);
    return 1;
}


//1.10f: D2Client.0x6FAC85F0
//  The corpse bursts: the effect behind `cltdofunc` 32, called by SKILLS_CltDoFunc32. pUnit is in ecx and nSkillId in edx, and `retn 10h` pops
//  nSkillLevel, nX, nY and nCorpseClassId.
//    * The record, `cltmissilea` (corpseexplosion for both rows) a SIGNED word inside [0, missiles.txt's count), nX and nY not 0, and the corpse's
//      monstats2 row by its class (DATATBLS_GetMonStats2TxtRecord), or the answer is 0.
//    * UNITFLAG_SKSRVDOFUNC on a non-NULL unit.
//    * The block, cleared, with flags 2021h, the unit as owner AND origin, the missile, the skill, the level, the point as the position, and as the
//      target the point moved by one roll of the unit's seed on each axis -- the new low seed modulo 3, UNSIGNED (xor edx, edx / div), less 1.
//    * The corpse's size from its monstats2 flags (byte 1 against gdwBitMasks[2] and [3]; MONSTATS2FLAGINDEX_SMALL and _LARGE), through
//      SKILLS_SpawnScatteredMissiles (nJitter, nCount):
//
//          SMALL        (0, 1)
//          LARGE        (3, 3), then missiles.txt `SubMissile1` of `cltmissilea` (+18h; bigblood1) with (2, 4)
//          neither      (2, 2), then `SubMissile2` (+1Ah; bigblood2) with (1, 3)
//
//      -- a sub-missile only when its word is above 0 SIGNED and below the count. So one piece, seven or five. The three calls that follow a test
//      share one call instruction in the original (SMALL's by a jump, the sub-missiles' after storing the id): a merged tail, which the C writes
//      where each call runs.
//    * The point back into the block's position; `cltmissileb` (explodingarrowexp) above 0 SIGNED and in range, made by MISSILE_ClientSpawn and
//      kept; `cltmissilec` (redlightmissile) likewise, with the level 2, not kept.
//    * When the caster has no path target (CUNIT_GetPathTargetUnit) and the `cltmissileb` missile was made, `wTgtSound` above 0 SIGNED played on
//      it (SOUNDINST_PlaySound, its three other arguments 0). 1.
//  The original inlines DATATBLS_GetMonStats2TxtRecord, SEED_RollRandomNumber and the missiles.txt record lookup here.
int32_t __fastcall SKILLS_SpawnCorpseExplosionEffect(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nX, int32_t nY, D2C_MonsterIds nCorpseClassId)
{
    D2SkillsTxt *pSkillsTxt;
    D2MonStats2Txt *pMonStats2;
    D2MissilesTxt *pMissilesTxt;
    int32_t nMissileA;
    int32_t nMissileB;
    int32_t nMissileC;
    int32_t nSubMissile;
    int32_t nMissilesCount;
    int32_t nSoundId;
    D2UnitStrc *pMissile;
    D2MissileStrc tParams;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileA = (int16_t)pSkillsTxt->wCltMissileA;
    if (nMissileA < 0 || nMissileA >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    if (!nX || !nY)
        return 0;

    pMonStats2 = DATATBLS_GetMonStats2TxtRecord(nCorpseClassId);
    if (!pMonStats2)
        return 0;

    //  BUG: pUnit is tested here only for the flag; the unit's seed is read untested below. UNREACHABLE.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETPOS | MISSILESPAWN_RANDOMDIR;
    tParams.pOwner = pUnit;
    tParams.pOrigin = pUnit;
    tParams.nMissile = nMissileA;

    //  BUG: the target the two rolls give is dead: every arm calls SKILLS_SpawnScatteredMissiles, which writes nTargetX and nTargetY for each
    //  piece before MISSILE_ClientSpawn reads them. INERT: the rolls still advance the seed.
    tParams.nTargetX = (int32_t)((uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) % 3) + nX - 1;

    tParams.nTargetY = (int32_t)((uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) % 3) + nY - 1;

    tParams.nSkill = nSkillId;
    tParams.nX = nX;
    tParams.nY = nY;
    tParams.nSkillLevel = nSkillLevel;

    if (pMonStats2->nFlags[MONSTATS2FLAGINDEX_SMALL >> 3] & gdwBitMasks[MONSTATS2FLAGINDEX_SMALL & 7])
    {
        SKILLS_SpawnScatteredMissiles(pUnit, &tParams, 0, 1);
    }
    else if (pMonStats2->nFlags[MONSTATS2FLAGINDEX_LARGE >> 3] & gdwBitMasks[MONSTATS2FLAGINDEX_LARGE & 7])
    {
        SKILLS_SpawnScatteredMissiles(pUnit, &tParams, 3, 3);

        nMissilesCount = sgptDataTables->nMissilesTxtRecordCount;
        if (nMissileA >= nMissilesCount)
            pMissilesTxt = NULL;
        else
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileA];

        //  BUG: the sub-missile's row is read with no NULL test (the lookup has a NULL arm). UNREACHABLE: the same id passed the same count at the
        //  head of the function.
        nSubMissile = (int16_t)pMissilesTxt->wSubMissile[0];
        if (nSubMissile > 0 && nSubMissile < nMissilesCount)
        {
            tParams.nMissile = nSubMissile;
            SKILLS_SpawnScatteredMissiles(pUnit, &tParams, 2, 4);
        }
    }
    else
    {
        SKILLS_SpawnScatteredMissiles(pUnit, &tParams, 2, 2);

        nMissilesCount = sgptDataTables->nMissilesTxtRecordCount;
        if (nMissileA >= nMissilesCount)
            pMissilesTxt = NULL;
        else
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileA];

        //  BUG: read with no NULL test, as above. UNREACHABLE.
        nSubMissile = (int16_t)pMissilesTxt->wSubMissile[1];
        if (nSubMissile > 0 && nSubMissile < nMissilesCount)
        {
            tParams.nMissile = nSubMissile;
            SKILLS_SpawnScatteredMissiles(pUnit, &tParams, 1, 3);
        }
    }

    //  BUG: `cltmissileb` and `cltmissilec` go out with what the last scatter left in the block -- its last piece's target, and
    //  SKILLS_SetUnitDirection as the init callback with that piece's facing -- and only the position is put back. INERT: for the stock rows.
    tParams.nX = nX;
    tParams.nY = nY;

    pMissile = NULL;
    nMissileB = (int16_t)pSkillsTxt->wCltMissileB;
    if (nMissileB > 0 && nMissileB < sgptDataTables->nMissilesTxtRecordCount)
    {
        tParams.nMissile = nMissileB;
        pMissile = MISSILE_ClientSpawn(&tParams);
    }

    nMissileC = (int16_t)pSkillsTxt->wCltMissileC;
    if (nMissileC > 0 && nMissileC < sgptDataTables->nMissilesTxtRecordCount)
    {
        tParams.nSkillLevel = 2;
        tParams.nMissile = nMissileC;
        MISSILE_ClientSpawn(&tParams);
    }

    if (!CUNIT_GetPathTargetUnit(pUnit) && pMissile)
    {
        nSoundId = (int16_t)pSkillsTxt->wTgtSound;
        if (nSoundId > 0)
            SOUNDINST_PlaySound((D2C_Sounds)nSoundId, pMissile, 0, 0, 0);
    }

    return 1;
}


//1.10f: D2Client.0x6FAC88F0
//  Corpse Explosion's do: `cltdofunc` 32 (Corpse Explosion, NihlathakCorpseExplosion). D2Common's UNITS_GetUsedSkill, 0 without; then its Param1,
//  Param2 and Param3 -- the corpse's X, Y and class, which `cltstfunc` 21 wrote at the cast's start -- to SKILLS_SpawnCorpseExplosionEffect with
//  the skill and the level, whose answer is the answer. nSkillId waits in the slot the prologue's `push ecx` made.
int32_t __fastcall SKILLS_CltDoFunc32(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillStrc *pSkill;
    int32_t nCorpseX;
    int32_t nCorpseY;
    int32_t nCorpseClassId;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return 0;

    nCorpseX = D2COMMON_SKILLS_GetParam1(pSkill);
    nCorpseY = D2COMMON_SKILLS_GetParam2(pSkill);
    nCorpseClassId = D2COMMON_SKILLS_GetParam3(pSkill);
    return SKILLS_SpawnCorpseExplosionEffect(pUnit, nSkillId, nSkillLevel, nCorpseX, nCorpseY, (D2C_MonsterIds)nCorpseClassId);
}


//1.10f: D2Client.0x6FAC8940
//  The poison cloud: the effect behind `cltdofunc` 33, called by SKILLS_CltDoFunc33. pUnit is in ecx and nSkillId in edx, and `retn 10h` pops
//  nSkillLevel, nX, nY and nCorpseClassId.
//    * The record, `cltmissilea` (poisoncorpseexplosion) a SIGNED word inside [0, missiles.txt's count), the corpse's monstats row by its class
//      (DATATBLS_GetMonStatsTxtRecord) -- its address tested and nothing read from it -- and nX and nY not 0, or the answer is 0.
//    * UNITFLAG_SKSRVDOFUNC on a non-NULL unit.
//    * The block, cleared, with flags 2001h, the unit as owner, the missile, the skill, the level and the point as the position:
//      MISSILE_ClientSpawn, its answer kept. `cltmissilec` (greenlightmissile) above 0 SIGNED and in range is made from the same block with the
//      level 2, not kept.
//    * With the first missile made: `cltmissileb` (poisonexplosioncloud) above 0 SIGNED and in range, by MISSILE_SpawnFromMissile off it with
//      (0, 2, 0); and when the caster has no path target (CUNIT_GetPathTargetUnit), `wTgtSound` above 0 SIGNED played on it -- the three zeros
//      pushed from eax, the path target's NULL. 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById and DATATBLS_GetMonStatsTxtRecord here.
int32_t __fastcall SKILLS_SpawnPoisonExplosionEffect(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nX, int32_t nY, D2C_MonsterIds nCorpseClassId)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileA;
    int32_t nMissileB;
    int32_t nMissileC;
    int32_t nSoundId;
    D2UnitStrc *pMissile;
    D2MissileStrc tParams;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileA = (int16_t)pSkillsTxt->wCltMissileA;
    if (nMissileA < 0 || nMissileA >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    if (!DATATBLS_GetMonStatsTxtRecord(nCorpseClassId))
        return 0;

    if (!nX || !nY)
        return 0;

    //  BUG: pUnit is tested here only for the flag; it goes on untested as the block's owner and into CUNIT_GetPathTargetUnit. UNREACHABLE.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_RANDOMDIR;
    tParams.pOwner = pUnit;
    tParams.nMissile = nMissileA;
    tParams.nX = nX;
    tParams.nY = nY;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;
    pMissile = MISSILE_ClientSpawn(&tParams);

    nMissileC = (int16_t)pSkillsTxt->wCltMissileC;
    if (nMissileC > 0 && nMissileC < sgptDataTables->nMissilesTxtRecordCount)
    {
        tParams.nSkillLevel = 2;
        tParams.nMissile = nMissileC;
        MISSILE_ClientSpawn(&tParams);
    }

    nMissileB = (int16_t)pSkillsTxt->wCltMissileB;
    if (pMissile)
    {
        if (nMissileB > 0 && nMissileB < sgptDataTables->nMissilesTxtRecordCount)
            MISSILE_SpawnFromMissile(pMissile, nMissileB, 0, 2, 0);

        if (!CUNIT_GetPathTargetUnit(pUnit))
        {
            nSoundId = (int16_t)pSkillsTxt->wTgtSound;
            if (nSoundId > 0)
                SOUNDINST_PlaySound((D2C_Sounds)nSoundId, pMissile, 0, 0, 0);
        }
    }

    return 1;
}


//1.10f: D2Client.0x6FAC8AE0
//  Poison Explosion's do: `cltdofunc` 33. SKILLS_CltDoFunc32's body with SKILLS_SpawnPoisonExplosionEffect for its callee, instruction for
//  instruction.
int32_t __fastcall SKILLS_CltDoFunc33(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillStrc *pSkill;
    int32_t nCorpseX;
    int32_t nCorpseY;
    int32_t nCorpseClassId;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return 0;

    nCorpseX = D2COMMON_SKILLS_GetParam1(pSkill);
    nCorpseY = D2COMMON_SKILLS_GetParam2(pSkill);
    nCorpseClassId = D2COMMON_SKILLS_GetParam3(pSkill);
    return SKILLS_SpawnPoisonExplosionEffect(pUnit, nSkillId, nSkillLevel, nCorpseX, nCorpseY, (D2C_MonsterIds)nCorpseClassId);
}


//1.10f: D2Client.0x6FAC8B30
//  A wall or a prison of bone: `cltstfunc` 22, Bone Wall's and Bone Prison's start.
//    * The record (kept in a frame slot), or the answer is 0.
//    * D2Common's UNITS_GetRoom of the caster, or 0.
//    * SKILLS_GetPathTargetPos's point; its answer is tested and never 0.
//    * The room holding the point -- the caster's, or one adjacent to it (CLIENT_FindRoomContainingSubtile) -- or 0.
//    * The row's INTOWN flag (byte 1 of dwFlags against gdwBitMasks[0]) answers 1; without it, the room in town (D2Common's
//      DUNGEON_IsRoomInTown) answers 0 and any other 1. Stock Bone Wall and Bone Prison carry no InTown, so a point in town refuses the cast.
//  The original inlines DATATBLS_GetSkillsTxtRecordById and CLIENT_FindRoomContainingSubtile here; the adjacent-room walk's compare is unsigned.
int32_t __fastcall SKILLS_CltStFunc22(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2ActiveRoomStrc *pRoom;
    int32_t nTargetX;
    int32_t nTargetY;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    pRoom = D2COMMON_UNITS_GetRoom(pUnit);
    if (!pRoom)
        return 0;

    //  BUG: the refusal is dead. UNREACHABLE: SKILLS_GetPathTargetPos answers 1 on every path.
    if (!SKILLS_GetPathTargetPos(pUnit, &nTargetX, &nTargetY))
        return 0;

    pRoom = CLIENT_FindRoomContainingSubtile(pRoom, nTargetX, nTargetY);
    if (!pRoom)
        return 0;

    if (((uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_INTOWN >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_INTOWN & 7])
        return 1;

    //  BUG: the town is judged by the point's room, for both rows, where the server's SKILLS_SrvSt19_BonePrison asks the target's room, for the
    //  prison alone. NOT ESTABLISHED.
    if (D2COMMON_DUNGEON_IsRoomInTown(pRoom))
        return 0;

    return 1;
}


//1.10f: D2Client.0x6FAC8C80
//  The iron golem's metal: `cltstfunc` 23, IronGolem's start, reading neither nSkillId nor nSkillLevel and no record. CUNIT_GetPathTargetUnit must
//  be an item (cmp dword ptr [esi], 4) whose class is metal (D2Common's ITEMS_IsMetalItem), lying on the ground (IMODE_ONGROUND), identified
//  (ITEMS_CheckItemFlag, with this file's path and line 556 as its assert arguments), in a room (UNITS_GetRoom) and owned by nobody
//  (STATLIST_GetOwner with NULL); each refusal answers 0.
int32_t __fastcall SKILLS_CltStFunc23(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2UnitStrc *pItem;

    pItem = CUNIT_GetPathTargetUnit(pUnit);
    if (!pItem)
        return 0;

    if (pItem->dwUnitType != UNIT_ITEM)
        return 0;

    if (!D2COMMON_ITEMS_IsMetalItem(pItem->dwClassId))
        return 0;

    if (pItem->dwAnimMode != IMODE_ONGROUND)
        return 0;

    if (!D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_IDENTIFIED, 556, szSkillsNecCpp))
        return 0;

    //  BUG: the client refuses an item with no room, where the server's SKILLS_SrvSt20_IronGolem has no room test. NOT ESTABLISHED.
    if (!D2COMMON_UNITS_GetRoom(pItem))
        return 0;

    return D2COMMON_STATLIST_GetOwner(pItem, NULL) == NULL;
}


//1.10f: D2Client.0x6FAC8D00
//  Revive: `cltstfunc` 24.
//    * The record, or the answer is 0.
//    * CUNIT_GetPathTargetUnit, a monster (cmp dword ptr [ebp+0], 1), or 0.
//    * Its monstats2 row by its class (DATATBLS_GetMonStats2TxtRecord), or 0; and the row's REVIVE flag (byte 1 against gdwBitMasks[0]), or 0 --
//      the flag's value kept in a frame slot and never read back.
//    * D2Common's #11017 with 0, and D2Client's MONSTER_CanBeRevived (a monster whose class can switch AI, D2Common's UNITS_CanSwitchAI, and whose
//      type flags 8 and 2 are clear, MONSTER_TestTypeFlag), each 0 answering 0.
//    * The monstats2 row a second time, or 0; its `OverlayHeight` picks the missile -- 1 `cltmissileb` (revivesmall), 3 `cltmissilec`
//      (revivelarge), any other `cltmissilea` (revivemedium), which is loaded first -- and one outside [0, missiles.txt's count), SIGNED,
//      answers 0.
//    * The block, cleared, with flags 2000h, the caster as owner, the corpse as ORIGIN, the skill, the level and the missile, and no position.
//      The answer is whether MISSILE_ClientSpawn made one (neg / sbb / neg).
//  The server's twin, SKILLS_SrvSt21_Revive, makes its own AI test with state 7; whether that test and MONSTER_CanBeRevived answer alike for every
//  monster is not established.
//  The original inlines DATATBLS_GetSkillsTxtRecordById and, twice, DATATBLS_GetMonStats2TxtRecord here.
BOOL __fastcall SKILLS_CltStFunc24(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2UnitStrc *pCorpse;
    D2MonStats2Txt *pMonStats2;
    int32_t nMissileId;
    D2MissileStrc tParams;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return FALSE;

    pCorpse = CUNIT_GetPathTargetUnit(pUnit);
    if (!pCorpse)
        return FALSE;

    if (pCorpse->dwUnitType != UNIT_MONSTER)
        return FALSE;

    pMonStats2 = DATATBLS_GetMonStats2TxtRecord(pCorpse->dwClassId);
    if (!pMonStats2)
        return FALSE;

    if (!(pMonStats2->nFlags[MONSTATS2FLAGINDEX_REVIVE >> 3] & gdwBitMasks[MONSTATS2FLAGINDEX_REVIVE & 7]))
        return FALSE;

    if (!D2COMMON_11017_CheckUnitIfConsumeable(pCorpse, 0))
        return FALSE;

    if (!MONSTER_CanBeRevived(pCorpse))
        return FALSE;

    pMonStats2 = DATATBLS_GetMonStats2TxtRecord(pCorpse->dwClassId);
    if (!pMonStats2)
        return FALSE;

    nMissileId = (int16_t)pSkillsTxt->wCltMissileA;
    if (pMonStats2->nOverlayHeight == 1)
        nMissileId = (int16_t)pSkillsTxt->wCltMissileB;
    else if (pMonStats2->nOverlayHeight == 3)
        nMissileId = (int16_t)pSkillsTxt->wCltMissileC;

    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return FALSE;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_RANDOMDIR;
    tParams.pOwner = pUnit;
    tParams.pOrigin = pCorpse;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;
    tParams.nMissile = nMissileId;
    return MISSILE_ClientSpawn(&tParams) != NULL;
}

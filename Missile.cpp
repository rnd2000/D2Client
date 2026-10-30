#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Game.h"
#include "CoreArchive.h"
#include "Missile.h"

//  Missile.cpp -- the client side of the missiles in D2Client.dll 1.10 (image base 0x6FAA0000): the per-frame handlers one per Missiles.txt
//  pCltDoFunc, the hit handlers one per pCltHitFunc, the per-frame update, the spawn and the helpers they share.
//  Address range 0x6FB2FF20..0x6FB3EDEE. The original source file is UNIT\Missile.cpp (the asserts in this unit name it). gpMissileCltDoFuncs
//  (0x6FB91260, 91 slots indexed by Missiles.txt pCltDoFunc) and gpMissileCltHitFuncs (0x6FB913D0, indexed by pCltHitFunc) hold the handlers, which
//  the original declares `void (__thiscall *)(D2UnitStrc *)`. A free function cannot be written __thiscall; with `this` as the only argument
//  __fastcall has the same ABI, so the handlers are written so.

//  The path literal the family passes as __FILE__ (60 characters at 0x6FB91518).
static const char szMissileCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\UNIT\\Missile.cpp";

//  Layout checks: the spawn-block fields and the Missiles.txt fields the handlers read.
static_assert(offsetof(D2MissileStrc, nLoops) == 0x34
              && offsetof(D2MissileStrc, nRange) == 0x4C,
              "MISSILE_CltSpawnTrailSubMissile: [esp+64h+var_28] and var_10 "
              "against pParams at var_5C (0x6FB3032D, 0x6FB30319)");
static_assert(offsetof(D2MissilesTxt, wCltSubMissile) == 0x1E
              && offsetof(D2MissilesTxt, dwCltParam) == 0x58
              && offsetof(D2MissilesTxt, dwCltCalc) == 0x84
              && offsetof(D2MissilesTxt, nSubStart) == 0x181
              && offsetof(D2MissilesTxt, nSubStop) == 0x182,
              "`word ptr [esi+1Eh]` 0x6FB30425; [eax+60h/5Ch/58h] "
              "0x6FB306CD..0x6FB306D5; [esi+84h] 0x6FB30432; [edi+181h], "
              "[edi+182h] 0x6FB30736, 0x6FB3074C");
static_assert(sizeof(D2MissilesTxt) == 0x1A4,
              "the record stride: id*8 - id, *3, *5, *4 (0x6FB30409..0x6FB3041E)");
static_assert(offsetof(D2MissilesTxt, wProgSound) == 0x34
              && offsetof(D2MissilesTxt, nInitSteps) == 0x135,
              "MISSILE_CltDoFunc9 `mov ax, [edx+34h]` 0x6FB3114C; "
              "MISSILE_CltDoFunc8 `mov cl, [ebx+135h]` 0x6FB30D86");
static_assert(offsetof(D2MissilesTxt, wCltHitSubMissile) == 0x2C
              && offsetof(D2MissilesTxt, wProgOverlay) == 0x36
              && offsetof(D2MissilesTxt, dwCltHitPar) == 0x6C
              && offsetof(D2MissilesTxt, dwRandStart) == 0x17C,
              "MISSILE_CltHitFunc64 `cmp word ptr [ebx+2Ch], 0` 0x6FB361AD; "
              "MISSILE_CltHitFunc9 `mov si, [esi+36h]` 0x6FB365F7; MISSILE_CltHitFunc1 "
              "[edi+70h] 0x6FB362AA, [edi+6Ch] 0x6FB362BD; MISSILE_SpawnOverArea "
              "`mov edi, [ecx+17Ch]` 0x6FB35D9B");
static_assert(offsetof(D2MissileStrc, nFrame) == 0x40,
              "MISSILE_SpawnOverArea: [esp+90h+var_1C] against pParams at var_5C (0x6FB35ECD)");
//  dwHitCalc is the server's hit calc, which a client handler evaluates.
static_assert(offsetof(D2MissilesTxt, dwHitCalc) == 0x88
              && offsetof(D2MissilesTxt, dwCltHitCalc) == 0x8C,
              "MISSILE_CltHitFunc12 `mov eax, [ebp+88h]` 0x6FB36834");
//  The server's hit sub-missile column pCltHitFunc 26 gates on, the skill's AuraFilter, and the visitor block's cap and id.
static_assert(offsetof(D2MissilesTxt, wHitSubMissile) == 0x24
              && offsetof(D2SkillsTxt, dwAuraFilter) == 0x50
              && offsetof(D2ClientSubMissileSpawnStrc, nMaxTargets) == 0x10
              && offsetof(D2ClientSubMissileSpawnStrc, nSubMissileId) == 0x14,
              "MISSILE_CltHitFunc26 `cmp word ptr [eax+24h], 0` 0x6FB37853, `mov eax, [eax+50h]` "
              "0x6FB37999; MISSILE_SpawnOnUnitCallback `cmp eax, [edx+10h]` 0x6FB3774C, "
              "`mov ecx, [edx+14h]` 0x6FB3778B");


//1.10f: D2Client.0x6FB2FF20
//  Scatters ground fire over a disc. Callers: MISSILE_CltHitFunc36 (hellmeteordown) and MISSILE_CltHitFunc12 (immolationarrow). For every subtile of
//  the square -nRadius..+nRadius around the missile, in this order of tests: a room under the missile at all; inside the circle, dx*dx + dy*dy <=
//  nRadius*nRadius; the ray, COLLISION_RayTrace(room, (x+dx, y+dy), (x+2dx, y+2dy), 4), which the original casts from the subtile outward, not from
//  the impact; the room that holds the subtile, and that room not in town. Then the roll `% 100 < nChancePercent`, the remainder unsigned (`div`) and
//  the compare signed (`jge`).
//  A non-zero nRange sets the spawn flags to 0x8001 and, per subtile, tParams.nRange to `seed % 25 + nRange - 12`: the client's range is jittered.
//  void: eax at the exit is a leftover that both callers discard. The missile pick is D2Common's seed inline, written as that inline.
//  The original inlines UNITS_GetXPosition, UNITS_GetYPosition, CLIENT_FindRoomContainingSubtile and SEED_RollRandomNumber here.
void __fastcall MISSILE_SprayRandomOverArea(D2UnitStrc *pMissile, int32_t *paMissileIds, int32_t nMissileCount, int32_t nRadius, int32_t nRange, int32_t nChancePercent)
{
    D2MissileStrc tParams;
    D2SeedStrc *pSeed;
    D2ActiveRoomStrc *pRoom;
    D2CoordStrc tBeginCoord;
    D2CoordStrc tEndCoord;
    int32_t nCenterX;
    int32_t nCenterY;
    int32_t nRadiusSquared;
    int32_t nDx;
    int32_t nDy;
    int32_t nTileX;
    int32_t nTileY;

    if (!D2COMMON_UNITS_GetRoom(pMissile))
        return;

    //  The seed macro's own test of the address of a member of a unit already read (0x6FB2FF42..0x6FB2FF49).
    pSeed = &pMissile->pSeed;
    if (!pSeed)
        return;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS;
    if (nRange)
        tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_RANGE;
    tParams.pOwner = CUNIT_GetOwnerUnit(pMissile);
    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
    tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);

    nCenterX = UNITS_GetXPosition(pMissile);

    nCenterY = UNITS_GetYPosition(pMissile);

    nRadiusSquared = nRadius * nRadius;
    for (nDx = -nRadius; nDx <= nRadius; ++nDx)
    {
        nTileX = nCenterX + nDx;
        for (nDy = -nRadius; nDy <= nRadius; ++nDy)
        {
            nTileY = nCenterY + nDy;

            pRoom = D2COMMON_UNITS_GetRoom(pMissile);
            if (!pRoom)
                continue;

            if (nDx * nDx + nDy * nDy > nRadiusSquared)
                continue;

            tBeginCoord.nX = nTileX;
            tBeginCoord.nY = nTileY;
            tEndCoord.nX = nTileX + nDx;
            tEndCoord.nY = nTileY + nDy;
            if (D2COMMON_COLLISION_RayTrace(pRoom, &tBeginCoord, &tEndCoord, 4))
                continue;

            pRoom = CLIENT_FindRoomContainingSubtile(D2COMMON_UNITS_GetRoom(pMissile), nTileX, nTileY);
            if (!pRoom || D2COMMON_DUNGEON_IsRoomInTown(pRoom))
                continue;

            if (nRange)
            {
                tParams.nRange = (int32_t)((uint32_t)SEED_RollRandomNumber(pSeed) % 25) + nRange - 12;
            }

            tParams.nX = nTileX;
            tParams.nY = nTileY;
            //  D2Common's seed inline: the modulo arm folded in, the power-of-two arm a call to SEED_RollRandomNumber and `and`.
            tParams.nMissile = paMissileIds[SEED_RollLimitedRandomNumberInline(pSeed, nMissileCount)];

            if ((int32_t)((uint32_t)SEED_RollRandomNumber(pSeed) % 100) < nChancePercent)
                MISSILE_ClientSpawn(&tParams);
        }
    }
}


//1.10f: D2Client.0x6FB30260
//  pCltDoFunc 2: blood1, blood2, bigblood1, bigblood2, sand pile, dirt pile. A spray that dies with its animation and is culled off screen: at the
//  end of its frame cycle the unit is marked dead and its path stopped; then, if its client position is outside the viewport, it is exploded at once
//  (MISSILE_ClientOnHit, no target, not forced), else its frame is set to 128 and the common per-frame work runs.
//  The cull tests x < 0, y < 0, x > gnViewportRight and y > gnViewportBottom + 0x40: 64 pixels of slack at the bottom only.
void __fastcall MISSILE_CltDoFunc2(D2UnitStrc *pMissile)
{
    int32_t nScreenX;
    int32_t nScreenY;

    if (D2COMMON_UNITS_IsAtEndOfFrameCycle(pMissile))
    {
        CUNIT_SetUnitDead(pMissile);
        D2COMMON_PATH_SetVelocity(pMissile->pDynamicPath, 0, szMissileCpp, 127);
    }

    nScreenX = D2COMMON_UNITS_GetClientCoordX(pMissile) - SCROLL_GetViewportOriginX();
    if (nScreenX >= 0 && nScreenX <= gnViewportRight)
    {
        nScreenY = D2COMMON_UNITS_GetClientCoordY(pMissile) - SCROLL_GetViewportOriginY();
        if (nScreenY >= 0 && nScreenY <= gnViewportBottom + 64)
        {
            D2COMMON_MISSILE_SetCurrentFrame(pMissile, 128);
            MISSILE_CltDoFunc1(pMissile);
            return;
        }
    }

    MISSILE_ClientOnHit(pMissile, NULL, 0);
}


//1.10f: D2Client.0x6FB302E0
//  Spawns one sub-missile where the parent stands, in a frame it moved; the whole body is behind PATH_HasMovedThisFrame. The spawn is flags 5,
//  owner, origin the parent, at the parent's position, with the parent's skill and level; nRange > 0 adds the range and 0x8000, nLoops > 0 the
//  loop count and 8 (`or al, 8` on the whole word).
//  BUG: nLoops is overwritten: the last store before the spawn is `2 * nLevel - 2` into the same field (0x6FB303C2, 0x6FB303C6), whatever the
//  argument was; only the 8 it set in the flags survives. REACHABLE: its one caller, pCltDoFunc 3, passes its evaluated dwCltCalc there.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
//  void: eax is the spawned missile, or PATH_HasMovedThisFrame's zero when the body is skipped, and no caller reads it.
void __fastcall MISSILE_CltSpawnTrailSubMissile(D2UnitStrc *pMissile, int32_t nSubMissileId, int32_t nRange, int32_t nLoops)
{
    D2MissileStrc tParams;
    int32_t nLevel;

    if (!D2COMMON_PATH_HasMovedThisFrame(pMissile->pDynamicPath))
        return;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_VELOCITY;
    if (nRange > 0)
    {
        tParams.nRange = nRange;
        tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_VELOCITY | MISSILESPAWN_RANGE;
    }
    if (nLoops > 0)
    {
        tParams.nLoops = nLoops;
        tParams.dwFlags |= MISSILESPAWN_SUBLOOPS;
    }

    tParams.pOwner = CUNIT_GetOwnerUnit(pMissile);
    tParams.pOrigin = pMissile;
    tParams.nMissile = nSubMissileId;

    tParams.nX = UNITS_GetXPosition(pMissile);

    tParams.nY = UNITS_GetYPosition(pMissile);

    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
    nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    tParams.nSkillLevel = nLevel;
    tParams.nLoops = 2 * nLevel - 2;
    MISSILE_ClientSpawn(&tParams);
}


//1.10f: D2Client.0x6FB303E0
//  pCltDoFunc 3: poisonjav, plaguejavelin, the two trap poison balls, viper_poisjav. In a frame the javelin moved, its CltSubMissile1 is laid behind
//  it -- unless that id is negative: a signed test of an unsigned column (`jl`), and `movsx` for the argument. The loop count is dwCltCalc evaluated
//  for the missile against its OWNER. Then the common per-frame work, always.
//  The original inlines the Missiles.txt record lookup here.
void __fastcall MISSILE_CltDoFunc3(D2UnitStrc *pMissile)
{
    D2MissilesTxt *pMissilesTxt;
    int32_t nMissileId;
    int32_t nLevel;
    int32_t nLoops;

    if (D2COMMON_PATH_HasMovedThisFrame(pMissile->pDynamicPath))
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
        {
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
            if (pMissilesTxt && (int16_t)pMissilesTxt->wCltSubMissile[0] >= 0)
            {
                nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
                nLoops = D2COMMON_MISSILE_EvaluateMissileFormula(pMissile, CUNIT_GetOwnerUnit(pMissile), pMissilesTxt->dwCltCalc, nMissileId, nLevel);
                MISSILE_CltSpawnTrailSubMissile(pMissile, (int16_t)pMissilesTxt->wCltSubMissile[0], 0, nLoops);
            }
        }
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB30470
//  Puffs from a lingering cloud. Callers: pCltDoFunc 4 and 45, both passing the cloud's Missiles.txt dwCltParam[0..2] and CltSubMissile1 as
//  nEveryNFrames, nCount, nSpread and the puff.
//  After the first frame it fires only when a roll below nEveryNFrames comes up 0 -- an average, not a period; a frame of 0 or an nEveryNFrames of 0
//  or less always fires. Each of nCount puffs goes flags 0x20, owner, origin the cloud, at a target point offset from the cloud by a roll in [0,
//  2*nSpread) less nSpread, and then pushed a further nSpread outward on each axis by the offset's own sign. So each coordinate's offset lies in
//  [-2*nSpread, -nSpread) or [nSpread, 2*nSpread): a square frame around the cloud's centre, not a disc.
//  The original inlines UNITS_GetXPosition, UNITS_GetYPosition and SEED_RollLimitedRandomNumber (the gate and one roll per axis) here.
void __fastcall MISSILE_EmitCloudPuffs(D2UnitStrc *pMissile, int32_t nEveryNFrames, int32_t nCount, int32_t nSpread, int32_t nPuffMissileId)
{
    D2MissileStrc tParams;
    int32_t nCloudX;
    int32_t nCloudY;
    int32_t nSpreadTwice;
    int32_t nOffsetX;
    int32_t nOffsetY;

    if (nPuffMissileId < 0 || nPuffMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return;

    if (D2COMMON_MISSILE_GetCurrentFrame(pMissile) && SEED_RollLimitedRandomNumber(&pMissile->pSeed, nEveryNFrames))
    {
        return;
    }

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_TARGETPOS;
    tParams.pOwner = CUNIT_GetOwnerUnit(pMissile);
    tParams.pOrigin = pMissile;
    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
    tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    tParams.nMissile = nPuffMissileId;

    nCloudX = UNITS_GetXPosition(pMissile);

    nCloudY = UNITS_GetYPosition(pMissile);

    if (nCount > 0)
    {
        nSpreadTwice = 2 * nSpread;
        do
        {
            nOffsetX = SEED_RollLimitedRandomNumber(&pMissile->pSeed, nSpreadTwice)
                       - nSpread;

            nOffsetY = SEED_RollLimitedRandomNumber(&pMissile->pSeed, nSpreadTwice)
                       - nSpread;

            tParams.nTargetX = nCloudX + nOffsetX + (nOffsetX < 0 ? -nSpread : nSpread);
            tParams.nTargetY = nCloudY + nOffsetY + (nOffsetY < 0 ? -nSpread : nSpread);
            MISSILE_ClientSpawn(&tParams);
        }
        while (--nCount);
    }
}


//1.10f: D2Client.0x6FB30690
//  pCltDoFunc 4: the poison and gas clouds, from poisonjavcloud to trappoisonjavcloud. Puffs from the cloud's own row, then the common per-frame
//  work, outside the guard: a missile with no record still gets it.
//  The original inlines the Missiles.txt record lookup here.
void __fastcall MISSILE_CltDoFunc4(D2UnitStrc *pMissile)
{
    D2MissilesTxt *pMissilesTxt;
    int32_t nMissileId;

    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
        {
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
            if (pMissilesTxt)
            {
                MISSILE_EmitCloudPuffs(pMissile, (int32_t)pMissilesTxt->dwCltParam[0], (int32_t)pMissilesTxt->dwCltParam[1], (int32_t)pMissilesTxt->dwCltParam[2], (int16_t)pMissilesTxt->wCltSubMissile[0]);
            }
        }
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB306F0
//  pCltDoFunc 5: the fire walls and ground fires, from blaze to moltenboulderfirepath. A sub-animation window SubStart..SubStop that the frame
//  shuffles inside:
//    frame == SubStart - 1   jump forward a roll below SubStop - SubStart
//    frame == SubStart       back to SubStart - 3, floored at 0
//    frame <  SubStart       back 2, floored at 0
//  all through the 8.8 nSeqCurrentFramePrecise (`sar ebx, 8` in, `shl` out). The first test reads the frame from the field, the other two from
//  MISSILE_GetCurrentFrame; the third compare is signed (`jge`). A missile with no record is destroyed.
//  The original inlines the Missiles.txt record lookup and SEED_RollLimitedRandomNumber here.
void __fastcall MISSILE_CltDoFunc5(D2UnitStrc *pMissile)
{
    D2MissilesTxt *pMissilesTxt;
    int32_t nMissileId;
    int32_t nSubStart;
    int32_t nFrame;
    int32_t nBackFrame;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }

    if (!pMissilesTxt)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    nSubStart = pMissilesTxt->nSubStart;
    nFrame = pMissile->nSeqCurrentFramePrecise >> 8;
    if (nFrame == nSubStart - 1)
    {
        pMissile->nSeqCurrentFramePrecise = (nFrame + SEED_RollLimitedRandomNumber(&pMissile->pSeed, pMissilesTxt->nSubStop - nSubStart)) << 8;
    }
    else if (D2COMMON_MISSILE_GetCurrentFrame(pMissile) == pMissilesTxt->nSubStart)
    {
        nBackFrame = pMissilesTxt->nSubStart - 3;
        if (nBackFrame < 0)
            nBackFrame = 0;
        pMissile->nSeqCurrentFramePrecise = nBackFrame << 8;
    }
    else if (D2COMMON_MISSILE_GetCurrentFrame(pMissile) < pMissilesTxt->nSubStart)
    {
        nFrame -= 2;
        if (nFrame < 0)
            nFrame = 0;
        pMissile->nSeqCurrentFramePrecise = nFrame << 8;
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB30820
//  pCltDoFunc 6: firewallmaker, vampirefirewallmaker, firestormmaker, countessfirewallmaker. In a frame the maker moved, it drops a piece of the wall
//  where it stands, flags 0x21, target its own position again:
//    * the piece is CltSubMissile1, or -- when CltSubMissile2 is present -- CltSubMissile2 on a coin's tails (`and eax, 1`), or -- when
//      CltSubMissile3 is present too -- one of the three by `% 3` (0 the second, 1 the third, 2 the first);
//    * the piece's own record's Range, when nonzero, as the range and 0x8000;
//    * 0x4000 when CltParam1 is 0, or when a roll below it is nonzero;
//    * once spawned, the piece takes the maker's precise position (UNITS_GetPrecisionX/Y onto PATH_SetPrecisionX/Y).
//  A maker with no record, a negative CltSubMissile1 or a first sub-missile with no record is destroyed and gets no per-frame work; one that did not
//  move gets it and spawns nothing. The owner is fetched first, before the path is asked.
//  The original inlines the Missiles.txt record lookup, UNITS_GetXPosition, UNITS_GetYPosition and SEED_RollRandomNumber here.
void __fastcall MISSILE_CltDoFunc6(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2MissilesTxt *pSubMissilesTxt;
    D2UnitStrc *pOwner;
    D2UnitStrc *pSpawned;
    D2DynamicPathStrc *pSpawnedPath;
    int32_t nMissileId;
    int32_t nSubMissileId;
    int32_t nX;
    int32_t nY;
    int32_t nFlagRange;
    int32_t nPrecisionX;
    int32_t nPrecisionY;

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!D2COMMON_PATH_HasMovedThisFrame(pMissile->pDynamicPath))
    {
        MISSILE_CltDoFunc1(pMissile);
        return;
    }

    nMissileId = pMissile->dwClassId;
    pMissilesTxt = NULL;
    if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
        pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltSubMissile[0] < 0)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    nX = UNITS_GetXPosition(pMissile);

    nY = UNITS_GetYPosition(pMissile);

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETPOS;
    tParams.pOwner = pOwner;
    tParams.nX = nX;
    tParams.nY = nY;
    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
    tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    tParams.nMissile = (int16_t)pMissilesTxt->wCltSubMissile[0];

    nSubMissileId = (int16_t)pMissilesTxt->wCltSubMissile[0];
    pSubMissilesTxt = NULL;
    if (nSubMissileId >= 0 && nSubMissileId < sgptDataTables->nMissilesTxtRecordCount)
        pSubMissilesTxt = &sgptDataTables->pMissilesTxt[nSubMissileId];
    if (!pSubMissilesTxt)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    if (pSubMissilesTxt->wRange)
    {
        tParams.dwFlags |= MISSILESPAWN_RANGE;
        tParams.nRange = (int16_t)pSubMissilesTxt->wRange;
    }

    if ((int16_t)pMissilesTxt->wCltSubMissile[1] >= 0)
    {
        if ((int16_t)pMissilesTxt->wCltSubMissile[2] < 0)
        {
            if (!(SEED_RollRandomNumber(&pMissile->pSeed) & 1))
                tParams.nMissile = (int16_t)pMissilesTxt->wCltSubMissile[1];
        }
        else
        {
            switch ((uint32_t)SEED_RollRandomNumber(&pMissile->pSeed) % 3)
            {
            case 0:
                tParams.nMissile = (int16_t)pMissilesTxt->wCltSubMissile[1];
                break;
            case 1:
                tParams.nMissile = (int16_t)pMissilesTxt->wCltSubMissile[2];
                break;
            default:
                break;
            }
        }
    }

    //  D2Common's seed inline: the modulo arm folded in, the power-of-two arm a call and `and`; a zero CltParam1 sets the flag before any roll, a
    //  negative one skips both.
    nFlagRange = (int32_t)pMissilesTxt->dwCltParam[0];
    if (!nFlagRange || SEED_RollLimitedRandomNumberInline(&pMissile->pSeed, nFlagRange))
        tParams.dwFlags |= MISSILESPAWN_NOLIGHT;

    tParams.nTargetX = UNITS_GetXPosition(pMissile);

    tParams.nTargetY = UNITS_GetYPosition(pMissile);

    pSpawned = MISSILE_ClientSpawn(&tParams);
    if (pSpawned)
    {
        nPrecisionX = D2COMMON_UNITS_GetPrecisionX(pMissile);
        nPrecisionY = D2COMMON_UNITS_GetPrecisionY(pMissile);
        pSpawnedPath = pSpawned->pDynamicPath;
        D2COMMON_PATH_SetPrecisionX(pSpawnedPath, nPrecisionX);
        D2COMMON_PATH_SetPrecisionY(pSpawnedPath, nPrecisionY);
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB30AD0
//  pCltDoFunc 7: guidedarrow, bonespirit, monbonespirit. The homing step: while bit 0 of MISSILE_GetTargetX is set (`test al, 1`), the stale target
//  is cleared (UNITMODE_ClearStaleTargetUnit), the path's target re-read and dropped if dead or, with an owner, not a selectable target for it; and
//  every CltParam1 frames -- 5 when the column is not positive -- the move restarts if the target is more than 3 and less than 25 away. The period is
//  a signed remainder of MISSILE_GetRemainingFrames (`cdq / idiv`). A missile with no record, or in town, is destroyed. Both exits are tail jumps: it
//  returns nothing.
//  The original inlines the Missiles.txt record lookup here.
void __fastcall MISSILE_CltDoFunc7(D2UnitStrc *pMissile)
{
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pOwner;
    D2UnitStrc *pTarget;
    int32_t nMissileId;
    int32_t nPeriod;
    int32_t nDistance;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (D2COMMON_DUNGEON_IsRoomInTown(D2COMMON_UNITS_GetRoom(pMissile)))
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    if (D2COMMON_MISSILE_GetTargetX(pMissile) & 1)
    {
        UNITMODE_ClearStaleTargetUnit(pMissile);
        pTarget = D2COMMON_PATH_GetTargetUnit(pMissile->pDynamicPath);
        if (pTarget && (CUNIT_IsUnitDead(pTarget) || (pOwner && !CUNIT_IsUnitSelectableTarget(pOwner, pTarget))))
        {
            pTarget = NULL;
        }

        nPeriod = (int32_t)pMissilesTxt->dwCltParam[0];
        if (nPeriod <= 0)
            nPeriod = 5;

        if (pTarget && !(D2COMMON_MISSILE_GetRemainingFrames(pMissile) % nPeriod))
        {
            nDistance = D2COMMON_UNITS_GetDistanceToOtherUnit(pMissile, pTarget);
            if (nDistance > 3 && nDistance < 25)
                D2COMMON_PATH_StartUnitMove(pMissile->pDynamicPath, pMissile, 0);
        }
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB30BD0
//  A new missile's look. No code caller: pCltDoFunc 8 and 22 store it as the spawn block's pInitFunc, with the parent as pInitArgs, and
//  MISSILE_ClientSpawn calls it on the new missile. Three steps, the two rolls on two different seeds:
//    * PARTICLE_SetUnitGfxCoords(missile, 0, 0, z), z a roll below CltParam2 on the PARENT's seed less CltParam2 / 2 (a signed halving, `cdq / sub /
//      sar`); 0 for a CltParam2 of 0 or less;
//    * the parent's direction onto the missile's path, rotation flag cleared (D2Common #10203 with 0);
//    * the animation's start frame, a roll below AnimLen on the missile's own seed, into nSeqCurrentFramePrecise (`shl edx, 8`); 0 for an AnimLen of
//      0.
//  The second parameter is the parent MISSILE, not its owner: both storers put the spawning missile itself in pInitArgs. A NULL missile, a missile
//  with no record or a NULL parent does nothing. void.
//  The original inlines the Missiles.txt record lookup and SEED_RollLimitedRandomNumber here.
void __fastcall MISSILE_RandomiseSpawnAppearance(D2UnitStrc *pMissile, D2UnitStrc *pParent)
{
    D2MissilesTxt *pMissilesTxt;
    int32_t nMissileId;
    int32_t nSpread;
    int32_t nZ;
    uint8_t nDirection;

    if (!pMissile)
        return;

    nMissileId = pMissile->dwClassId;
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return;
    pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    if (!pMissilesTxt || !pParent)
        return;

    nZ = 0;
    nSpread = (int32_t)pMissilesTxt->dwCltParam[1];
    if (nSpread > 0)
    {
        nZ = SEED_RollLimitedRandomNumber(&pParent->pSeed, nSpread)
             - (int32_t)pMissilesTxt->dwCltParam[1] / 2;
    }

    PARTICLE_SetUnitGfxCoords(pMissile, 0, 0, nZ);
    nDirection = D2COMMON_PATH_GetDirection(pParent->pDynamicPath);
    D2COMMON_PATH_SetDirection(pMissile->pDynamicPath, nDirection);
    D2COMMON_10203_PATH_SetRotateFlag(pMissile->pDynamicPath, 0);

    pMissile->nSeqCurrentFramePrecise = SEED_RollLimitedRandomNumber(&pMissile->pSeed, pMissilesTxt->nAnimLen) << 8;
}


//1.10f: D2Client.0x6FB30D20
//  pCltDoFunc 8: the lightning bolts -- chainlightning, lightningbolt and more. Once MISSILE_GetRemainingFrames has reached the row's InitSteps (a
//  signed compare with the byte, `jl`) and in a frame the bolt moved, it spawns CltSubMissile1 at its position, flags 1, with CltParam1 as the loop
//  count (1 when below 1) and MISSILE_RandomiseSpawnAppearance as the on-spawn hook, this bolt its argument -- which is how the new piece faces the
//  way the bolt does and takes a random height and start frame. A bolt with no record or a negative CltSubMissile1 is destroyed (two exits, one
//  effect). The hook is stored as a pointer: the original's MISSILE_ClientSpawn calls it through the spawn block.
//  The original inlines the Missiles.txt record lookup, UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall MISSILE_CltDoFunc8(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    int32_t nMissileId;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltSubMissile[0] < 0)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    if (D2COMMON_MISSILE_GetRemainingFrames(pMissile) >= pMissilesTxt->nInitSteps && D2COMMON_PATH_HasMovedThisFrame(pMissile->pDynamicPath))
    {
        memset(&tParams, 0, sizeof(tParams));
        tParams.dwFlags = MISSILESPAWN_STARTPOS;
        tParams.pOwner = CUNIT_GetOwnerUnit(pMissile);

        tParams.nX = UNITS_GetXPosition(pMissile);

        tParams.nY = UNITS_GetYPosition(pMissile);

        tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
        tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        tParams.nMissile = (int16_t)pMissilesTxt->wCltSubMissile[0];
        tParams.nLoops = 1;
        if ((int32_t)pMissilesTxt->dwCltParam[0] >= 1)
            tParams.nLoops = (int32_t)pMissilesTxt->dwCltParam[0];
        tParams.pInitFunc = (void (__fastcall *)(D2UnitStrc *, int32_t))MISSILE_RandomiseSpawnAppearance;
        tParams.pInitArgs = (int32_t)pMissile;
        MISSILE_ClientSpawn(&tParams);
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB30E80
//  pCltDoFunc 9: meteorcenter, vampiremeteorcenter, royalstrikemeteorcenter -- the target circle under a falling meteor, which is also the fall's
//  clock. From the row: F = CltParam1, R = CltParam2 and V = CltParam3, each floored at 1 (`jge`, `jl`, `jl`).
//    * The glow. The rate is the skill's Calc1 for the owner, clamped to 1..60; every 60 / rate frames, counting MISSILE_GetRemainingFrames + 1
//      (signed, `idiv`), the centre's light grows by one while its radius is below the rate.
//    * The meteor, when no frame remains: CltSubMissile1 at the centre, flags 1, its gfx offset (-V*F, 0, R*F) (PARTICLE_SetUnitGfxCoords) and its
//      gfx vector (V, 0, -R, scaled) (PARTICLE_SetUnitGfxVector); then CltSubMissile2 the same, if it is above 0 (a 16-bit `test / jle`).
//    * The sound, two frames before F: the first sound on the centre is detached, and the row's ProgSound played if it is above 0.
//  A centre with no record, a negative CltSubMissile1 or no skills row for its skill is destroyed (three exits, one effect).
//  The original inlines the Missiles.txt record lookup, DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall MISSILE_CltDoFunc9(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2SkillsTxt *pSkillsTxt;
    D2ClientLightStrc *pLight;
    D2UnitStrc *pOwner;
    D2UnitStrc *pSpawned;
    int32_t nMissileId;
    int32_t nSkillId;
    int32_t nLevel;
    int32_t nFrames;
    int32_t nRise;
    int32_t nVectorX;
    int32_t nGfxX;
    int32_t nGfxZ;
    int32_t nRate;
    int32_t nPeriod;
    int32_t nRadius;
    int32_t nLived;
    int32_t hSound;
    int16_t nSubMissileId;
    int16_t nSoundId;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltSubMissile[0] < 0)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    nFrames = 1;
    if ((int32_t)pMissilesTxt->dwCltParam[0] >= 1)
        nFrames = (int32_t)pMissilesTxt->dwCltParam[0];
    nRise = 1;
    if ((int32_t)pMissilesTxt->dwCltParam[1] >= 1)
        nRise = (int32_t)pMissilesTxt->dwCltParam[1];
    nGfxZ = nRise * nFrames;
    nVectorX = 1;
    if ((int32_t)pMissilesTxt->dwCltParam[2] >= 1)
        nVectorX = (int32_t)pMissilesTxt->dwCltParam[2];
    nGfxX = nVectorX * nFrames;

    nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    pOwner = CUNIT_GetOwnerUnit(pMissile);
    nRate = D2COMMON_SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxt->dwCalc[0], (D2C_Skills)nSkillId, nLevel);
    if (nRate <= 1)
        nRate = 1;
    else if (nRate >= 60)
        nRate = 60;
    nPeriod = 60 / nRate;

    pLight = pMissile->pLight;
    nRadius = pLight ? DLIGHTMAP_GetRadius(pLight) : 0;
    nLived = D2COMMON_MISSILE_GetRemainingFrames(pMissile);
    if (!((nLived + 1) % nPeriod) && nRadius < nRate && pLight)
        DLIGHTMAP_SetRadius(pLight, nRadius + 1);

    if (!nLived)
    {
        memset(&tParams, 0, sizeof(tParams));
        tParams.dwFlags = MISSILESPAWN_STARTPOS;
        tParams.pOwner = CUNIT_GetOwnerUnit(pMissile);

        tParams.nX = UNITS_GetXPosition(pMissile);

        tParams.nY = UNITS_GetYPosition(pMissile);

        tParams.nMissile = (int16_t)pMissilesTxt->wCltSubMissile[0];
        tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
        tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        pSpawned = MISSILE_ClientSpawn(&tParams);
        if (pSpawned)
        {
            PARTICLE_SetUnitGfxCoords(pSpawned, -nGfxX, 0, nGfxZ);
            PARTICLE_SetUnitGfxVector(pSpawned, nVectorX, 0, -nRise, 1);
        }

        nSubMissileId = (int16_t)pMissilesTxt->wCltSubMissile[1];
        if (nSubMissileId > 0)
        {
            tParams.nMissile = nSubMissileId;
            pSpawned = MISSILE_ClientSpawn(&tParams);
            if (pSpawned)
            {
                PARTICLE_SetUnitGfxCoords(pSpawned, -nGfxX, 0, nGfxZ);
                PARTICLE_SetUnitGfxVector(pSpawned, nVectorX, 0, -nRise, 1);
            }
        }
    }

    if (nLived == nFrames - 2)
    {
        hSound = UNITSND_GetFirstSoundOnUnit(pMissile);
        if (hSound)
            SOUNDINST_DetachFromUnit(hSound, pMissile, 1);
        nSoundId = (int16_t)pMissilesTxt->wProgSound;
        if (nSoundId > 0)
            SOUNDINST_PlaySound((D2C_Sounds)nSoundId, pMissile, 0, 0, 0);
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB31180
//  Drops one piece of an area, on the period. Callers: pCltDoFunc 10 (monblizcenter), 13 (blizzardcenter) and 24 (cairnstones). When
//  MISSILE_GetRemainingFrames is a multiple of nPeriod (a signed remainder, `cdq / idiv`) and the missile has an owner and a room:
//    * the missile's own seed is re-seeded from its X plus the frames it has lived (D2Common #10913, SEED_InitLowSeed), so every client that agrees
//      on where the centre is and how long it has lived drops the same piece on the same spot;
//    * two rolls below 2 * (nRadius - 1) on that seed give the offsets, (nRadius - 1) - roll on each axis: a square of side 2 * nRadius - 1 round the
//      missile, in the subtiles UNITS_GetX/YPosition answer;
//    * the spot must not be blocked -- D2Common's COLLISION_CheckMask with mask 4, a 16-bit answer (`test ax, ax`) -- and then nSubMissileId is
//      spawned there, flags 0x2003, the owner's, with the missile's skill and level.
//  Returns the spawned missile, or NULL: a period of 0 or less, a remainder, no owner, no room, a blocked spot. The owner's and the room's exits
//  return the eax they have just tested, which is NULL.
//  The original inlines UNITS_GetXPosition, UNITS_GetYPosition and SEED_RollLimitedRandomNumber here.
D2UnitStrc *__fastcall MISSILE_DropPeriodicSubMissile(D2UnitStrc *pMissile, int32_t nRadius, int32_t nPeriod, int32_t nSubMissileId)
{
    D2MissileStrc tParams;
    D2UnitStrc *pOwner;
    D2ActiveRoomStrc *pRoom;
    int32_t nSeedBaseX;
    int32_t nHalfSpread;
    int32_t nSpread;
    int32_t nOffX;
    int32_t nOffY;

    if (nPeriod <= 0)
        return NULL;
    if (D2COMMON_MISSILE_GetRemainingFrames(pMissile) % nPeriod)
        return NULL;

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
        return NULL;
    pRoom = D2COMMON_UNITS_GetRoom(pMissile);
    if (!pRoom)
        return NULL;

    nSeedBaseX = UNITS_GetXPosition(pMissile);
    D2COMMON_SEED_InitLowSeed(&pMissile->pSeed, nSeedBaseX + D2COMMON_MISSILE_GetRemainingFrames(pMissile));

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETOFFSET | MISSILESPAWN_RANDOMDIR;
    tParams.pOwner = pOwner;
    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
    tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    tParams.nMissile = nSubMissileId;

    nHalfSpread = nRadius - 1;
    nSpread = 2 * nHalfSpread;

    nOffX = nHalfSpread - SEED_RollLimitedRandomNumber(&pMissile->pSeed, nSpread);

    nOffY = nHalfSpread - SEED_RollLimitedRandomNumber(&pMissile->pSeed, nSpread);

    tParams.nX = nOffX + UNITS_GetXPosition(pMissile);

    tParams.nY = nOffY + UNITS_GetYPosition(pMissile);

    if (D2COMMON_COLLISION_CheckMask(pRoom, tParams.nX, tParams.nY, 4))
        return NULL;

    return MISSILE_ClientSpawn(&tParams);
}


//1.10f: D2Client.0x6FB31390
//  pCltDoFunc 10: monblizcenter, one row. Every period one of the pieces CltSubMissile1 .. CltSubMissile2 is dropped round the centre and let fall:
//    * the piece: CltSubMissile1, plus a roll below the range's width when CltSubMissile2 is above it -- D2Common's seed inline with the modulo arm
//      folded in and the power-of-two arm a call and `and`;
//    * the step: the missile's level over CltParam3 (floored at 1), a SIGNED division (`cdq / idiv`) of MISSILE_GetLevel's unsigned answer; the
//      radius is CltParam1 plus the step floored at 2, the period CltParam2 less the step UNFLOORED (`sub ecx, eax` takes the raw quotient), floored
//      at 3;
//    * MISSILE_DropPeriodicSubMissile drops it; a piece with no record is destroyed; otherwise its own row's CltParam2 (at least 1) is the fall per
//      frame and CltParam1 (at least that) the height: PARTICLE_SetUnitGfxCoords(piece, 0, 0, height), PARTICLE_SetUnitGfxVector(piece, 0, 0, -fall,
//      1), and its total and current frames both become height / fall (signed).
//  A centre with no record or a negative CltSubMissile1 is destroyed.
//  The original inlines the Missiles.txt record lookup here.
void __fastcall MISSILE_CltDoFunc10(D2UnitStrc *pMissile)
{
    D2MissilesTxt *pMissilesTxt;
    D2MissilesTxt *pSubMissilesTxt;
    D2UnitStrc *pSubMissile;
    int32_t nMissileId;
    int32_t nSubMissileId;
    int32_t nSubMissileMax;
    int32_t nLevel;
    int32_t nLevelsPerStep;
    int32_t nStep;
    int32_t nRadius;
    int32_t nPeriod;
    int32_t nFall;
    int32_t nHeight;
    int32_t nFrames;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltSubMissile[0] < 0)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    nSubMissileId = (int16_t)pMissilesTxt->wCltSubMissile[0];
    nSubMissileMax = (int16_t)pMissilesTxt->wCltSubMissile[1];
    if (nSubMissileId < nSubMissileMax)
    {
        nSubMissileId += SEED_RollLimitedRandomNumberInline(&pMissile->pSeed, nSubMissileMax - nSubMissileId + 1);
    }

    nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    nLevelsPerStep = (int32_t)pMissilesTxt->dwCltParam[2];
    if (nLevelsPerStep < 1)
        nLevelsPerStep = 1;
    nStep = nLevel / nLevelsPerStep;
    nRadius = (nStep >= 2 ? nStep : 2) + (int32_t)pMissilesTxt->dwCltParam[0];
    nPeriod = (int32_t)pMissilesTxt->dwCltParam[1] - nStep;
    if (nPeriod < 3)
        nPeriod = 3;

    pSubMissile = MISSILE_DropPeriodicSubMissile(pMissile, nRadius, nPeriod, nSubMissileId);
    if (pSubMissile)
    {
        pSubMissilesTxt = NULL;
        nMissileId = pSubMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pSubMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
        if (!pSubMissilesTxt)
        {
            MISSILE_DestroyClientMissile(pSubMissile);
            MISSILE_CltDoFunc1(pMissile);
            return;
        }

        nFall = (int32_t)pSubMissilesTxt->dwCltParam[1];
        if (nFall < 1)
            nFall = 1;
        nHeight = nFall;
        if (nFall <= (int32_t)pSubMissilesTxt->dwCltParam[0])
            nHeight = (int32_t)pSubMissilesTxt->dwCltParam[0];

        PARTICLE_SetUnitGfxCoords(pSubMissile, 0, 0, nHeight);
        PARTICLE_SetUnitGfxVector(pSubMissile, 0, 0, -nFall, 1);
        nFrames = nHeight / nFall;
        D2COMMON_MISSILE_SetTotalFrames(pSubMissile, nFrames);
        D2COMMON_MISSILE_SetCurrentFrame(pSubMissile, nFrames);
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB31540
//  pCltDoFunc 11: the corpse explosions, the Grim Ward stops, Andariel's debris explosions 3..5, the queen's two death splats and more. Once its
//  animation has run its cycle (D2Common's UNITS_IsAtEndOfFrameCycle, #10378) the missile is marked dead and its light map freed; until then the
//  common per-frame work. Both exits are tail jumps.
void __fastcall MISSILE_CltDoFunc11(D2UnitStrc *pMissile)
{
    if (D2COMMON_UNITS_IsAtEndOfFrameCycle(pMissile))
    {
        CUNIT_SetUnitDead(pMissile);
        CUNIT_GfxFreeUnitLightMap(pMissile);
    }
    else
    {
        MISSILE_CltDoFunc1(pMissile);
    }
}


//1.10f: D2Client.0x6FB31570
//  pCltDoFunc 12 -- no row of stock Missiles.txt carries it, so a stock game never runs it. A screen shake on the missile's first frame
//  (MISSILE_GetRemainingFrames 0, the frames lived): magnitude CltParam1, and CltParam2, T, cut in thirds -- T / 3 for the ramp up and again for the
//  ramp down, T - 2 * (T / 3) for the hold -- each times 25000 and over 25 (a thousand per unit; VIEWEFFECTS_StartScreenShake takes milliseconds
//  against GetTickCount, so the three add up to T seconds). Both columns must be above 0 or the missile is destroyed. All three divisions are signed.
//  The product is formed as the original forms it, 32 bits that wrap, and then divided; C's int multiplication would be undefined past 85899 in a
//  third, so it is written unsigned and cast back -- the same bits.
void __fastcall MISSILE_CltDoFunc12(D2UnitStrc *pMissile)
{
    D2MissilesTxt *pMissilesTxt;
    int32_t nMissileId;
    int32_t nThird;
    int32_t nRampMs;
    int32_t nHoldProduct;
    int32_t nHoldMs;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int32_t)pMissilesTxt->dwCltParam[0] <= 0 || (int32_t)pMissilesTxt->dwCltParam[1] <= 0)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    if (!D2COMMON_MISSILE_GetRemainingFrames(pMissile))
    {
        nThird = (int32_t)pMissilesTxt->dwCltParam[1] / 3;
        nRampMs = (int32_t)(25000u * (uint32_t)nThird) / 25;
        //  the hold's divide by 25 as the original computes it: MSVC folds the plain form into a multiply by 1000, wrong once the product wraps
        nHoldProduct = (int32_t)(25000u * (uint32_t)((int32_t)pMissilesTxt->dwCltParam[1] - 2 * nThird));
        nHoldMs = (int32_t)(((int64_t)nHoldProduct * 0x51EB851FLL) >> 35);
        nHoldMs += (int32_t)((uint32_t)nHoldMs >> 31);
        VIEWEFFECTS_StartScreenShake((int32_t)pMissilesTxt->dwCltParam[0], nRampMs, nHoldMs, nRampMs);
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB31660
//  pCltDoFunc 67 -- no row of stock Missiles.txt carries it. Three timed sub-missiles: for each i of 0, 1, 2, when CltSubMissile(i+1) is above 0 (a
//  16-bit signed `cmp / jle`) and MISSILE_GetRemainingFrames equals CltParam(i+1) exactly, that sub-missile is spawned at the missile's position for
//  its owner, with its skill and level. Then the common per-frame work, called, not jumped to. A missile with no record is destroyed. Each spawn is
//  MISSILE_ClientSpawnSimple written out (its first parameter is the OWNER); its arguments are computed first, in the copy's order, Y before X and
//  the level before the skill, so the calls into D2Common and CUNIT_GetOwnerUnit run in the original's order. The three blocks are one block unrolled
//  and are written out as the original has them.
//  The original inlines the Missiles.txt record lookup, UNITS_GetXPosition, UNITS_GetYPosition and MISSILE_ClientSpawnSimple here.
void __fastcall MISSILE_CltDoFunc67(D2UnitStrc *pMissile)
{
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pOwner;
    int32_t nMissileId;
    int32_t nX;
    int32_t nY;
    int32_t nLevel;
    int32_t nSkill;
    int32_t nSubMissileId;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    if ((int16_t)pMissilesTxt->wCltSubMissile[0] > 0 && D2COMMON_MISSILE_GetRemainingFrames(pMissile) == (int32_t)pMissilesTxt->dwCltParam[0])
    {
        nY = UNITS_GetYPosition(pMissile);

        nX = UNITS_GetXPosition(pMissile);

        nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
        nSubMissileId = (int16_t)pMissilesTxt->wCltSubMissile[0];
        pOwner = CUNIT_GetOwnerUnit(pMissile);

        MISSILE_ClientSpawnSimple(pOwner, nSubMissileId, nX, nY, nSkill, nLevel);
    }

    if ((int16_t)pMissilesTxt->wCltSubMissile[1] > 0 && D2COMMON_MISSILE_GetRemainingFrames(pMissile) == (int32_t)pMissilesTxt->dwCltParam[1])
    {
        nY = UNITS_GetYPosition(pMissile);

        nX = UNITS_GetXPosition(pMissile);

        nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
        nSubMissileId = (int16_t)pMissilesTxt->wCltSubMissile[1];
        pOwner = CUNIT_GetOwnerUnit(pMissile);

        MISSILE_ClientSpawnSimple(pOwner, nSubMissileId, nX, nY, nSkill, nLevel);
    }

    if ((int16_t)pMissilesTxt->wCltSubMissile[2] > 0 && D2COMMON_MISSILE_GetRemainingFrames(pMissile) == (int32_t)pMissilesTxt->dwCltParam[2])
    {
        nY = UNITS_GetYPosition(pMissile);

        nX = UNITS_GetXPosition(pMissile);

        nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
        nSubMissileId = (int16_t)pMissilesTxt->wCltSubMissile[2];
        pOwner = CUNIT_GetOwnerUnit(pMissile);

        MISSILE_ClientSpawnSimple(pOwner, nSubMissileId, nX, nY, nSkill, nLevel);
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB31990
//  pCltDoFunc 13: blizzardcenter, one row -- pCltDoFunc 10 with the radius and the period from the SKILL: SKILLS_EvaluateSkillFormula of the skills
//  row's Calc1 and Calc2 for the owner, at the missile's skill and level. The piece is chosen the same way, but here the whole roll is inlined, both
//  arms, where pCltDoFunc 10's calls SEED_RollRandomNumber in one. It gives up -- the centre is destroyed -- on no record, a negative CltSubMissile1,
//  no skills row, a period of 0 (tested first) or a radius not above 0; from there it is pCltDoFunc 10's drop and fall.
//  The original inlines the Missiles.txt record lookup, DATATBLS_GetSkillsTxtRecordById and SEED_RollLimitedRandomNumber here.
void __fastcall MISSILE_CltDoFunc13(D2UnitStrc *pMissile)
{
    D2MissilesTxt *pMissilesTxt;
    D2MissilesTxt *pSubMissilesTxt;
    D2SkillsTxt *pSkillsTxt;
    D2UnitStrc *pOwner;
    D2UnitStrc *pSubMissile;
    int32_t nMissileId;
    int32_t nSubMissileId;
    int32_t nSubMissileMax;
    int32_t nSkillId;
    int32_t nLevel;
    int32_t nRadius;
    int32_t nPeriod;
    int32_t nFall;
    int32_t nHeight;
    int32_t nFrames;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltSubMissile[0] < 0)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    nSubMissileId = (int16_t)pMissilesTxt->wCltSubMissile[0];
    nSubMissileMax = (int16_t)pMissilesTxt->wCltSubMissile[1];
    if (nSubMissileId < nSubMissileMax)
    {
        nSubMissileId += SEED_RollLimitedRandomNumber(&pMissile->pSeed, nSubMissileMax - nSubMissileId + 1);
    }

    nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    nRadius = D2COMMON_SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxt->dwCalc[0], (D2C_Skills)nSkillId, nLevel);
    nPeriod = D2COMMON_SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxt->dwCalc[1], (D2C_Skills)nSkillId, nLevel);
    if (!nPeriod || nRadius <= 0)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    pSubMissile = MISSILE_DropPeriodicSubMissile(pMissile, nRadius, nPeriod, nSubMissileId);
    if (pSubMissile)
    {
        pSubMissilesTxt = NULL;
        nMissileId = pSubMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pSubMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
        if (!pSubMissilesTxt)
        {
            MISSILE_DestroyClientMissile(pSubMissile);
            MISSILE_CltDoFunc1(pMissile);
            return;
        }

        nFall = (int32_t)pSubMissilesTxt->dwCltParam[1];
        if (nFall < 1)
            nFall = 1;
        nHeight = nFall;
        if (nFall <= (int32_t)pSubMissilesTxt->dwCltParam[0])
            nHeight = (int32_t)pSubMissilesTxt->dwCltParam[0];

        PARTICLE_SetUnitGfxCoords(pSubMissile, 0, 0, nHeight);
        PARTICLE_SetUnitGfxVector(pSubMissile, 0, 0, -nFall, 1);
        nFrames = nHeight / nFall;
        D2COMMON_MISSILE_SetTotalFrames(pSubMissile, nFrames);
        D2COMMON_MISSILE_SetCurrentFrame(pSubMissile, nFrames);
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB31BA0
//  pCltDoFunc 14 -- no row of stock Missiles.txt carries it. A homing step: the target is the missile's path target, or its owner's
//  (CUNIT_GetPathTargetUnit twice); every CltParam3 frames (5 when the column is not positive; a signed remainder of MISSILE_GetRemainingFrames, `cdq
//  / idiv`) and while the target is at most CltParam4 away, the path's target is set CltParam5 (at least 1) subtiles from the missile towards the
//  target on each axis -- the sign of the difference, 0 on an axis where they agree -- and the move restarted. No owner or no record: destroyed; no
//  target: the common per-frame work only. The step block has no out-of-line body: pCltDoFunc 15 carries the same block written out.
//  The original inlines the Missiles.txt record lookup, UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall MISSILE_CltDoFunc14(D2UnitStrc *pMissile)
{
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pOwner;
    D2UnitStrc *pTarget;
    int32_t nMissileId;
    int32_t nInterval;
    int32_t nMissileX;
    int32_t nMissileY;
    int32_t nTargetX;
    int32_t nTargetY;
    int32_t nStep;
    int32_t nStepX;
    int32_t nStepY;
    int32_t nX;
    int32_t nY;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    pTarget = CUNIT_GetPathTargetUnit(pMissile);
    if (!pTarget)
    {
        pTarget = CUNIT_GetPathTargetUnit(pOwner);
        if (!pTarget)
        {
            MISSILE_CltDoFunc1(pMissile);
            return;
        }
    }

    nInterval = (int32_t)pMissilesTxt->dwCltParam[2];
    if (nInterval <= 0)
        nInterval = 5;

    if (!(D2COMMON_MISSILE_GetRemainingFrames(pMissile) % nInterval) && D2COMMON_UNITS_GetDistanceToOtherUnit(pMissile, pTarget) <= (int32_t)pMissilesTxt->dwCltParam[3])
    {
        nMissileX = UNITS_GetXPosition(pMissile);

        nMissileY = UNITS_GetYPosition(pMissile);

        nTargetX = UNITS_GetXPosition(pTarget);

        nTargetY = UNITS_GetYPosition(pTarget);

        nStep = (int32_t)pMissilesTxt->dwCltParam[4];
        if (nStep < 1)
            nStep = 1;
        nStepX = 0;
        nStepY = 0;
        if (nTargetX > nMissileX)
            nStepX = nStep;
        else if (nTargetX < nMissileX)
            nStepX = -nStep;
        if (nTargetY > nMissileY)
            nStepY = nStep;
        else if (nTargetY < nMissileY)
            nStepY = -nStep;

        nY = UNITS_GetYPosition(pMissile);

        nX = UNITS_GetXPosition(pMissile);

        D2COMMON_10170_PathSetTargetPos(pMissile->pDynamicPath, nX + nStepX, nY + nStepY);
        D2COMMON_PATH_StartUnitMove(pMissile->pDynamicPath, pMissile, 0);
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB31DD0
//  pCltDoFunc 15: fingermagespider, one row -- a homing missile that leaves a trail of copies of itself. The missile's TargetX field (D2Common's
//  MISSILE_GetTargetX, #11141) serves as a mark: only a value above 0 -- the parent's -- steps and spawns; the pieces are spawned with 0
//  (MISSILE_SetTargetX) and, while it is 0 or less, it is stepped down by one each frame, the piece destroyed once it reaches -CltParam2 -- so a
//  piece lives CltParam2 frames. Then, while the path has a velocity:
//    * the step, for the parent only (mark above 0): pCltDoFunc 14's block, against the path's target unit or else the owner's, every CltParam3
//      frames of MISSILE_GetCurrentFrame, while the target is LESS than CltParam4 away (pCltDoFunc 14 allows equal);
//    * the trail, for the parent only: every CltParam1 frames (at least 1), when CltSubMissile1 is above 0 (a 16-bit `cmp / jle`), a piece at the
//      missile, flags 5, velocity 0, range the missile's total frames less 1, the owner's, with its skill and level.
//  No record, no owner, or a piece at the end of its life: destroyed; no velocity: the common per-frame work only.
//  The original inlines the Missiles.txt record lookup, UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall MISSILE_CltDoFunc15(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pOwner;
    D2UnitStrc *pTarget;
    D2UnitStrc *pSpawned;
    int32_t nMissileId;
    int32_t nMark;
    int32_t nInterval;
    int32_t nMissileX;
    int32_t nMissileY;
    int32_t nTargetX;
    int32_t nTargetY;
    int32_t nStep;
    int32_t nStepX;
    int32_t nStepY;
    int32_t nX;
    int32_t nY;
    int32_t nPeriod;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    //  The original's `neg eax` on the column is written as an unsigned negation cast back (the same bits); C's -(int) would be undefined at INT_MIN.
    nMark = D2COMMON_MISSILE_GetTargetX(pMissile);
    if (nMark <= (int32_t)(0u - pMissilesTxt->dwCltParam[1]))
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }
    if (nMark <= 0)
        D2COMMON_MISSILE_SetTargetX(pMissile, nMark - 1);

    if (!D2COMMON_PATH_GetVelocity(pMissile->pDynamicPath))
    {
        MISSILE_CltDoFunc1(pMissile);
        return;
    }

    pTarget = D2COMMON_PATH_GetTargetUnit(pMissile->pDynamicPath);
    if (!pTarget)
        pTarget = CUNIT_GetPathTargetUnit(pOwner);

    nInterval = (int32_t)pMissilesTxt->dwCltParam[2];
    if (nInterval <= 0)
        nInterval = 5;

    if (pTarget && !(D2COMMON_MISSILE_GetCurrentFrame(pMissile) % nInterval) && nMark > 0 && D2COMMON_UNITS_GetDistanceToOtherUnit(pMissile, pTarget) < (int32_t)pMissilesTxt->dwCltParam[3])
    {
        nMissileX = UNITS_GetXPosition(pMissile);

        nMissileY = UNITS_GetYPosition(pMissile);

        nTargetX = UNITS_GetXPosition(pTarget);

        nTargetY = UNITS_GetYPosition(pTarget);

        nStep = (int32_t)pMissilesTxt->dwCltParam[4];
        if (nStep < 1)
            nStep = 1;
        nStepX = 0;
        nStepY = 0;
        if (nTargetX > nMissileX)
            nStepX = nStep;
        else if (nTargetX < nMissileX)
            nStepX = -nStep;
        if (nTargetY > nMissileY)
            nStepY = nStep;
        else if (nTargetY < nMissileY)
            nStepY = -nStep;

        nY = UNITS_GetYPosition(pMissile);

        nX = UNITS_GetXPosition(pMissile);

        D2COMMON_10170_PathSetTargetPos(pMissile->pDynamicPath, nX + nStepX, nY + nStepY);
        D2COMMON_PATH_StartUnitMove(pMissile->pDynamicPath, pMissile, 0);
    }

    if ((int16_t)pMissilesTxt->wCltSubMissile[0] > 0)
    {
        nPeriod = (int32_t)pMissilesTxt->dwCltParam[0];
        if (nPeriod <= 1)
            nPeriod = 1;
        if (!(D2COMMON_MISSILE_GetCurrentFrame(pMissile) % nPeriod) && nMark > 0)
        {
            memset(&tParams, 0, sizeof(tParams));
            tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_VELOCITY;

            tParams.nX = UNITS_GetXPosition(pMissile);

            tParams.nY = UNITS_GetYPosition(pMissile);

            tParams.nRange = D2COMMON_MISSILE_GetTotalFrames(pMissile) - 1;
            tParams.nVelocity = 0;
            tParams.nMissile = (int16_t)pMissilesTxt->wCltSubMissile[0];
            tParams.pOwner = pOwner;
            tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
            tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
            pSpawned = MISSILE_ClientSpawn(&tParams);
            if (pSpawned)
                D2COMMON_MISSILE_SetTargetX(pSpawned, 0);
        }
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB32150
//  pCltDoFunc 16: diabwallmaker, one row -- Diablo's fire wall. In each frame the maker moved it drops CltSubMissile1 where it stands, target the
//  same spot, flags 0x21 -- 0x4021 three times in four, when the low two bits of a roll on its own seed are not both 0 -- for the owner, with its
//  skill and level. No record, a negative CltSubMissile1 or no owner: destroyed. The owner is asked for twice, once to test and once for the spawn
//  block. The roll's low bits are also stored to a frame slot nothing reads.
//  The original inlines the Missiles.txt record lookup, SEED_RollRandomNumber, UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall MISSILE_CltDoFunc16(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    int32_t nMissileId;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltSubMissile[0] < 0 || !CUNIT_GetOwnerUnit(pMissile))
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    if (D2COMMON_PATH_HasMovedThisFrame(pMissile->pDynamicPath))
    {
        memset(&tParams, 0, sizeof(tParams));
        tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETPOS;

        if (SEED_RollRandomNumber(&pMissile->pSeed) & 3)
            tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETPOS | MISSILESPAWN_NOLIGHT;

        tParams.pOwner = CUNIT_GetOwnerUnit(pMissile);

        tParams.nX = UNITS_GetXPosition(pMissile);

        tParams.nY = UNITS_GetYPosition(pMissile);

        tParams.nTargetX = UNITS_GetXPosition(pMissile);

        tParams.nTargetY = UNITS_GetYPosition(pMissile);

        tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
        tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        tParams.nMissile = (int16_t)pMissilesTxt->wCltSubMissile[0];
        MISSILE_ClientSpawn(&tParams);
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB32310
//  pCltDoFunc 17: cursecenter and mindblast center. The piece is the missile's stream missile (D2Common's MISSILE_GetStreamMissile), or
//  CltSubMissile1 when that is 0; the radius its stream range. While MISSILE_GetRemainingFrames is below CltParam1 and every third frame (`idiv` by
//  3), MISSILE_SpawnOverArea scatters pieces over the radius round the missile: one chance in the radius, or in half of it (signed, `cdq / sub /
//  sar`) above 4, stepping 2 above 4 and 1 at or below. A negative piece, a radius not above 0 or no record: destroyed. The piece's sign is tested
//  once, whichever value it came from.
//  The original inlines the Missiles.txt record lookup, UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall MISSILE_CltDoFunc17(D2UnitStrc *pMissile)
{
    D2MissilesTxt *pMissilesTxt;
    int32_t nMissileId;
    int32_t nSubMissileId;
    int32_t nRadius;
    int32_t nOneInChance;
    int32_t nLevel;
    int32_t nSkill;
    int32_t nX;
    int32_t nY;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    nSubMissileId = D2COMMON_MISSILE_GetStreamMissile(pMissile);
    if (!nSubMissileId)
        nSubMissileId = (int16_t)pMissilesTxt->wCltSubMissile[0];
    if (nSubMissileId < 0)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    nRadius = D2COMMON_MISSILE_GetStreamRange(pMissile);
    if (nRadius <= 0)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    if (D2COMMON_MISSILE_GetRemainingFrames(pMissile) < (int32_t)pMissilesTxt->dwCltParam[0] && !(D2COMMON_MISSILE_GetRemainingFrames(pMissile) % 3))
    {
        if (nRadius <= 4)
            nOneInChance = nRadius;
        else
            nOneInChance = nRadius / 2;
        nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        nSkill = D2COMMON_MISSILE_GetSkill(pMissile);

        nY = UNITS_GetYPosition(pMissile);

        nX = UNITS_GetXPosition(pMissile);

        MISSILE_SpawnOverArea(pMissile, nX, nY, nRadius, nSubMissileId, nSkill, nLevel, nOneInChance, (nRadius > 4) + 1);
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB32470
//  pCltDoFunc 18: bonespear and viper_bonespear -- the trail. Once MISSILE_GetRemainingFrames has reached InitSteps (the byte zero-extended, a signed
//  `jl`) and in a frame the spear moved, it spawns CltSubMissile1 at its position, flags 1, CltParam1 as the loop count UNFLOORED, for the owner,
//  with its skill and level; the piece then takes the spear's direction, precise position and gfx height (PARTICLE_GetUnitGfxCoordZ onto
//  PARTICLE_SetUnitGfxCoords(0, 0, z)).
//  Unlike its neighbours it destroys nothing: no record or a negative CltSubMissile1 gives the common per-frame work (MISSILE_CltDoFunc1), not
//  MISSILE_DestroyClientMissile.
//  The original inlines the Missiles.txt record lookup, UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall MISSILE_CltDoFunc18(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pSpawned;
    int32_t nMissileId;
    int32_t nX;
    int32_t nY;
    int32_t nPrecisionX;
    int32_t nPrecisionY;
    uint8_t nDirection;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }

    if (pMissilesTxt && (int16_t)pMissilesTxt->wCltSubMissile[0] >= 0 && D2COMMON_MISSILE_GetRemainingFrames(pMissile) >= pMissilesTxt->nInitSteps && D2COMMON_PATH_HasMovedThisFrame(pMissile->pDynamicPath))
    {
        nX = UNITS_GetXPosition(pMissile);

        nY = UNITS_GetYPosition(pMissile);

        memset(&tParams, 0, sizeof(tParams));
        tParams.dwFlags = MISSILESPAWN_STARTPOS;
        tParams.pOwner = CUNIT_GetOwnerUnit(pMissile);
        tParams.nX = nX;
        tParams.nY = nY;
        tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
        tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        tParams.nMissile = (int16_t)pMissilesTxt->wCltSubMissile[0];
        tParams.nLoops = (int32_t)pMissilesTxt->dwCltParam[0];
        pSpawned = MISSILE_ClientSpawn(&tParams);
        if (pSpawned)
        {
            nDirection = D2COMMON_PATH_GetDirection(pMissile->pDynamicPath);
            D2COMMON_PATH_SetDirection(pSpawned->pDynamicPath, nDirection);
            nPrecisionX = D2COMMON_UNITS_GetPrecisionX(pMissile);
            D2COMMON_PATH_SetPrecisionX(pSpawned->pDynamicPath, nPrecisionX);
            nPrecisionY = D2COMMON_UNITS_GetPrecisionY(pMissile);
            D2COMMON_PATH_SetPrecisionY(pSpawned->pDynamicPath, nPrecisionY);
            PARTICLE_SetUnitGfxCoords(pSpawned, 0, 0, PARTICLE_GetUnitGfxCoordZ(pMissile));
        }
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB32610
//  pCltDoFunc 19: frozenorb, one row -- the orb's spray of bolts. When the frames lived leave no positive remainder by CltParam1 (at least 1; `test /
//  jle`, so a negative remainder spawns too), a bolt, CltSubMissile1, flags 2, origin the orb, for the owner, with its skill and level, is aimed at
//  an offset on a circle of radius 30: gCircle30OffsetX/Y indexed by the orb's TargetX modulo 64 -- a signed remainder made positive. TargetX then
//  advances by CltParam2, modulo 64 (signed, not made positive), so the spray turns 19/64 of a circle a bolt. No record or a negative CltSubMissile1:
//  destroyed. The owner is not tested.
//  The advance is written as an unsigned sum cast back: the original's 32-bit wrap without C's signed overflow.
//  The original inlines the Missiles.txt record lookup here.
void __fastcall MISSILE_CltDoFunc19(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pOwner;
    int32_t nMissileId;
    int32_t nPeriod;
    int32_t nAngle;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltSubMissile[0] < 0)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    nPeriod = 1;
    if ((int32_t)pMissilesTxt->dwCltParam[0] >= 1)
        nPeriod = (int32_t)pMissilesTxt->dwCltParam[0];

    if (D2COMMON_MISSILE_GetRemainingFrames(pMissile) % nPeriod > 0)
    {
        MISSILE_CltDoFunc1(pMissile);
        return;
    }

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_TARGETOFFSET;
    tParams.pOwner = pOwner;
    tParams.pOrigin = pMissile;
    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
    tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    tParams.nMissile = (int16_t)pMissilesTxt->wCltSubMissile[0];

    nAngle = D2COMMON_MISSILE_GetTargetX(pMissile) % 64;
    if (nAngle < 0)
        nAngle = -nAngle;
    tParams.nTargetX = gCircle30OffsetX[nAngle];
    tParams.nTargetY = gCircle30OffsetY[nAngle];
    D2COMMON_MISSILE_SetTargetX(pMissile, (int32_t)(pMissilesTxt->dwCltParam[1] + (uint32_t)nAngle) % 64);

    MISSILE_ClientSpawn(&tParams);
    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB32750
//  pCltDoFunc 20: frozenorbnova, one row. While the frames lived are below CltParam1 and on every CltParam2-th of them (at least 1; one read of the
//  count for both tests), the missile's (TargetX, TargetY) turns an eighth of a circle, ((X - Y) / 2, (X + Y) / 2) -- both halvings signed -- the
//  path's target is set that far from the missile and the move restarted, and the turned pair is stored back. No record: destroyed. The sum and
//  difference are written unsigned and cast back: the original's 32-bit wrap without C's signed overflow.
//  The original inlines the Missiles.txt record lookup, UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall MISSILE_CltDoFunc20(D2UnitStrc *pMissile)
{
    D2MissilesTxt *pMissilesTxt;
    int32_t nMissileId;
    int32_t nPeriod;
    int32_t nLived;
    int32_t nTargetX;
    int32_t nTargetY;
    int32_t nTurnX;
    int32_t nTurnY;
    int32_t nX;
    int32_t nY;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    nPeriod = 1;
    if ((int32_t)pMissilesTxt->dwCltParam[1] >= 1)
        nPeriod = (int32_t)pMissilesTxt->dwCltParam[1];

    nLived = D2COMMON_MISSILE_GetRemainingFrames(pMissile);
    if (nLived < (int32_t)pMissilesTxt->dwCltParam[0] && !(nLived % nPeriod))
    {
        nTargetX = D2COMMON_MISSILE_GetTargetX(pMissile);
        nTargetY = D2COMMON_MISSILE_GetTargetY(pMissile);
        nTurnX = (int32_t)((uint32_t)nTargetX - (uint32_t)nTargetY) / 2;
        nTurnY = (int32_t)((uint32_t)nTargetX + (uint32_t)nTargetY) / 2;

        nY = UNITS_GetYPosition(pMissile);

        nX = UNITS_GetXPosition(pMissile);

        D2COMMON_10170_PathSetTargetPos(pMissile->pDynamicPath, nX + nTurnX, nY + nTurnY);
        D2COMMON_PATH_StartUnitMove(pMissile->pDynamicPath, pMissile, 0);
        D2COMMON_MISSILE_SetTargetX(pMissile, nTurnX);
        D2COMMON_MISSILE_SetTargetY(pMissile, nTurnY);
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB32870
//  pCltDoFunc 21: brdeathcontrol, one row -- the bolts of a death sequence (its sub-missile is brdeathlightningbolt, pCltDoFunc 22). The missile's
//  TargetX is a countdown here: while it is not 0 it is stepped down by one. At 0 a bolt, CltSubMissile1, flags 2, owner and origin both the owner,
//  with the missile's skill and level, is aimed at an offset of up to CltParam1 on each axis -- two rolls below 2 * CltParam1 + 1 on the missile's
//  seed, less CltParam1; (0, 0) is replaced by (1 + a roll below CltParam1, 0) -- and the offset is copied into the bolt's own TargetX/Y, read back
//  out of the spawn block after the spawn. The countdown restarts at 24, 9, 14 or 2 (25, 10, 15 or 3 less one) by the frames lived: below 125, below
//  175, 300 or more, else. No record or a negative CltSubMissile1: destroyed; no owner: the common per-frame work only.
//  The rolls' ranges and offsets are written unsigned and cast back.
//  The original inlines the Missiles.txt record lookup and SEED_RollLimitedRandomNumber here.
void __fastcall MISSILE_CltDoFunc21(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pOwner;
    D2UnitStrc *pSpawned;
    int32_t nMissileId;
    int32_t nTimer;
    int32_t nLived;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltSubMissile[0] < 0)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
    {
        MISSILE_CltDoFunc1(pMissile);
        return;
    }

    nTimer = D2COMMON_MISSILE_GetTargetX(pMissile);
    if (!nTimer)
    {
        memset(&tParams, 0, sizeof(tParams));
        tParams.nMissile = (int16_t)pMissilesTxt->wCltSubMissile[0];
        tParams.dwFlags = MISSILESPAWN_TARGETOFFSET;
        tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
        tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        tParams.pOwner = pOwner;
        tParams.pOrigin = pOwner;

        tParams.nTargetX = (int32_t)(SEED_RollLimitedRandomNumber(&pMissile->pSeed, (int32_t)(2 * pMissilesTxt->dwCltParam[0] + 1)) - pMissilesTxt->dwCltParam[0]);

        tParams.nTargetY = (int32_t)(SEED_RollLimitedRandomNumber(&pMissile->pSeed, (int32_t)(2 * pMissilesTxt->dwCltParam[0] + 1)) - pMissilesTxt->dwCltParam[0]);

        if (!tParams.nTargetX && !tParams.nTargetY)
        {
            tParams.nTargetX = SEED_RollLimitedRandomNumber(&pMissile->pSeed, (int32_t)pMissilesTxt->dwCltParam[0]) + 1;
        }

        pSpawned = MISSILE_ClientSpawn(&tParams);
        if (pSpawned)
        {
            D2COMMON_MISSILE_SetTargetX(pSpawned, tParams.nTargetX);
            D2COMMON_MISSILE_SetTargetY(pSpawned, tParams.nTargetY);
        }

        nLived = D2COMMON_MISSILE_GetRemainingFrames(pMissile);
        if (nLived < 125)
            nTimer = 25;
        else if (nLived < 175)
            nTimer = 10;
        else
            nTimer = nLived >= 300 ? 15 : 3;
    }

    D2COMMON_MISSILE_SetTargetX(pMissile, nTimer - 1);
    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB32AC0
//  pCltDoFunc 22: brdeathlightningbolt, one row -- pCltDoFunc 21's bolt. In each frame it moved and when CltSubMissile1 is 0 or more, it leaves a
//  piece: flags 0, owner the owner, origin the bolt, no position in the block, with MISSILE_RandomiseSpawnAppearance as its on-spawn hook and the
//  bolt as the hook's argument, then lifted to the bolt's gfx height. Every CltParam1 frames (at least 1) it turns: one bit of a roll on its own seed
//  picks (X - Y, X + Y) or (Y + X, Y - X) from its (TargetX, TargetY), three quarters of each (signed): each branch is a rotation by an eighth of a
//  circle scaled by sqrt(2). A nonzero step moves the path's target that far from the bolt and restarts the move, and the step is stored back as the
//  new TargetX/Y. A negative CltSubMissile1 means no trail at all. No record: destroyed; no owner: the common work only. The turn is written in
//  unsigned arithmetic cast back: the original's 32-bit wrap without C's signed overflow.
//  The original inlines the Missiles.txt record lookup, SEED_RollRandomNumber, UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall MISSILE_CltDoFunc22(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pOwner;
    D2UnitStrc *pSpawned;
    int32_t nMissileId;
    int32_t nPeriod;
    int32_t nTargetX;
    int32_t nTargetY;
    int32_t nTurnX;
    int32_t nTurnY;
    int32_t nStepX;
    int32_t nStepY;
    int32_t nX;
    int32_t nY;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
    {
        MISSILE_CltDoFunc1(pMissile);
        return;
    }

    if ((int16_t)pMissilesTxt->wCltSubMissile[0] >= 0 && D2COMMON_PATH_HasMovedThisFrame(pMissile->pDynamicPath))
    {
        memset(&tParams, 0, sizeof(tParams));
        tParams.pOwner = pOwner;
        tParams.pOrigin = pMissile;
        tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
        tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        tParams.nMissile = (int16_t)pMissilesTxt->wCltSubMissile[0];
        tParams.pInitFunc = (void (__fastcall *)(D2UnitStrc *, int32_t))MISSILE_RandomiseSpawnAppearance;
        tParams.pInitArgs = (int32_t)pMissile;
        pSpawned = MISSILE_ClientSpawn(&tParams);
        if (pSpawned)
            PARTICLE_SetUnitGfxCoords(pSpawned, 0, 0, PARTICLE_GetUnitGfxCoordZ(pMissile));
    }

    nPeriod = (int32_t)pMissilesTxt->dwCltParam[0];
    if (nPeriod < 1)
        nPeriod = 1;

    if (!(D2COMMON_MISSILE_GetRemainingFrames(pMissile) % nPeriod))
    {
        nTargetX = D2COMMON_MISSILE_GetTargetX(pMissile);
        nTargetY = D2COMMON_MISSILE_GetTargetY(pMissile);

        if (SEED_RollRandomNumber(&pMissile->pSeed) & 1)
        {
            nTurnX = (int32_t)((uint32_t)nTargetX - (uint32_t)nTargetY);
            nTurnY = (int32_t)((uint32_t)nTargetX + (uint32_t)nTargetY);
        }
        else
        {
            nTurnX = (int32_t)((uint32_t)nTargetY + (uint32_t)nTargetX);
            nTurnY = (int32_t)((uint32_t)nTargetY - (uint32_t)nTargetX);
        }
        nStepX = (int32_t)(3u * (uint32_t)nTurnX) / 4;
        nStepY = (int32_t)(3u * (uint32_t)nTurnY) / 4;

        if (nStepX || nStepY)
        {
            nY = UNITS_GetYPosition(pMissile);

            nX = UNITS_GetXPosition(pMissile);

            D2COMMON_10170_PathSetTargetPos(pMissile->pDynamicPath, nX + nStepX, nY + nStepY);
            D2COMMON_PATH_StartUnitMove(pMissile->pDynamicPath, pMissile, 0);
        }

        D2COMMON_MISSILE_SetTargetX(pMissile, nStepX);
        D2COMMON_MISSILE_SetTargetY(pMissile, nStepY);
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB32CB0
//  pCltDoFunc 23: denofevillight, one row -- a light that never runs out: once its current frame is below 100 (a signed `jge` past it) it is set back
//  to 500, then the common per-frame work, tail-jumped to.
void __fastcall MISSILE_CltDoFunc23(D2UnitStrc *pMissile)
{
    if (D2COMMON_MISSILE_GetCurrentFrame(pMissile) < 100)
        D2COMMON_MISSILE_SetCurrentFrame(pMissile, 500);

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB32CE0
//  pCltDoFunc 24: cairnstones, one row -- the Cairn Stones opening. Two windows of the current frame F, D being CltParam1:
//    * D < F < Range - D (the column read as a signed 16-bit word): MISSILE_DropPeriodicSubMissile drops CltSubMissile1 when it is above 0, radius
//      CltParam3, period CltParam2; a piece with no record is destroyed, and one whose row's CltParam2 is above 0 is lifted to that row's CltParam1
//      and let fall at CltParam2 a frame, for CltParam1 / CltParam2 frames (signed);
//    * D < F < D + 125: the units of the missile's ROOM are walked, by pRoomNext, and each object whose class id is 17..21 -- OBJECT_STONEALPHA to
//      OBJECT_STONELAMBDA, an UNSIGNED range (`jb`, `jnb`) -- gets, when CltSubMissile1 is above 0, that piece spawned on it and let fall the same
//      way; and every eighth frame (signed), when CltSubMissile2 is above 0, a bolt from it, flags 0x421, to the stone whose unit id the missile
//      keeps in TargetX -- the id is then replaced by this stone's, always when there was no such stone and one time in four when there was (a call
//      to SEED_RollRandomNumber on the OTHER stone's seed).
//  The room is not tested: D2Common's UNITS_GetRoom answer is read at once. The walk's own test at its top can never fail, the loop being entered and
//  repeated only on a unit. The spawn on a stone is MISSILE_ClientSpawnSimple written out and its answer tested, and the fall after it is the first
//  window's block again; that block has no out-of-line body and is written out twice. No record: destroyed.
//  The original inlines the Missiles.txt record lookup, UNITS_GetXPosition, UNITS_GetYPosition and MISSILE_ClientSpawnSimple here.
void __fastcall MISSILE_CltDoFunc24(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2MissilesTxt *pPieceTxt;
    D2UnitStrc *pOwner;
    D2UnitStrc *pPiece;
    D2UnitStrc *pStone;
    D2UnitStrc *pOther;
    uint32_t dwOtherGUID;
    int32_t nMissileId;
    int32_t nFrame;
    int32_t nDelay;
    int32_t nFrames;
    int32_t nX;
    int32_t nY;
    int32_t nLevel;
    int32_t nSkill;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    nFrame = D2COMMON_MISSILE_GetCurrentFrame(pMissile);
    pOwner = CUNIT_GetOwnerUnit(pMissile);
    nDelay = (int32_t)pMissilesTxt->dwCltParam[0];

    if (nFrame < (int16_t)pMissilesTxt->wRange - nDelay && nFrame > nDelay && (int16_t)pMissilesTxt->wCltSubMissile[0] > 0)
    {
        pPiece = MISSILE_DropPeriodicSubMissile(pMissile, (int32_t)pMissilesTxt->dwCltParam[2], (int32_t)pMissilesTxt->dwCltParam[1], (int16_t)pMissilesTxt->wCltSubMissile[0]);
        if (pPiece)
        {
            pPieceTxt = NULL;
            nMissileId = pPiece->dwClassId;
            if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
                pPieceTxt = &sgptDataTables->pMissilesTxt[nMissileId];
            if (!pPieceTxt)
            {
                MISSILE_DestroyClientMissile(pPiece);
            }
            else if ((int32_t)pPieceTxt->dwCltParam[1] > 0)
            {
                PARTICLE_SetUnitGfxCoords(pPiece, 0, 0, (int32_t)pPieceTxt->dwCltParam[0]);
                PARTICLE_SetUnitGfxVector(pPiece, 0, 0, -(int32_t)pPieceTxt->dwCltParam[1], 1);
                nFrames = (int32_t)pPieceTxt->dwCltParam[0] / (int32_t)pPieceTxt->dwCltParam[1];
                D2COMMON_MISSILE_SetTotalFrames(pPiece, nFrames);
                D2COMMON_MISSILE_SetCurrentFrame(pPiece, nFrames);
            }
        }
    }

    if (nFrame < nDelay + 125 && nFrame > nDelay)
    {
        for (pStone = D2COMMON_UNITS_GetRoom(pMissile)->pUnitFirst; pStone; pStone = pStone->pRoomNext)
        {
            if (pStone->dwUnitType != UNIT_OBJECT || (uint32_t)pStone->dwClassId < OBJECT_STONEALPHA || (uint32_t)pStone->dwClassId >= OBJECT_STONETHETA)
            {
                continue;
            }

            if ((int16_t)pMissilesTxt->wCltSubMissile[0] > 0)
            {
                nY = UNITS_GetYPosition(pStone);

                nX = UNITS_GetXPosition(pStone);

                nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
                nSkill = D2COMMON_MISSILE_GetSkill(pMissile);

                pPiece = MISSILE_ClientSpawnSimple(pOwner, (int16_t)pMissilesTxt->wCltSubMissile[0], nX, nY, nSkill, nLevel);
                if (pPiece)
                {
                    pPieceTxt = NULL;
                    nMissileId = pPiece->dwClassId;
                    if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
                        pPieceTxt = &sgptDataTables->pMissilesTxt[nMissileId];
                    if (!pPieceTxt)
                    {
                        MISSILE_DestroyClientMissile(pPiece);
                    }
                    else if ((int32_t)pPieceTxt->dwCltParam[1] > 0)
                    {
                        PARTICLE_SetUnitGfxCoords(pPiece, 0, 0, (int32_t)pPieceTxt->dwCltParam[0]);
                        PARTICLE_SetUnitGfxVector(pPiece, 0, 0, -(int32_t)pPieceTxt->dwCltParam[1], 1);
                        nFrames = (int32_t)pPieceTxt->dwCltParam[0] / (int32_t)pPieceTxt->dwCltParam[1];
                        D2COMMON_MISSILE_SetTotalFrames(pPiece, nFrames);
                        D2COMMON_MISSILE_SetCurrentFrame(pPiece, nFrames);
                    }
                }
            }

            if ((int16_t)pMissilesTxt->wCltSubMissile[1] > 0 && !(nFrame % 8))
            {
                dwOtherGUID = (uint32_t)D2COMMON_MISSILE_GetTargetX(pMissile);
                pOther = NULL;
                if (dwOtherGUID)
                    pOther = CUNIT_FindUnitByGuidAndType(dwOtherGUID, UNIT_OBJECT);

                if (!pOther)
                {
                    D2COMMON_MISSILE_SetTargetX(pMissile, (int32_t)pStone->dwUnitId);
                }
                else
                {
                    memset(&tParams, 0, sizeof(tParams));
                    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETPOS | MISSILESPAWN_FRAMESTOTARGET;
                    tParams.pOwner = pOwner;

                    tParams.nX = UNITS_GetXPosition(pStone);

                    tParams.nY = UNITS_GetYPosition(pStone);

                    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
                    tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
                    tParams.nMissile = (int16_t)pMissilesTxt->wCltSubMissile[1];

                    tParams.nTargetX = UNITS_GetXPosition(pOther);

                    tParams.nTargetY = UNITS_GetYPosition(pOther);

                    MISSILE_ClientSpawn(&tParams);
                    if (!(SEED_RollRandomNumber(&pOther->pSeed) & 3))
                        D2COMMON_MISSILE_SetTargetX(pMissile, (int32_t)pStone->dwUnitId);
                }
            }
        }
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB33180
//  pCltDoFunc 25: towermist and orbmist -- a drifting mist trail. Every CltParam3 frames (at least 1) of the remaining ones it spawns CltSubMissile1,
//  flags 0x21, for the owner, with its skill and level, at the missile's position jittered by up to CltParam2 on each axis, aimed at that spot plus a
//  slant of up to CltParam1 on each axis. Four rolls on the missile's seed, in this order: the slant's X and Y (below 2 * CltParam1 + 1, less
//  CltParam1), the spot's X, then its Y (the same over CltParam2) -- the first three D2Common's seed inline with the power-of-two arm a call, the
//  fourth the whole roll inlined. No record or a negative CltSubMissile1: destroyed.
//  The rolls' ranges and offsets are written in unsigned arithmetic cast back: the original's 32-bit wrap without C's signed overflow.
//  The original inlines the Missiles.txt record lookup, UNITS_GetXPosition, UNITS_GetYPosition and SEED_RollLimitedRandomNumber here.
void __fastcall MISSILE_CltDoFunc25(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    int32_t nMissileId;
    int32_t nPeriod;
    int32_t nAimX;
    int32_t nAimY;
    int32_t nSpawnX;
    int32_t nSpawnY;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltSubMissile[0] < 0)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    nPeriod = 1;
    if ((int32_t)pMissilesTxt->dwCltParam[2] >= 1)
        nPeriod = (int32_t)pMissilesTxt->dwCltParam[2];
    if (D2COMMON_MISSILE_GetRemainingFrames(pMissile) % nPeriod)
    {
        MISSILE_CltDoFunc1(pMissile);
        return;
    }

    nAimX = (int32_t)(SEED_RollLimitedRandomNumberInline(&pMissile->pSeed, (int32_t)(2 * pMissilesTxt->dwCltParam[0] + 1)) - pMissilesTxt->dwCltParam[0]);
    nAimY = (int32_t)(SEED_RollLimitedRandomNumberInline(&pMissile->pSeed, (int32_t)(2 * pMissilesTxt->dwCltParam[0] + 1)) - pMissilesTxt->dwCltParam[0]);

    nSpawnX = UNITS_GetXPosition(pMissile);
    nSpawnX += (int32_t)(SEED_RollLimitedRandomNumberInline(&pMissile->pSeed, (int32_t)(2 * pMissilesTxt->dwCltParam[1] + 1)) - pMissilesTxt->dwCltParam[1]);

    nSpawnY = UNITS_GetYPosition(pMissile);

    nSpawnY += (int32_t)(SEED_RollLimitedRandomNumber(&pMissile->pSeed, (int32_t)(2 * pMissilesTxt->dwCltParam[1] + 1)) - pMissilesTxt->dwCltParam[1]);

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETPOS;
    tParams.pOwner = CUNIT_GetOwnerUnit(pMissile);
    tParams.nX = nSpawnX;
    tParams.nY = nSpawnY;
    tParams.nTargetX = nSpawnX + nAimX;
    tParams.nTargetY = nSpawnY + nAimY;
    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
    tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    tParams.nMissile = (int16_t)pMissilesTxt->wCltSubMissile[0];
    MISSILE_ClientSpawn(&tParams);

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB33440
//  pCltDoFunc 26: mephistodeathcontrol, one row -- Mephisto's death. Two countdowns, in TargetX and TargetY, each stepped down by one per frame and
//  restarted when it is 0 or less:
//    * TargetX, when CltSubMissile1 is above 0: MISSILE_CltSpawnScatteredSubMissile(owner, missile, CltSubMissile1, CltParam3) -- the fire walls --
//      and a restart at CltParam1;
//    * TargetY, when CltSubMissile2 is above 0: a flying rock, flags 0x102, owner the owner, origin the missile, its skill with a LEVEL of 1..5 (a
//      roll modulo 5 plus 1), aimed at an offset of up to CltParam3 on each axis -- (0, 0) replaced by (1 + a roll below CltParam3 - 1, 0) -- the
//      rock's own TargetX a roll modulo 5, and a restart at CltParam2.
//  Every roll is on the missile's seed: the two modulo-5 rolls are SEED_RollRandomNumber inlined with an unsigned `div` by 5, the three offset rolls
//  the whole limited roll inlined. No record: destroyed; no owner: the common per-frame work only. The rolls' ranges and offsets are written in
//  unsigned arithmetic cast back.
//  The original inlines the Missiles.txt record lookup, SEED_RollRandomNumber and SEED_RollLimitedRandomNumber here.
void __fastcall MISSILE_CltDoFunc26(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pOwner;
    D2UnitStrc *pSpawned;
    int32_t nMissileId;
    int32_t nSpread;
    int32_t nRange;
    int32_t nTimer;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
    {
        MISSILE_CltDoFunc1(pMissile);
        return;
    }

    nSpread = (int32_t)pMissilesTxt->dwCltParam[2];

    if ((int16_t)pMissilesTxt->wCltSubMissile[0] > 0)
    {
        nTimer = D2COMMON_MISSILE_GetTargetX(pMissile);
        if (nTimer <= 0)
        {
            MISSILE_CltSpawnScatteredSubMissile(pOwner, pMissile, (int16_t)pMissilesTxt->wCltSubMissile[0], nSpread);
            nTimer = (int32_t)pMissilesTxt->dwCltParam[0];
        }
        D2COMMON_MISSILE_SetTargetX(pMissile, nTimer - 1);
    }

    if ((int16_t)pMissilesTxt->wCltSubMissile[1] > 0)
    {
        nTimer = D2COMMON_MISSILE_GetTargetY(pMissile);
        if (nTimer <= 0)
        {
            memset(&tParams, 0, sizeof(tParams));
            tParams.dwFlags = MISSILESPAWN_TARGETOFFSET | MISSILESPAWN_ARC;
            tParams.pOwner = pOwner;
            tParams.pOrigin = pMissile;
            tParams.nMissile = (int16_t)pMissilesTxt->wCltSubMissile[1];
            tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);

            tParams.nSkillLevel = (int32_t)((uint32_t)SEED_RollRandomNumber(&pMissile->pSeed) % 5 + 1);

            nRange = (int32_t)(2 * (uint32_t)nSpread + 1);

            tParams.nTargetX = (int32_t)(SEED_RollLimitedRandomNumber(&pMissile->pSeed, nRange) - (uint32_t)nSpread);

            tParams.nTargetY = (int32_t)(SEED_RollLimitedRandomNumber(&pMissile->pSeed, nRange) - (uint32_t)nSpread);

            if (!tParams.nTargetX && !tParams.nTargetY)
            {
                tParams.nTargetX = SEED_RollLimitedRandomNumber(&pMissile->pSeed, (int32_t)((uint32_t)nSpread - 1)) + 1;
            }

            pSpawned = MISSILE_ClientSpawn(&tParams);
            if (pSpawned)
            {
                D2COMMON_MISSILE_SetTargetX(pSpawned, (int32_t)((uint32_t)SEED_RollRandomNumber(&pMissile->pSeed) % 5));
            }

            nTimer = (int32_t)pMissilesTxt->dwCltParam[1];
        }
        D2COMMON_MISSILE_SetTargetY(pMissile, nTimer - 1);
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB33680
//  One piece at a scattered target. Callers: pCltDoFunc 26 (Mephisto's fire walls) and 29. A spawn block, flags 2, the owner, the parent as origin,
//  the parent's skill and level, aimed at an offset of up to nSpread on each axis: two rolls below 2 * nSpread + 1 on the PARENT's seed, less
//  nSpread; (0, 0) is replaced by (1 + a roll below nSpread - 1, 0). The spawned piece gets the offset as its own TargetX/Y, read back out of the
//  block after the spawn. Returns the piece, or NULL. The ranges and offsets are written in unsigned arithmetic cast back.
//  The original inlines SEED_RollLimitedRandomNumber for the three rolls here.
D2UnitStrc *__fastcall MISSILE_CltSpawnScatteredSubMissile(D2UnitStrc *pOwner, D2UnitStrc *pMissile, int32_t nSubMissileId, int32_t nSpread)
{
    D2MissileStrc tParams;
    D2UnitStrc *pSpawned;
    int32_t nRange;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_TARGETOFFSET;
    tParams.nMissile = nSubMissileId;
    tParams.pOwner = pOwner;
    tParams.pOrigin = pMissile;
    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
    tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);

    nRange = (int32_t)(2 * (uint32_t)nSpread + 1);

    tParams.nTargetX = (int32_t)(SEED_RollLimitedRandomNumber(&pMissile->pSeed, nRange) - (uint32_t)nSpread);

    tParams.nTargetY = (int32_t)(SEED_RollLimitedRandomNumber(&pMissile->pSeed, nRange) - (uint32_t)nSpread);

    if (!tParams.nTargetX && !tParams.nTargetY)
    {
        tParams.nTargetX = SEED_RollLimitedRandomNumber(&pMissile->pSeed, (int32_t)((uint32_t)nSpread - 1)) + 1;
    }

    pSpawned = MISSILE_ClientSpawn(&tParams);
    if (pSpawned)
    {
        D2COMMON_MISSILE_SetTargetX(pSpawned, tParams.nTargetX);
        D2COMMON_MISSILE_SetTargetY(pSpawned, tParams.nTargetY);
    }

    return pSpawned;
}


//1.10f: D2Client.0x6FB337E0
//  pCltDoFunc 27: mephistofirewallmaker and andyfirewallmaker -- pCltDoFunc 22's zig-zag with a four-way roll. In each frame it moved and when
//  CltSubMissile1 is above 0 it leaves a piece: flags 0, the owner, origin the maker, its skill and level -- no hook and no height, unlike 22. Every
//  CltParam1 frames (at least 1) two bits of a roll on its own seed pick the step: 0 turns (X - Y, X + Y), 2 turns (Y + X, Y - X), both scaled by
//  three quarters (signed), and 1 or 3 keep (TargetX, TargetY) whole; the step moves the path's target and restarts the move ALWAYS (22 skips a zero
//  step) and is stored back as the new TargetX/Y. No record: destroyed; no owner: the common per-frame work only.
//  The turns are written in unsigned arithmetic cast back.
//  The original inlines the Missiles.txt record lookup, SEED_RollRandomNumber, UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall MISSILE_CltDoFunc27(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pOwner;
    int32_t nMissileId;
    int32_t nPeriod;
    int32_t nTargetX;
    int32_t nTargetY;
    int32_t nStepX;
    int32_t nStepY;
    int32_t nX;
    int32_t nY;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
    {
        MISSILE_CltDoFunc1(pMissile);
        return;
    }

    if ((int16_t)pMissilesTxt->wCltSubMissile[0] > 0 && D2COMMON_PATH_HasMovedThisFrame(pMissile->pDynamicPath))
    {
        memset(&tParams, 0, sizeof(tParams));
        tParams.pOwner = pOwner;
        tParams.pOrigin = pMissile;
        tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
        tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        tParams.nMissile = (int16_t)pMissilesTxt->wCltSubMissile[0];
        MISSILE_ClientSpawn(&tParams);
    }

    nPeriod = (int32_t)pMissilesTxt->dwCltParam[0];
    if (nPeriod < 1)
        nPeriod = 1;

    if (!(D2COMMON_MISSILE_GetRemainingFrames(pMissile) % nPeriod))
    {
        nTargetX = D2COMMON_MISSILE_GetTargetX(pMissile);
        nTargetY = D2COMMON_MISSILE_GetTargetY(pMissile);

        switch ((int32_t)(SEED_RollRandomNumber(&pMissile->pSeed) & 3))
        {
        case 0:
            nStepX = (int32_t)(3u * ((uint32_t)nTargetX - (uint32_t)nTargetY)) / 4;
            nStepY = (int32_t)(3u * ((uint32_t)nTargetX + (uint32_t)nTargetY)) / 4;
            break;
        case 2:
            nStepX = (int32_t)(3u * ((uint32_t)nTargetY + (uint32_t)nTargetX)) / 4;
            nStepY = (int32_t)(3u * ((uint32_t)nTargetY - (uint32_t)nTargetX)) / 4;
            break;
        default:
            nStepX = nTargetX;
            nStepY = nTargetY;
            break;
        }

        nY = UNITS_GetYPosition(pMissile);

        nX = UNITS_GetXPosition(pMissile);

        D2COMMON_10170_PathSetTargetPos(pMissile->pDynamicPath, nX + nStepX, nY + nStepY);
        D2COMMON_PATH_StartUnitMove(pMissile->pDynamicPath, pMissile, 0);
        D2COMMON_MISSILE_SetTargetX(pMissile, nStepX);
        D2COMMON_MISSILE_SetTargetY(pMissile, nStepY);
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB339B0
//  pCltDoFunc 28: mephistodonotdraw, one row -- on the one frame whose remaining count equals CltParam1 exactly, four CltSubMissile1 (corpseexplosion
//  in the row), flags 0x2001, round the OWNER: its position plus gnMephistoDir4OffsetX/Y[i] for i of 0..3 -- the loop steps a byte offset by 4 to
//  0x10. No record or a negative CltSubMissile1: destroyed; no owner or any other frame: the common per-frame work only.
//  The original inlines the Missiles.txt record lookup, UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall MISSILE_CltDoFunc28(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pOwner;
    int32_t nMissileId;
    int32_t nX;
    int32_t nY;
    int32_t i;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltSubMissile[0] < 0)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner || D2COMMON_MISSILE_GetRemainingFrames(pMissile) != (int32_t)pMissilesTxt->dwCltParam[0])
    {
        MISSILE_CltDoFunc1(pMissile);
        return;
    }

    nX = UNITS_GetXPosition(pOwner);

    nY = UNITS_GetYPosition(pOwner);

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_RANDOMDIR;
    tParams.pOwner = pOwner;
    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
    tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    tParams.nMissile = (int16_t)pMissilesTxt->wCltSubMissile[0];
    for (i = 0; i < 4; ++i)
    {
        tParams.nX = nX + gnMephistoDir4OffsetX[i];
        tParams.nY = nY + gnMephistoDir4OffsetY[i];
        MISSILE_ClientSpawn(&tParams);
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB33B30
//  A scattered burst, for MISSILE_StepAndarielCloud only. nMissileId's row is looked up (nothing happens without one); then min(7 * nDensity, 8)
//  times a spawn block, flags 0, the owner, origin the missile, its skill and level, is spawned, and a spawned piece gets three rolls on the
//  MISSILE's seed: an offset of [0, 7 * nDensity) less half of it on each axis (a signed halving) and a Z speed below the PIECE's row's CltParam2.
//  Then PARTICLE_AllocUnitParticles, its gfx offset (x, y, CltParam1), its vector (0, 0, speed, 1) and its second vector (0, 0, CltParam3, 1) -- all
//  three columns the piece's own.
//  The original inlines the Missiles.txt record lookup and SEED_RollLimitedRandomNumber here.
void __fastcall MISSILE_SpawnScatteredSubMissiles(D2UnitStrc *pOwner, D2UnitStrc *pMissile, int32_t nMissileId, int32_t nDensity)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pSubMissilesTxt;
    D2UnitStrc *pSpawned;
    int32_t nSpread;
    int32_t nCount;
    int32_t nHalf;
    int32_t nX;
    int32_t nY;
    int32_t nVectorZ;

    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return;
    pSubMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    if (!pSubMissilesTxt)
        return;

    nSpread = (int32_t)(7u * (uint32_t)nDensity);
    nCount = nSpread > 8 ? 8 : nSpread;
    if (nCount <= 0)
        return;

    do
    {
        memset(&tParams, 0, sizeof(tParams));
        tParams.pOrigin = pMissile;
        tParams.pOwner = pOwner;
        tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
        tParams.nMissile = nMissileId;
        tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        pSpawned = MISSILE_ClientSpawn(&tParams);
        if (pSpawned)
        {
            nX = SEED_RollLimitedRandomNumber(&pMissile->pSeed, nSpread);
            nHalf = nSpread / 2;
            nX -= nHalf;

            nY = SEED_RollLimitedRandomNumber(&pMissile->pSeed, nSpread) - nHalf;

            nVectorZ = SEED_RollLimitedRandomNumber(&pMissile->pSeed, (int32_t)pSubMissilesTxt->dwCltParam[1]);

            PARTICLE_AllocUnitParticles(pSpawned);
            PARTICLE_SetUnitGfxCoords(pSpawned, nX, nY, (int32_t)pSubMissilesTxt->dwCltParam[0]);
            PARTICLE_SetUnitGfxVector(pSpawned, 0, 0, nVectorZ, 1);
            PARTICLE_SetUnitGfxVector2(pSpawned, 0, 0, (int32_t)pSubMissilesTxt->dwCltParam[2], 1);
        }
    }
    while (--nCount);
}


//1.10f: D2Client.0x6FB33D50
//  One frame of Andariel's death cloud, for pCltDoFunc 29 only. nMissileId's row is looked up (nothing without one); its CltSubMissile1, when above
//  0, goes to MISSILE_SpawnScatteredSubMissiles with nStage as the density; its ProgSound is played on the OWNER on frame 315 exactly. Then only on
//  even frames, and below stage 3 only every sixteenth (signed remainders): nMissileId itself is spawned, flags 1, for the owner, with the missile's
//  skill and level, at the missile's position plus an offset -- none at density 1, else two rolls below the density less half of it, the density 1
//  below stage 3, 2 below 6, 3 below 9, else 4. Each offset roll is SEED_RollLimitedRandomNumber inlined whole, with its own `nMax > 0` head inside
//  the density test.
//  The original inlines the Missiles.txt record lookup, SEED_RollLimitedRandomNumber, UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall MISSILE_StepAndarielCloud(D2UnitStrc *pOwner, D2UnitStrc *pMissile, int32_t nMissileId, int32_t nFrame, int32_t nStage)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    int32_t nDensity;
    int32_t nHalf;
    int32_t nOffX;
    int32_t nOffY;
    int16_t nSoundId;

    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return;
    pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    if (!pMissilesTxt)
        return;

    if ((int16_t)pMissilesTxt->wCltSubMissile[0] > 0)
        MISSILE_SpawnScatteredSubMissiles(pOwner, pMissile, (int16_t)pMissilesTxt->wCltSubMissile[0], nStage);

    nSoundId = (int16_t)pMissilesTxt->wProgSound;
    if (nSoundId > 0 && nFrame == 315)
        SOUNDINST_PlaySound((D2C_Sounds)nSoundId, pOwner, 0, 0, 0);

    if (nFrame % 2)
        return;
    if (nStage < 3 && nFrame % 16)
        return;

    if (nStage < 3)
        nDensity = 1;
    else if (nStage < 6)
        nDensity = 2;
    else if (nStage < 9)
        nDensity = 3;
    else
        nDensity = 4;

    nOffX = 0;
    nOffY = 0;
    if (nDensity > 1)
    {
        nOffX = SEED_RollLimitedRandomNumber(&pMissile->pSeed, nDensity);
        nHalf = nDensity / 2;
        nOffX -= nHalf;

        nOffY = SEED_RollLimitedRandomNumber(&pMissile->pSeed, nDensity) - nHalf;
    }

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS;
    tParams.pOwner = pOwner;
    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
    tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    tParams.nMissile = nMissileId;

    tParams.nX = UNITS_GetXPosition(pMissile) + nOffX;

    tParams.nY = UNITS_GetYPosition(pMissile) + nOffY;

    MISSILE_ClientSpawn(&tParams);
}


//1.10f: D2Client.0x6FB33F90
//  One piece of falling debris, for pCltDoFunc 29 only, which hands it its CltSubMissile3 (andyfallingdebris1 in the one stock row). The id's row is
//  looked up and the missile must stand in a room (UNITS_GetRoom), or nothing happens. When the id is below its row's CltSubMissile1 the piece is
//  picked from the span between the two, both ends in -- the id plus a roll below CltSubMissile1 - id + 1 -- and the picked id's row is looked up in
//  turn. A spawn block, flags 1, the owner, the picked id, the missile's skill and level, is then tried at most FOUR times: two rolls below 2 *
//  nRadius + 1, less nRadius, give an offset kept only inside the disc, dx*dx + dy*dy <= nRadius*nRadius (a signed `jg`); the point is the missile's
//  position plus the offset; the room that holds it is searched from the missile's, and the point must be free of collision mask 1 under pattern 0
//  (COLLISION_CheckAnyCollisionWithPattern). A spawn that fails is retried too. The first piece spawned gets its particles, a height of the picked
//  row's CltParam1 and a Z speed of minus its CltParam2 -- both doubled when nRadius is 6 -- and ends the call. An attempt spent outside the disc, on
//  no room, on a blocked point or on a refused spawn is not given back: four failures spawn nothing.
//  The squares and nRadius * nRadius are `imul`, 32 bits that wrap, written unsigned and cast back.
//  The original inlines the Missiles.txt record lookup, UNITS_GetXPosition, UNITS_GetYPosition and CLIENT_FindRoomContainingSubtile here.
void __fastcall MISSILE_SpawnRandomSubMissile(D2UnitStrc *pOwner, D2UnitStrc *pMissile, int32_t nMissileId, int32_t nRadius)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2ActiveRoomStrc *pRoom;
    D2UnitStrc *pSpawned;
    int32_t nLastId;
    int32_t nRange;
    int32_t nRadiusSq;
    int32_t nAttempt;
    int32_t nOffX;
    int32_t nOffY;
    int32_t nX;
    int32_t nY;

    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return;
    pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    if (!pMissilesTxt)
        return;

    if (!D2COMMON_UNITS_GetRoom(pMissile))
        return;

    nLastId = (int16_t)pMissilesTxt->wCltSubMissile[0];
    if (nMissileId < nLastId)
    {
        //  D2Common's seed inline: `jg` on the span, both arms a call to SEED_RollRandomNumber (the `div` and the `and`).
        nMissileId += SEED_RollLimitedRandomNumberInline(&pMissile->pSeed, nLastId - nMissileId + 1);

        if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
            return;
        pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
        if (!pMissilesTxt)
            return;
    }

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS;
    tParams.pOwner = pOwner;
    tParams.nMissile = nMissileId;
    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
    tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);

    nRadiusSq = (int32_t)((uint32_t)nRadius * (uint32_t)nRadius);
    nRange = (int32_t)(2 * (uint32_t)nRadius + 1);
    nAttempt = 0;
    do
    {
        ++nAttempt;

        //  D2Common's seed inline, twice: `jg` on the range, the modulo arm folded in, the power-of-two arm a call and `and`.
        nOffX = (int32_t)(SEED_RollLimitedRandomNumberInline(&pMissile->pSeed, nRange) - (uint32_t)nRadius);
        nOffY = (int32_t)(SEED_RollLimitedRandomNumberInline(&pMissile->pSeed, nRange) - (uint32_t)nRadius);
        if ((int32_t)((uint32_t)nOffX * (uint32_t)nOffX + (uint32_t)nOffY * (uint32_t)nOffY) > nRadiusSq)
            continue;

        nX = UNITS_GetXPosition(pMissile) + nOffX;

        nY = UNITS_GetYPosition(pMissile) + nOffY;

        pRoom = CLIENT_FindRoomContainingSubtile(D2COMMON_UNITS_GetRoom(pMissile), nX, nY);
        if (!pRoom || D2COMMON_COLLISION_CheckAnyCollisionWithPattern(pRoom, nX, nY, 0, 1))
            continue;

        tParams.nX = nX;
        tParams.nY = nY;
        pSpawned = MISSILE_ClientSpawn(&tParams);
        if (!pSpawned)
            continue;

        PARTICLE_AllocUnitParticles(pSpawned);
        if (nRadius == 6)
        {
            PARTICLE_SetUnitGfxCoords(pSpawned, 0, 0, (int32_t)(2 * pMissilesTxt->dwCltParam[0]));
            PARTICLE_SetUnitGfxVector(pSpawned, 0, 0, (int32_t)(2 * (0 - pMissilesTxt->dwCltParam[1])), 1);
        }
        else
        {
            PARTICLE_SetUnitGfxCoords(pSpawned, 0, 0, (int32_t)pMissilesTxt->dwCltParam[0]);
            PARTICLE_SetUnitGfxVector(pSpawned, 0, 0, (int32_t)(0 - pMissilesTxt->dwCltParam[1]), 1);
        }
        return;
    }
    while (nAttempt < 4);
}


//1.10f: D2Client.0x6FB34300
//  pCltDoFunc 29: andycontrol0, one row -- Andariel's death, run from one missile. F is MISSILE_GetRemainingFrames, and it counts the frames the
//  missile has LIVED, not those it has left: D2Common answers the total less the current, MISSILE_ClientSpawn sets both to the range and
//  MISSILE_CltDoFunc1 takes one from the current each frame. So F is 0 on the first call and rises toward the row's Range. With an owner, every
//  frame:
//    * the cloud, while F is above 10 and CltSubMissile1 above 0: MISSILE_StepAndarielCloud with andycolumnfirebase (310) and a stage of 1 below 30;
//      (F - 15) / 4 below 85, from 3 at 30 to 17 at 84; 12 below Range - 50; 12 - (F - (Range - 50)) / 4 below Range; and 1 from there. Every
//      division is signed and wRange sign-extended;
//    * the fire walls: every CltParam1 frames (at least 1, a signed remainder), when CltSubMissile2 is above 0, one andyfirewallmaker at a scattered
//      target, MISSILE_CltSpawnScatteredSubMissile with a spread of 20;
//    * the shake, on F == 90 exactly when CltParam2 is above 0: VIEWEFFECTS_StartScreenShake of magnitude 8 with CltParam2, T, cut in thirds as
//      pCltDoFunc 12 cuts it (T / 3 of ramp up and again of ramp down, T - 2 * (T / 3) of hold, each times 25000 and over 25), in milliseconds;
//    * the debris, when CltSubMissile3 is above 0 and 115 < F < 315: MISSILE_SpawnRandomSubMissile twice with radius 6 while 200 < F < 225, else once
//      with radius 20 on every fourth frame (a signed remainder).
//  Then the common per-frame work. No record: destroyed; no owner: the common per-frame work only.
//  The cloud step spawns CltSubMissile1 (andycolumnfirebase, 310) itself and hands that row's CltSubMissile1 (andycolumnfire, 311) to the scattered
//  burst, whose pieces read their own row.
//  The original inlines the Missiles.txt record lookup here.
void __fastcall MISSILE_CltDoFunc29(D2UnitStrc *pMissile)
{
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pOwner;
    int32_t nMissileId;
    int32_t nFrame;
    int32_t nStage;
    int32_t nRange;
    int32_t nPeriod;
    int32_t nThird;
    int32_t nRampMs;
    int32_t nHoldProduct;
    int32_t nHoldMs;
    int32_t nDebrisId;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
    {
        MISSILE_CltDoFunc1(pMissile);
        return;
    }

    nFrame = D2COMMON_MISSILE_GetRemainingFrames(pMissile);

    if ((int16_t)pMissilesTxt->wCltSubMissile[0] > 0 && nFrame > 10)
    {
        if (nFrame < 30)
        {
            nStage = 1;
        }
        else if (nFrame < 85)
        {
            nStage = (nFrame - 15) / 4;
        }
        else
        {
            nRange = (int16_t)pMissilesTxt->wRange;
            if (nFrame < nRange - 50)
                nStage = 12;
            else if (nFrame < nRange)
                nStage = 12 - (nFrame - (nRange - 50)) / 4;
            else
                nStage = 1;
        }
        MISSILE_StepAndarielCloud(pOwner, pMissile, (int16_t)pMissilesTxt->wCltSubMissile[0], nFrame, nStage);
    }

    nPeriod = 1;
    if ((int32_t)pMissilesTxt->dwCltParam[0] >= 1)
        nPeriod = (int32_t)pMissilesTxt->dwCltParam[0];
    if ((int16_t)pMissilesTxt->wCltSubMissile[1] > 0 && !(nFrame % nPeriod))
        MISSILE_CltSpawnScatteredSubMissile(pOwner, pMissile, (int16_t)pMissilesTxt->wCltSubMissile[1], 20);

    if ((int32_t)pMissilesTxt->dwCltParam[1] > 0 && nFrame == 90)
    {
        nThird = (int32_t)pMissilesTxt->dwCltParam[1] / 3;
        nRampMs = (int32_t)(25000u * (uint32_t)nThird) / 25;
        //  the hold's divide by 25 as the original computes it: MSVC folds the plain form into a multiply by 1000, wrong once the product wraps
        nHoldProduct = (int32_t)(25000u * (uint32_t)((int32_t)pMissilesTxt->dwCltParam[1] - 2 * nThird));
        nHoldMs = (int32_t)(((int64_t)nHoldProduct * 0x51EB851FLL) >> 35);
        nHoldMs += (int32_t)((uint32_t)nHoldMs >> 31);
        VIEWEFFECTS_StartScreenShake(8, nRampMs, nHoldMs, nRampMs);
    }

    nDebrisId = (int16_t)pMissilesTxt->wCltSubMissile[2];
    if (nDebrisId > 0 && nFrame > 115 && nFrame < 315)
    {
        if (nFrame > 200 && nFrame < 225)
        {
            MISSILE_SpawnRandomSubMissile(pOwner, pMissile, nDebrisId, 6);
            MISSILE_SpawnRandomSubMissile(pOwner, pMissile, nDebrisId, 6);
        }
        else if (!(nFrame % 4))
        {
            MISSILE_SpawnRandomSubMissile(pOwner, pMissile, nDebrisId, 20);
        }
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB34500
//  pCltDoFunc 30: towerchestspawner, one row (CltSubMissile1 towermisttrail). When F -- MISSILE_GetRemainingFrames, the frames lived (see pCltDoFunc
//  29) -- equals CltParam1, the missile's TargetY is set to 1, a flag; from then on, while MISSILE_GetTargetY is nonzero, every CltParam2 frames (at
//  least 1; a signed remainder) a CltSubMissile1, flags 0x21, for the owner, with the missile's skill and level, from the missile's position toward
//  it plus an offset of up to CltParam3 on each axis -- two rolls below 2 * CltParam3 + 1 on the missile's seed, less CltParam3, both taken before
//  the block is cleared. Then the common per-frame work. No record or a negative CltSubMissile1: destroyed.
//  The original inlines the Missiles.txt record lookup, UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall MISSILE_CltDoFunc30(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    int32_t nMissileId;
    int32_t nFrame;
    int32_t nPeriod;
    int32_t nSpread;
    int32_t nRange;
    int32_t nAimX;
    int32_t nAimY;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltSubMissile[0] < 0)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    nFrame = D2COMMON_MISSILE_GetRemainingFrames(pMissile);
    if (nFrame == (int32_t)pMissilesTxt->dwCltParam[0])
        D2COMMON_MISSILE_SetTargetY(pMissile, 1);

    if (D2COMMON_MISSILE_GetTargetY(pMissile))
    {
        nPeriod = 1;
        if ((int32_t)pMissilesTxt->dwCltParam[1] >= 1)
            nPeriod = (int32_t)pMissilesTxt->dwCltParam[1];
        if (!(nFrame % nPeriod))
        {
            nSpread = (int32_t)pMissilesTxt->dwCltParam[2];
            nRange = (int32_t)(2 * (uint32_t)nSpread + 1);

            //  D2Common's seed inline, twice: `jg` on the range, the modulo arm folded in, the power-of-two arm a call and `and`.
            nAimX = (int32_t)(SEED_RollLimitedRandomNumberInline(&pMissile->pSeed, nRange) - (uint32_t)nSpread);
            nAimY = (int32_t)(SEED_RollLimitedRandomNumberInline(&pMissile->pSeed, nRange) - (uint32_t)nSpread);

            memset(&tParams, 0, sizeof(tParams));
            tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETPOS;
            tParams.pOwner = CUNIT_GetOwnerUnit(pMissile);

            tParams.nX = UNITS_GetXPosition(pMissile);

            tParams.nY = UNITS_GetYPosition(pMissile);

            tParams.nTargetX = UNITS_GetXPosition(pMissile) + nAimX;

            tParams.nTargetY = UNITS_GetYPosition(pMissile) + nAimY;

            tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
            tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
            tParams.nMissile = (int16_t)pMissilesTxt->wCltSubMissile[0];
            MISSILE_ClientSpawn(&tParams);
        }
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB34760
//  pCltDoFunc 43: six rows, blade creeper (Blade Sentinel's srvmissilea) and the five catapult balls "on" (catapultchargedball on, which
//  monstats.txt's catapult1..4 fire as MissA1, and the spike, cold, plague and meteor balls) -- a missile that RIDES its owner. No owner: destroyed.
//  An owner that is a player or a monster (signed compares) and dead: the missile hits, MISSILE_ClientOnHit with no target and not forced. Otherwise
//  D2Common #10235, PATH_UpdateRiderPath(missile, owner), and UNITS_UpdateFrame.
//  It never calls the common per-frame work: MISSILE_CltDoFunc1 is what takes a frame off a missile's count, so the client never runs these down.
void __fastcall MISSILE_CltDoFunc43(D2UnitStrc *pMissile)
{
    D2UnitStrc *pOwner;

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    if ((int32_t)pOwner->dwUnitType >= (int32_t)UNIT_PLAYER && (int32_t)pOwner->dwUnitType <= (int32_t)UNIT_MONSTER && CUNIT_IsUnitDead(pOwner))
    {
        MISSILE_ClientOnHit(pMissile, NULL, 0);
        return;
    }

    D2COMMON_10235_PATH_UpdateRiderPath(pMissile, pOwner);
    D2COMMON_UNITS_UpdateFrame(pMissile);
}


//1.10f: D2Client.0x6FB347B0
//  pCltDoFunc 44: distraction, one row (CltSubMissile1 distraction fog) -- a missile that rides its owner while the owner keeps using the missile's
//  skill. Every frame:
//    * no record, a negative CltSubMissile1, no owner, an owner using no skill, or one whose used skill (SKILLS_GetSkillIdFromSkill) is not the
//      missile's: the missile leaves the client-id table, CUNIT_RemoveUnitByClientId(its id, its type) -- the removal MISSILE_DestroyClientMissile
//      ends with, without the travel sound and the light that function lets go of first;
//    * when its path moved this frame (D2Common #10234), a CltSubMissile1, flags 0x20, origin the missile, target (0, 0) -- two explicit stores over
//      the cleared block -- with its skill and level, for the missile itself if it is a player or a monster and else for its owner, asked again: a
//      missile is neither, so the first arm never runs here;
//    * then PATH_UpdateRiderPath(missile, owner) and the common per-frame work.
//  A NULL missile takes the same removal with (-1, 6).
//  The original inlines the Missiles.txt record lookup here.
void __fastcall MISSILE_CltDoFunc44(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pMount;
    D2UnitStrc *pSpawnOwner;
    D2SkillStrc *pSkill;
    BOOL bRiding;
    int32_t nMissileId;
    int32_t nSkillLevel;
    int32_t nSkillId;
    int32_t nSubMissileId;

    if (!pMissile)
    {
        //  BUG: a NULL missile removes unit type 6, one row past the six of the client-id table. UNREACHABLE: nothing hands a handler NULL.
        //  CUNIT_RemoveUnitByClientId does not reproduce the read of that missing row.
        CUNIT_RemoveUnitByClientId(-1, UNIT_TYPES_COUNT);
        return;
    }

    pMissilesTxt = NULL;
    nMissileId = pMissile->dwClassId;
    if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
        pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];

    bRiding = FALSE;
    pMount = NULL;
    if (pMissilesTxt && (int16_t)pMissilesTxt->wCltSubMissile[0] >= 0)
    {
        pMount = CUNIT_GetOwnerUnit(pMissile);
        if (pMount)
        {
            pSkill = D2COMMON_UNITS_GetUsedSkill(pMount);
            if (pSkill)
            {
                nSkillId = D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szMissileCpp, 2285);
                bRiding = nSkillId == D2COMMON_MISSILE_GetSkill(pMissile);
            }
        }
    }
    if (!bRiding)
    {
        CUNIT_RemoveUnitByClientId((int32_t)pMissile->dwUnitId, pMissile->dwUnitType);
        return;
    }

    if (D2COMMON_PATH_HasMovedThisFrame(pMissile->pDynamicPath))
    {
        nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
        nSubMissileId = (int16_t)pMissilesTxt->wCltSubMissile[0];
        if (pMissile->dwUnitType == UNIT_PLAYER || pMissile->dwUnitType == UNIT_MONSTER)
            pSpawnOwner = pMissile;
        else
            pSpawnOwner = CUNIT_GetOwnerUnit(pMissile);
        if (pSpawnOwner)
        {
            memset(&tParams, 0, sizeof(tParams));
            tParams.pOwner = pSpawnOwner;
            tParams.dwFlags = MISSILESPAWN_TARGETPOS;
            tParams.pOrigin = pMissile;
            tParams.nTargetX = 0;
            tParams.nTargetY = 0;
            tParams.nSkill = nSkillId;
            tParams.nSkillLevel = nSkillLevel;
            tParams.nMissile = nSubMissileId;
            MISSILE_ClientSpawn(&tParams);
        }
    }

    D2COMMON_10235_PATH_UpdateRiderPath(pMissile, pMount);
    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB348F0
//  pCltDoFunc 45: distraction fog, one row (CltSubMissile1 distraction puff) -- MISSILE_EmitCloudPuffs with the three columns CltParam1..3 as the
//  period, the count and the spread and CltSubMissile1 as the puff, then the common per-frame work (a tail jump). pCltDoFunc 4 makes the same call
//  with no sign test and destroys nothing; here no record or a negative CltSubMissile1 (`test cx, cx / jl`): destroyed.
//  The original inlines the Missiles.txt record lookup here.
void __fastcall MISSILE_CltDoFunc45(D2UnitStrc *pMissile)
{
    D2MissilesTxt *pMissilesTxt;
    int32_t nMissileId;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltSubMissile[0] < 0)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    MISSILE_EmitCloudPuffs(pMissile, (int32_t)pMissilesTxt->dwCltParam[0], (int32_t)pMissilesTxt->dwCltParam[1], (int32_t)pMissilesTxt->dwCltParam[2], (int16_t)pMissilesTxt->wCltSubMissile[0]);
    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB34960
//  pCltDoFunc 46: lightingtrailingjavalin and advlighttrailingjav (CltSubMissile1 their trails) -- a pair of side trails. On the missile's first two
//  frames -- F, MISSILE_GetRemainingFrames, counts the frames lived, and the test is F < 2 (`cmp eax, 2 / jge`) -- the vector from the missile to its
//  path's first point, turned a quarter, is kept as TargetX = -(firstY - y) and TargetY = firstX - x. On every frame its path moved, two
//  CltSubMissile1 pieces, flags 0x0B, for the owner, at the missile, with its skill and level and CltParam1 as their loops, aimed at (TargetX,
//  TargetY) and then at its negation -- one to each flank. Then the common per-frame work.
//  No record, a negative CltSubMissile1 or no path: the missile leaves the client-id table (CUNIT_RemoveUnitByClientId, as pCltDoFunc 44's removal);
//  a NULL missile takes it with (-1, 6), one row past the table, as 44's does.
//  The original inlines the Missiles.txt record lookup, UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall MISSILE_CltDoFunc46(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    int32_t nMissileId;
    int32_t nX;
    int32_t nY;
    int32_t nFirstX;

    if (!pMissile)
    {
        //  BUG: a NULL missile removes unit type 6, one row past the six of the client-id table. UNREACHABLE: nothing hands a handler NULL.
        //  CUNIT_RemoveUnitByClientId does not reproduce the read of that missing row.
        CUNIT_RemoveUnitByClientId(-1, UNIT_TYPES_COUNT);
        return;
    }

    pMissilesTxt = NULL;
    nMissileId = pMissile->dwClassId;
    if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
        pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltSubMissile[0] < 0 || !pMissile->pDynamicPath)
    {
        CUNIT_RemoveUnitByClientId((int32_t)pMissile->dwUnitId, pMissile->dwUnitType);
        return;
    }

    nX = UNITS_GetXPosition(pMissile);

    nY = UNITS_GetYPosition(pMissile);

    if (D2COMMON_MISSILE_GetRemainingFrames(pMissile) < 2)
    {
        nFirstX = D2COMMON_10175_PathGetFirstPointX(pMissile->pDynamicPath);
        D2COMMON_MISSILE_SetTargetX(pMissile, -(D2COMMON_10176_PathGetFirstPointY(pMissile->pDynamicPath) - nY));
        D2COMMON_MISSILE_SetTargetY(pMissile, nFirstX - nX);
    }

    if (D2COMMON_PATH_HasMovedThisFrame(pMissile->pDynamicPath))
    {
        memset(&tParams, 0, sizeof(tParams));
        tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETOFFSET | MISSILESPAWN_SUBLOOPS;
        tParams.pOwner = CUNIT_GetOwnerUnit(pMissile);
        tParams.nX = nX;
        tParams.nY = nY;
        tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
        tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        tParams.nMissile = (int16_t)pMissilesTxt->wCltSubMissile[0];
        tParams.nLoops = (int32_t)pMissilesTxt->dwCltParam[0];
        tParams.nTargetX = D2COMMON_MISSILE_GetTargetX(pMissile);
        tParams.nTargetY = D2COMMON_MISSILE_GetTargetY(pMissile);
        MISSILE_ClientSpawn(&tParams);

        //  `neg` written as an unsigned negation cast back.
        tParams.nTargetX = (int32_t)(0u - (uint32_t)D2COMMON_MISSILE_GetTargetX(pMissile));
        tParams.nTargetY = (int32_t)(0u - (uint32_t)D2COMMON_MISSILE_GetTargetY(pMissile));
        MISSILE_ClientSpawn(&tParams);
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB34B00
//  pCltDoFunc 68: sucfireball, one row -- on every frame its path moved, one CltSubMissile1, flags 9, for the owner, at the missile, with its skill
//  and level and CltParam1 as its loops. Then the common per-frame work. No record, or a CltSubMissile1 of 0 (`cmp word ptr [ebx+1Eh], 0 / jz`: a
//  zero test, where the handlers round it test the sign): destroyed.
//  The one row leaves CltSubMissile1 blank. Whether a blank cell passes a zero test, and what MISSILE_ClientSpawn makes of it, are NOT ESTABLISHED.
//  The original inlines the Missiles.txt record lookup, UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall MISSILE_CltDoFunc68(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    int32_t nMissileId;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || !pMissilesTxt->wCltSubMissile[0])
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    if (D2COMMON_PATH_HasMovedThisFrame(pMissile->pDynamicPath))
    {
        memset(&tParams, 0, sizeof(tParams));
        tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_SUBLOOPS;
        tParams.pOwner = CUNIT_GetOwnerUnit(pMissile);

        tParams.nX = UNITS_GetXPosition(pMissile);

        tParams.nY = UNITS_GetYPosition(pMissile);

        tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
        tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        tParams.nLoops = (int32_t)pMissilesTxt->dwCltParam[0];
        tParams.nMissile = (int16_t)pMissilesTxt->wCltSubMissile[0];
        MISSILE_ClientSpawn(&tParams);
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB34C30
//  pCltDoFunc 47: moltenboulder, one row (ProgSound sorceress_fireball_1) -- pCltDoFunc 6 with a sound. When ProgSound is above 0: the value
//  PARTICLE_GetUnitBouncesLeft answers is compared with the one kept in TargetX, the sound is played on the missile when the kept one is not 0 and
//  differs, and the new value is kept. Then pCltDoFunc 6 (a tail jump), the fire-wall maker's work. No record: destroyed.
//  The original inlines the Missiles.txt record lookup here.
void __fastcall MISSILE_CltDoFunc47(D2UnitStrc *pMissile)
{
    D2MissilesTxt *pMissilesTxt;
    int32_t nMissileId;
    int32_t nKept;
    int32_t nBounces;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    if ((int16_t)pMissilesTxt->wProgSound > 0)
    {
        nKept = D2COMMON_MISSILE_GetTargetX(pMissile);
        nBounces = PARTICLE_GetUnitBouncesLeft(pMissile);
        if (nKept && nKept != nBounces)
            SOUNDINST_PlaySound((D2C_Sounds)(int16_t)pMissilesTxt->wProgSound, pMissile, 0, 0, 0);
        D2COMMON_MISSILE_SetTargetX(pMissile, nBounces);
    }

    MISSILE_CltDoFunc6(pMissile);
}


//1.10f: D2Client.0x6FB34CC0
//  pCltDoFunc 48: erruption center, one row (CltSubMissile1..2 erruption crack 1 and erruption smoke 1) -- fissure's cracks. The record, a
//  non-negative CltSubMissile1, an owner, a room under the missile and the skill's skills.txt row, or the missile is destroyed. The skill's Calc1 and
//  Calc2, each evaluated for a fresh CUNIT_GetOwnerUnit, are the range R and the period (floored at 1). On every period-th frame (a signed
//  remainder): the missile's seed is re-seeded from its X plus F (SEED_InitLowSeed), two rolls below 2 * (R - 1) less R - 1 give an offset -- one
//  column and one row short on the high side -- and when the point is free of collision mask 0x45 in the MISSILE's room (COLLISION_CheckMask), a
//  CltSubMissile1, flags 0x2001, for the owner, with the skill and level, is spawned there and marked dead at once (CUNIT_SetUnitDead), and a
//  CltSubMissile2 at the same point when that is not negative. Then the common per-frame work.
//  The original inlines the Missiles.txt record lookup, DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition, UNITS_GetYPosition and
//  SEED_RollLimitedRandomNumber here.
void __fastcall MISSILE_CltDoFunc48(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2SkillsTxt *pSkillsTxt;
    D2ActiveRoomStrc *pRoom;
    D2UnitStrc *pOwner;
    D2UnitStrc *pSpawned;
    int32_t nMissileId;
    int32_t nSkillId;
    int32_t nLevel;
    int32_t nRange;
    int32_t nPeriod;
    int32_t nFrame;
    int32_t nHalf;
    int32_t nSpread;
    int32_t nX;
    int32_t nY;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltSubMissile[0] < 0)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    pRoom = D2COMMON_UNITS_GetRoom(pMissile);
    if (!pRoom)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    nRange = D2COMMON_SKILLS_EvaluateSkillFormula(CUNIT_GetOwnerUnit(pMissile), pSkillsTxt->dwCalc[0], (D2C_Skills)nSkillId, nLevel);
    nPeriod = D2COMMON_SKILLS_EvaluateSkillFormula(CUNIT_GetOwnerUnit(pMissile), pSkillsTxt->dwCalc[1], (D2C_Skills)nSkillId, nLevel);
    if (nPeriod < 1)
        nPeriod = 1;

    nFrame = D2COMMON_MISSILE_GetRemainingFrames(pMissile);
    if (nFrame % nPeriod)
    {
        MISSILE_CltDoFunc1(pMissile);
        return;
    }

    D2COMMON_SEED_InitLowSeed(&pMissile->pSeed, UNITS_GetXPosition(pMissile) + nFrame);

    nHalf = (int32_t)((uint32_t)nRange - 1);

    nX = UNITS_GetXPosition(pMissile);

    nSpread = (int32_t)(2 * (uint32_t)nHalf);

    nX = (int32_t)((uint32_t)SEED_RollLimitedRandomNumber(&pMissile->pSeed, nSpread) - (uint32_t)nHalf + (uint32_t)nX);

    nY = UNITS_GetYPosition(pMissile);

    nY = (int32_t)((uint32_t)SEED_RollLimitedRandomNumber(&pMissile->pSeed, nSpread) - (uint32_t)nHalf + (uint32_t)nY);

    if (D2COMMON_COLLISION_CheckMask(pRoom, nX, nY, 0x45))
    {
        MISSILE_CltDoFunc1(pMissile);
        return;
    }

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_RANDOMDIR;
    tParams.pOwner = pOwner;
    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
    tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    tParams.nMissile = (int16_t)pMissilesTxt->wCltSubMissile[0];
    tParams.nX = nX;
    tParams.nY = nY;
    pSpawned = MISSILE_ClientSpawn(&tParams);
    if (pSpawned)
        CUNIT_SetUnitDead(pSpawned);

    if ((int16_t)pMissilesTxt->wCltSubMissile[1] >= 0)
    {
        tParams.nMissile = (int16_t)pMissilesTxt->wCltSubMissile[1];
        MISSILE_ClientSpawn(&tParams);
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB34FF0
//  pCltDoFunc 49: vines and plague vines (CltSubMissile1 their trails) -- every CltParam1 frames (at least 1; the remainder of F by it, signed), a
//  CltSubMissile1, flags 0x20, origin the missile, target (0, 0) -- two explicit stores over the cleared block -- with its skill and level, for the
//  missile itself if it is a player or a monster and else for its owner (pCltDoFunc 44's pick: a missile is neither, so the owner); a piece spawned
//  is turned to the missile's direction (PATH_GetDirection, PATH_SetDirection). Then the common per-frame work. No record or a negative
//  CltSubMissile1: destroyed.
//  The original inlines the Missiles.txt record lookup here.
void __fastcall MISSILE_CltDoFunc49(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pSpawnOwner;
    D2UnitStrc *pSpawned;
    int32_t nMissileId;
    int32_t nFrame;
    int32_t nPeriod;
    int32_t nSubMissileId;
    int32_t nSkillLevel;
    int32_t nSkillId;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltSubMissile[0] < 0)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    nFrame = D2COMMON_MISSILE_GetRemainingFrames(pMissile);
    nPeriod = (int32_t)pMissilesTxt->dwCltParam[0];
    if (nPeriod < 1)
        nPeriod = 1;

    if (!(nFrame % nPeriod))
    {
        nSubMissileId = (int16_t)pMissilesTxt->wCltSubMissile[0];
        nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
        if (pMissile->dwUnitType == UNIT_PLAYER || pMissile->dwUnitType == UNIT_MONSTER)
            pSpawnOwner = pMissile;
        else
            pSpawnOwner = CUNIT_GetOwnerUnit(pMissile);
        if (pSpawnOwner)
        {
            memset(&tParams, 0, sizeof(tParams));
            tParams.nTargetX = 0;
            tParams.nTargetY = 0;
            tParams.dwFlags = MISSILESPAWN_TARGETPOS;
            tParams.pOwner = pSpawnOwner;
            tParams.pOrigin = pMissile;
            tParams.nSkill = nSkillId;
            tParams.nSkillLevel = nSkillLevel;
            tParams.nMissile = nSubMissileId;
            pSpawned = MISSILE_ClientSpawn(&tParams);
            if (pSpawned)
                D2COMMON_PATH_SetDirection(pSpawned->pDynamicPath, D2COMMON_PATH_GetDirection(pMissile->pDynamicPath));
        }
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB35110
//  pCltDoFunc 50: volcano, one row (CltSubMissile1 volcano debris 2) -- volcano's debris. The record, a non-negative CltSubMissile1 and the skill's
//  skills.txt row, or the missile is destroyed. The spread is CltParam2, or when that is not above 0 the skill's AuraRangeCalc for the owner, floored
//  at 1; the period is CltParam1, or the skill's Calc4 (dwCalc[3]), floored at 1. While F lies strictly between CltParam3 and CltParam4 (signed
//  `jle`, `jge`) and on every period-th frame: the seed is re-seeded from TargetX (SEED_InitLowSeed), two rolls below 2 * spread + 1, less the
//  spread, give an offset from the missile's position, TargetX becomes the seed's low dword (SEED_GetLowSeed) -- so each drop seeds the next -- and
//  MISSILE_ClientSpawnSkillEffect gets the missile, CltSubMissile1, no spawn offset, the point, the skill, the level and CltParam5. Then the common
//  per-frame work (a tail jump).
//  The original inlines the Missiles.txt record lookup, DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition, UNITS_GetYPosition and
//  SEED_RollLimitedRandomNumber here.
void __fastcall MISSILE_CltDoFunc50(D2UnitStrc *pMissile)
{
    D2MissilesTxt *pMissilesTxt;
    D2SkillsTxt *pSkillsTxt;
    D2UnitStrc *pOwner;
    int32_t nMissileId;
    int32_t nSkillId;
    int32_t nSkillLevel;
    int32_t nSpread;
    int32_t nPeriod;
    int32_t nFrame;
    int32_t nRange;
    int32_t nX;
    int32_t nY;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltSubMissile[0] < 0)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    pOwner = CUNIT_GetOwnerUnit(pMissile);

    nSpread = (int32_t)pMissilesTxt->dwCltParam[1];
    if (nSpread <= 0)
    {
        nSpread = D2COMMON_SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxt->dwAuraRangeCalc, (D2C_Skills)nSkillId, nSkillLevel);
        if (nSpread < 1)
            nSpread = 1;
    }

    nPeriod = (int32_t)pMissilesTxt->dwCltParam[0];
    if (nPeriod <= 0)
    {
        nPeriod = D2COMMON_SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxt->dwCalc[3], (D2C_Skills)nSkillId, nSkillLevel);
        if (nPeriod < 1)
            nPeriod = 1;
    }

    nFrame = D2COMMON_MISSILE_GetRemainingFrames(pMissile);
    if (nFrame <= (int32_t)pMissilesTxt->dwCltParam[2] || nFrame >= (int32_t)pMissilesTxt->dwCltParam[3] || nFrame % nPeriod)
    {
        MISSILE_CltDoFunc1(pMissile);
        return;
    }

    D2COMMON_SEED_InitLowSeed(&pMissile->pSeed, D2COMMON_MISSILE_GetTargetX(pMissile));

    nX = UNITS_GetXPosition(pMissile);

    nRange = (int32_t)(2 * (uint32_t)nSpread + 1);

    nX = (int32_t)((uint32_t)SEED_RollLimitedRandomNumber(&pMissile->pSeed, nRange) - (uint32_t)nSpread + (uint32_t)nX);

    nY = UNITS_GetYPosition(pMissile);

    nY = (int32_t)((uint32_t)SEED_RollLimitedRandomNumber(&pMissile->pSeed, nRange) - (uint32_t)nSpread + (uint32_t)nY);

    D2COMMON_MISSILE_SetTargetX(pMissile, (int32_t)D2COMMON_SEED_GetLowSeed(&pMissile->pSeed));
    MISSILE_ClientSpawnSkillEffect(pMissile, (int16_t)pMissilesTxt->wCltSubMissile[0], 0, 0, nX, nY, D2COMMON_MISSILE_GetSkill(pMissile), nSkillLevel, (int32_t)pMissilesTxt->dwCltParam[4]);

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB353C0
//  pCltDoFunc 51: recycler delay and vine recycler delay, CltSubMissile1..2 recycler vine and recycler explosion. Two single frames, by F, the frames
//  lived:
//    * F == CltParam1, when CltSubMissile1 is not negative: a CltSubMissile1, flags 0x2001, for the owner, with the skill and level, at the missile,
//      then CltParam3 more (the count re-read on every pass) at the missile plus an offset of up to CltParam4 on each axis -- two rolls below 2 *
//      CltParam4 + 1, less CltParam4, one block reused;
//    * F == CltParam2, when CltSubMissile2 is not negative and the missile has an owner: the owner is hidden, dwFlagEx |= UNITFLAGEX_NODRAW, and a
//      CltSubMissile2, flags 0x2000, for the owner, origin the missile, is spawned; a piece spawned plays the row's ProgSound when that is above 0
//      (necromancer_corpseexp_1 for the first row).
//  Then the common per-frame work. No record: destroyed.
//  The original inlines the Missiles.txt record lookup, UNITS_GetXPosition, UNITS_GetYPosition and SEED_RollLimitedRandomNumber here.
void __fastcall MISSILE_CltDoFunc51(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pOwner;
    D2UnitStrc *pSpawned;
    int32_t nMissileId;
    int32_t nFrame;
    int32_t nSpread;
    int32_t nRange;
    int32_t nX;
    int32_t nY;
    int32_t i;
    int16_t nSoundId;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    nFrame = D2COMMON_MISSILE_GetRemainingFrames(pMissile);

    if ((int16_t)pMissilesTxt->wCltSubMissile[0] >= 0 && nFrame == (int32_t)pMissilesTxt->dwCltParam[0])
    {
        nX = UNITS_GetXPosition(pMissile);

        nY = UNITS_GetYPosition(pMissile);

        memset(&tParams, 0, sizeof(tParams));
        tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_RANDOMDIR;
        tParams.nMissile = (int16_t)pMissilesTxt->wCltSubMissile[0];
        tParams.pOwner = CUNIT_GetOwnerUnit(pMissile);
        tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
        tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        tParams.nY = nY;
        tParams.nX = nX;
        MISSILE_ClientSpawn(&tParams);

        nSpread = (int32_t)pMissilesTxt->dwCltParam[3];
        nRange = (int32_t)(2 * (uint32_t)nSpread + 1);
        for (i = 0; i < (int32_t)pMissilesTxt->dwCltParam[2]; ++i)
        {
            tParams.nX = (int32_t)((uint32_t)SEED_RollLimitedRandomNumber(&pMissile->pSeed, nRange) - (uint32_t)nSpread + (uint32_t)nX);

            tParams.nY = (int32_t)((uint32_t)SEED_RollLimitedRandomNumber(&pMissile->pSeed, nRange) - (uint32_t)nSpread + (uint32_t)nY);

            MISSILE_ClientSpawn(&tParams);
        }
    }

    if ((int16_t)pMissilesTxt->wCltSubMissile[1] >= 0 && nFrame == (int32_t)pMissilesTxt->dwCltParam[1])
    {
        pOwner = CUNIT_GetOwnerUnit(pMissile);
        if (pOwner)
        {
            pOwner->dwFlagEx |= UNITFLAGEX_NODRAW;
            memset(&tParams, 0, sizeof(tParams));
            tParams.dwFlags = MISSILESPAWN_RANDOMDIR;
            tParams.nMissile = (int16_t)pMissilesTxt->wCltSubMissile[1];
            tParams.pOwner = pOwner;
            tParams.pOrigin = pMissile;
            tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
            tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
            pSpawned = MISSILE_ClientSpawn(&tParams);
            if (pSpawned)
            {
                nSoundId = (int16_t)pMissilesTxt->wProgSound;
                if (nSoundId > 0)
                    SOUNDINST_PlaySound((D2C_Sounds)nSoundId, pSpawned, 0, 0, 0);
            }
        }
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB35670
//  pCltDoFunc 52: wake of destruction maker and baal cold maker (CltSubMissile1 their trails) -- on every frame the path moved, TargetX and TargetY
//  are read once and two CltSubMissile1 pieces, flags 2, for the owner, origin the missile, with the skill and level, are aimed at them and at their
//  negation (the values read, not read again as pCltDoFunc 46 does). Then the common per-frame work. No record, a negative CltSubMissile1 or no
//  owner: destroyed.
//  The original inlines the Missiles.txt record lookup here.
void __fastcall MISSILE_CltDoFunc52(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pOwner;
    int32_t nMissileId;
    int32_t nTargetX;
    int32_t nTargetY;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltSubMissile[0] < 0)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    if (D2COMMON_PATH_HasMovedThisFrame(pMissile->pDynamicPath))
    {
        nTargetX = D2COMMON_MISSILE_GetTargetX(pMissile);
        nTargetY = D2COMMON_MISSILE_GetTargetY(pMissile);
        memset(&tParams, 0, sizeof(tParams));
        tParams.dwFlags = MISSILESPAWN_TARGETOFFSET;
        tParams.pOwner = pOwner;
        tParams.pOrigin = pMissile;
        tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
        tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        tParams.nTargetX = nTargetX;
        tParams.nTargetY = nTargetY;
        tParams.nMissile = (int16_t)pMissilesTxt->wCltSubMissile[0];
        MISSILE_ClientSpawn(&tParams);

        tParams.nTargetX = (int32_t)(0u - (uint32_t)nTargetX);
        tParams.nTargetY = (int32_t)(0u - (uint32_t)nTargetY);
        MISSILE_ClientSpawn(&tParams);
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB35780
//  pCltDoFunc 53: tigerfury, one row (CltSubMissile1 tigerfurytrail) -- on every frame the path moved, a CltSubMissile1, flags 0, for the owner,
//  origin the missile, with the skill and level; then pCltDoFunc 7, the homing step (a call), in place of the common per-frame work. No record or a
//  negative CltSubMissile1: destroyed.
//  The original inlines the Missiles.txt record lookup here.
void __fastcall MISSILE_CltDoFunc53(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    int32_t nMissileId;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltSubMissile[0] < 0)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    if (D2COMMON_PATH_HasMovedThisFrame(pMissile->pDynamicPath))
    {
        memset(&tParams, 0, sizeof(tParams));
        tParams.pOwner = CUNIT_GetOwnerUnit(pMissile);
        tParams.pOrigin = pMissile;
        tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
        tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        tParams.nMissile = (int16_t)pMissilesTxt->wCltSubMissile[0];
        MISSILE_ClientSpawn(&tParams);
    }

    MISSILE_CltDoFunc7(pMissile);
}


//1.10f: D2Client.0x6FB35840
//  pCltDoFunc 56: baal taunt control, one row (CltSubMissile1..2 its lightning and poison controls). The slots are counted first: CltSubMissile[i]
//  with CltParam[i + 1], while the one is not negative and the other above 0, at most three (two pointers stepped together). None: the missile hits
//  at once, MISSILE_ClientOnHit with no target, not forced, and nothing else runs. Else, once F has reached CltParam1: the seed is re-seeded from the
//  missile's X, a slot is rolled below the count, and on every CltParam[slot + 1]-th frame (`idiv`) that slot's missile, flags 0, for the owner,
//  origin the missile, with the skill and level, is spawned and made to hit at once, forced (bForceExplode 1). Then the common per-frame work. No
//  record: destroyed.
//  The original inlines the Missiles.txt record lookup, UNITS_GetXPosition and SEED_RollLimitedRandomNumber here.
void __fastcall MISSILE_CltDoFunc56(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pSpawned;
    int32_t nMissileId;
    int32_t nFrame;
    int32_t nSlots;
    int32_t nSlot;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    nFrame = D2COMMON_MISSILE_GetRemainingFrames(pMissile);

    for (nSlots = 0; nSlots < 3; ++nSlots)
    {
        if ((int16_t)pMissilesTxt->wCltSubMissile[nSlots] < 0 || (int32_t)pMissilesTxt->dwCltParam[nSlots + 1] <= 0)
            break;
    }

    if (nSlots <= 0)
    {
        MISSILE_ClientOnHit(pMissile, NULL, 0);
        return;
    }

    if (nFrame >= (int32_t)pMissilesTxt->dwCltParam[0])
    {
        D2COMMON_SEED_InitLowSeed(&pMissile->pSeed, UNITS_GetXPosition(pMissile));

        nSlot = SEED_RollLimitedRandomNumber(&pMissile->pSeed, nSlots);

        if (!(nFrame % (int32_t)pMissilesTxt->dwCltParam[nSlot + 1]))
        {
            memset(&tParams, 0, sizeof(tParams));
            tParams.pOwner = CUNIT_GetOwnerUnit(pMissile);
            tParams.pOrigin = pMissile;
            tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
            tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
            tParams.nMissile = (int16_t)pMissilesTxt->wCltSubMissile[nSlot];
            pSpawned = MISSILE_ClientSpawn(&tParams);
            if (pSpawned)
                MISSILE_ClientOnHit(pSpawned, NULL, 1);
        }
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB359C0
//  pCltDoFunc 57: blade shield attachment, one row (CltSubMissile1 blade shield missile) -- pCltDoFunc 43 with a volley. No record, a negative
//  CltSubMissile1 or no owner: destroyed. Then pCltDoFunc 43's whole body, written into this one: the missile rides its owner, or hits when a player
//  or monster owner is dead. And then, whatever that did to the missile (where all three of its arms meet): when the owner's path has a target
//  (SKILLS_GetPathTargetPos), TargetY is raised by one while below TargetX, a CltSubMissile1, flags 0x20, owned by and starting from the OWNER, aimed
//  at that target, with the missile's skill and level, is spawned and given path type 14 and a move, and TargetY is set back to 0 -- so the count it
//  just raised never passes 1 -- and the common per-frame work runs. With no target nothing more happens, not even the common per-frame work.
//  The original inlines the Missiles.txt record lookup and MISSILE_CltDoFunc43 here.
void __fastcall MISSILE_CltDoFunc57(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pOwner;
    D2UnitStrc *pSpawned;
    int32_t nMissileId;
    int32_t nTargetX;
    int32_t nTargetY;
    int32_t nCount;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltSubMissile[0] < 0)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    MISSILE_CltDoFunc43(pMissile);

    if (!SKILLS_GetPathTargetPos(pOwner, &nTargetX, &nTargetY))
        return;

    nCount = D2COMMON_MISSILE_GetTargetY(pMissile);
    if (nCount < D2COMMON_MISSILE_GetTargetX(pMissile))
        D2COMMON_MISSILE_SetTargetY(pMissile, nCount + 1);

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_TARGETPOS;
    tParams.pOwner = pOwner;
    tParams.pOrigin = pOwner;
    tParams.nTargetX = nTargetX;
    tParams.nTargetY = nTargetY;
    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
    tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    tParams.nMissile = (int16_t)pMissilesTxt->wCltSubMissile[0];
    pSpawned = MISSILE_ClientSpawn(&tParams);
    if (pSpawned)
    {
        D2COMMON_PATH_SetType(pSpawned->pDynamicPath, 14);
        D2COMMON_PATH_StartUnitMove(pSpawned->pDynamicPath, pSpawned, 0);
    }

    D2COMMON_MISSILE_SetTargetY(pMissile, 0);
    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB35B40
//  pCltDoFunc 58: royalstrikechaosice, one row -- a path that wobbles. The period is the row's SERVER column Param1, not CltParam1 (the stock row has
//  3 in both). On every period-th frame (at least 1): the seed is re-seeded from TargetX, TargetY is read as two signed words, a vector (X low, Y
//  high; `movsx`), and one step of the seed turns it: bit 0 of the low dword clear negates X, set negates Y, and the new vector is ((Y' + 4X) / 4,
//  (X' + 4Y) / 4) -- signed quarters, a zero component made 1. The path's target is set that far from the missile and the move restarted; TargetX
//  becomes the seed's low dword and TargetY the new vector packed back, each half masked to 16 bits. Then the common per-frame work (a tail jump). No
//  record: destroyed. `&pMissile->pSeed` is tested for NULL (`test ebp, ebp / jz`), which cannot fire for a unit already dereferenced: a macro's own
//  check, written as the source had it.
//  The original inlines the Missiles.txt record lookup, SEED_RollRandomNumber, UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall MISSILE_CltDoFunc58(D2UnitStrc *pMissile)
{
    D2MissilesTxt *pMissilesTxt;
    D2SeedStrc *pSeed;
    int32_t nMissileId;
    int32_t nPeriod;
    int32_t nPacked;
    int32_t nVecX;
    int32_t nVecY;
    int32_t nTurnX;
    int32_t nTurnY;
    int32_t nStepX;
    int32_t nStepY;
    int32_t nX;
    int32_t nY;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    nPeriod = 1;
    if (pMissilesTxt->dwParam[0] >= 1)
        nPeriod = pMissilesTxt->dwParam[0];
    if (D2COMMON_MISSILE_GetRemainingFrames(pMissile) % nPeriod)
    {
        MISSILE_CltDoFunc1(pMissile);
        return;
    }

    pSeed = &pMissile->pSeed;
    if (!pSeed)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    D2COMMON_SEED_InitLowSeed(pSeed, D2COMMON_MISSILE_GetTargetX(pMissile));
    nPacked = D2COMMON_MISSILE_GetTargetY(pMissile);
    nVecX = (int16_t)nPacked;
    nVecY = (int16_t)((uint32_t)nPacked >> 16);

    if (SEED_RollRandomNumber(pSeed) & 1)
    {
        nTurnX = nVecX;
        nTurnY = -nVecY;
    }
    else
    {
        nTurnX = -nVecX;
        nTurnY = nVecY;
    }

    nStepX = (nTurnY + 4 * nVecX) / 4;
    nStepY = (nTurnX + 4 * nVecY) / 4;
    if (!nStepX)
        nStepX = 1;
    if (!nStepY)
        nStepY = 1;

    nY = UNITS_GetYPosition(pMissile);

    nX = UNITS_GetXPosition(pMissile);

    D2COMMON_10170_PathSetTargetPos(pMissile->pDynamicPath, nX + nStepX, nY + nStepY);
    D2COMMON_PATH_StartUnitMove(pMissile->pDynamicPath, pMissile, 0);
    D2COMMON_MISSILE_SetTargetX(pMissile, (int32_t)D2COMMON_SEED_GetLowSeed(pSeed));
    D2COMMON_MISSILE_SetTargetY(pMissile, (int32_t)((((uint32_t)nStepY & 0xFFFF) << 16) + ((uint32_t)nStepX & 0xFFFF)));
    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB35D10
//  A missile on every subtile of a disc. Callers: SKILLS_CltDoFunc53, pCltDoFunc 17 and pCltHitFunc 1, 32 and 57. The owner is pUnit itself for a
//  player or a monster (pCltDoFunc 44's pick, a NULL unit handed to CUNIT_GetOwnerUnit), else its owner -- none, nothing happens. One block, flags
//  0x201, the owner, nSkillId, nLevel and nMissileId, is spawned for every (dx, dy) from -nRadius to nRadius by nStep, dy outer, with dx*dx + dy*dy
//  <= nRadius*nRadius (`imul`, a signed `jg` past it), at (nX + dx, nY + dy), when:
//    * the chance passes: a roll below nOneInChance on pUnit's seed is 0 -- and with nOneInChance not above 0 no roll is made at all;
//    * the room that holds the point is found from pUnit's.
//  A non-zero RandStart gives each spawn a start frame rolled below it on the same seed, 0 for a negative one. The effect is the disc met with the
//  room and its neighbours: a point farther off is dropped. void: the original's eax at the exit is a leftover (the loop counter or the owner), not a
//  result.
//  The original inlines the Missiles.txt record lookup, SEED_RollLimitedRandomNumber, CLIENT_FindRoomContainingSubtile here.
void __fastcall MISSILE_SpawnOverArea(D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t nRadius, int32_t nMissileId, int32_t nSkillId, int32_t nLevel, int32_t nOneInChance, int32_t nStep)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2ActiveRoomStrc *pRoom;
    D2UnitStrc *pOwner;
    int32_t nRadiusSq;
    int32_t nDx;
    int32_t nDy;

    pMissilesTxt = NULL;
    if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
        pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];

    if (pUnit && (pUnit->dwUnitType == UNIT_PLAYER || pUnit->dwUnitType == UNIT_MONSTER))
    {
        pOwner = pUnit;
    }
    else
    {
        pOwner = CUNIT_GetOwnerUnit(pUnit);
        if (!pOwner)
            return;
    }

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_STARTFRAME;
    //  BUG: the record lookup keeps NULL for an id out of range and nothing tests it. LATENT: the first read, of RandStart, would
    //  fault on such an id.
    if (pMissilesTxt->dwRandStart)
        tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_STARTFRAME;
    tParams.nSkillLevel = nLevel;
    tParams.pOwner = pOwner;
    tParams.nSkill = nSkillId;
    tParams.nMissile = nMissileId;
    nRadiusSq = (int32_t)((uint32_t)nRadius * (uint32_t)nRadius);

    for (nDy = -nRadius; nDy <= nRadius; nDy += nStep)
    {
        for (nDx = -nRadius; nDx <= nRadius; nDx += nStep)
        {
            if ((int32_t)((uint32_t)nDx * (uint32_t)nDx + (uint32_t)nDy * (uint32_t)nDy) > nRadiusSq)
                continue;

            if (SEED_RollLimitedRandomNumber(&pUnit->pSeed, nOneInChance))
                continue;

            tParams.nX = nDx + nX;
            tParams.nY = nDy + nY;
            if (pMissilesTxt->dwRandStart)
            {
                tParams.nFrame = SEED_RollLimitedRandomNumber(&pUnit->pSeed, (int32_t)pMissilesTxt->dwRandStart);
            }

            pRoom = CLIENT_FindRoomContainingSubtile(D2COMMON_UNITS_GetRoom(pUnit), tParams.nX, tParams.nY);
            if (pRoom)
                MISSILE_ClientSpawn(&tParams);
        }
    }
}


//1.10f: D2Client.0x6FB35FD0
//  Two rings of one missile from another. Callers: SKILLS_SpawnPoisonExplosionEffect, pCltHitFunc 2 and 63. Nothing for an id of 0 (`test ebx, ebx /
//  jz`). One block, flags 0x17 -- 0x1F with nCltParam as the loops when that is above 0 -- for the missile's owner, origin the missile, at the
//  missile, with its skill and level:
//    * at the SERVER column Param1 << 7 as the velocity, aimed along gnSpawnRing16X/Y from entry 0, every max(nFullRingStep, 1)-th of the sixteen;
//    * when nRingDirectionStep is above 0, at Param2 << 7, along entries 1 to 15, every nRingDirectionStep-th.
//  Both loops walk a BYTE offset -- 4 * step added to it while it is below 0x40 and 0x3C -- and are written so.
//  The original inlines the Missiles.txt record lookup, UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall MISSILE_SpawnFromMissile(D2UnitStrc *pMissile, int32_t nMissileId, int32_t nRingDirectionStep, int32_t nFullRingStep, int32_t nCltParam)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    int32_t nByteStep;
    int32_t nOffset;

    pMissilesTxt = NULL;
    if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
        pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];

    if (!nMissileId)
        return;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETOFFSET | MISSILESPAWN_VELOCITY | MISSILESPAWN_VELOCITYSCALED;
    if (nCltParam > 0)
    {
        tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETOFFSET | MISSILESPAWN_VELOCITY | MISSILESPAWN_SUBLOOPS | MISSILESPAWN_VELOCITYSCALED;
        tParams.nLoops = nCltParam;
    }
    tParams.pOwner = CUNIT_GetOwnerUnit(pMissile);
    tParams.pOrigin = pMissile;
    tParams.nMissile = nMissileId;

    tParams.nX = UNITS_GetXPosition(pMissile);

    tParams.nY = UNITS_GetYPosition(pMissile);

    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
    tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);

    //  BUG: the record lookup keeps NULL for an id out of range and this first velocity read is untested. LATENT: only a non-zero bad id reaches it.
    tParams.nVelocity = (int32_t)((uint32_t)pMissilesTxt->dwParam[0] << 7);
    if (nFullRingStep < 1)
        nFullRingStep = 1;
    nByteStep = (int32_t)(4u * (uint32_t)nFullRingStep);
    for (nOffset = 0; nOffset < 0x40; nOffset = (int32_t)((uint32_t)nOffset + (uint32_t)nByteStep))
    {
        tParams.nTargetX = gnSpawnRing16X[nOffset / 4];
        tParams.nTargetY = gnSpawnRing16Y[nOffset / 4];
        MISSILE_ClientSpawn(&tParams);
    }

    if (nRingDirectionStep > 0)
    {
        tParams.nVelocity = (int32_t)((uint32_t)pMissilesTxt->dwParam[1] << 7);
        nByteStep = (int32_t)(4u * (uint32_t)nRingDirectionStep);
        for (nOffset = 0; nOffset < 0x3C; nOffset = (int32_t)((uint32_t)nOffset + (uint32_t)nByteStep))
        {
            tParams.nTargetX = gnSpawnRing16X[1 + nOffset / 4];
            tParams.nTargetY = gnSpawnRing16Y[1 + nOffset / 4];
            MISSILE_ClientSpawn(&tParams);
        }
    }
}


//  The pCltHitFunc handlers. gpMissileCltHitFuncs (0x6FB913D0) is indexed by Missiles.txt pCltHitFunc and called by MISSILE_ClientOnHit as
//  f(pMissile, pTarget), __fastcall; unlike the pCltDoFunc table's its answer is read (`call eax / test eax, eax / jz`). pTarget is the unit hit,
//  often NULL; a handler that does not read it still receives it in edx.

//1.10f: D2Client.0x6FB36150
//  pCltHitFunc 64 -- no stock row carries it. For the missile's owner (asked FIRST: none, or no record, or a negative CltHitSubMissile1 answers 1 and
//  does nothing): one block, flags 2, the owner, origin the missile, its skill and level, aimed along gnMissileDir8OffsetX/Y every max(cHitPar1,
//  1)-th entry of the eight (a byte offset stepped by 4 * the stride below 0x20), and a piece spawned gets that entry as its TargetX/Y. Answers 1.
//  The original inlines the Missiles.txt record lookup here.
int32_t __fastcall MISSILE_CltHitFunc64(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pOwner;
    D2UnitStrc *pSpawned;
    int32_t nMissileId;
    int32_t nStride;
    int32_t nByteStep;
    int32_t nOffset;

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
        return 1;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltHitSubMissile[0] < 0)
        return 1;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_TARGETOFFSET;
    tParams.pOwner = pOwner;
    tParams.pOrigin = pMissile;
    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
    tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    //  BUG: the block's missile id is never stored, so every piece is missile 0 whatever CltHitSubMissile1 says. UNREACHABLE: no stock row carries
    //  pCltHitFunc 64.

    nStride = (int32_t)pMissilesTxt->dwCltHitPar[0];
    if (nStride < 1)
        nStride = 1;
    nByteStep = (int32_t)((uint32_t)nStride << 2);
    for (nOffset = 0; nOffset < 0x20; nOffset = (int32_t)((uint32_t)nOffset + (uint32_t)nByteStep))
    {
        tParams.nTargetY = gnMissileDir8OffsetY[nOffset / 4];
        tParams.nTargetX = gnMissileDir8OffsetX[nOffset / 4];
        pSpawned = MISSILE_ClientSpawn(&tParams);
        if (pSpawned)
        {
            D2COMMON_MISSILE_SetTargetX(pSpawned, gnMissileDir8OffsetX[nOffset / 4]);
            D2COMMON_MISSILE_SetTargetY(pSpawned, gnMissileDir8OffsetY[nOffset / 4]);
        }
    }

    return 1;
}


//1.10f: D2Client.0x6FB36250
//  pCltHitFunc 1: explodingarrow, fireball and vampirefireball (CltHitSubMissile1 fireexplosion2) -- a fire burst. MISSILE_SpawnOverArea at the
//  missile's position, radius cHitPar1, one in cHitPar2, step 1, with the missile's skill and level -- and missile 265, fireexplosion2, a constant
//  (`push 109h`): CltHitSubMissile1 is tested for a negative and not used. The position is read Y before X. No record or a negative
//  CltHitSubMissile1: nothing. Answers 1.
//  The original inlines the Missiles.txt record lookup, UNITS_GetYPosition and UNITS_GetXPosition here.
int32_t __fastcall MISSILE_CltHitFunc1(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissilesTxt *pMissilesTxt;
    int32_t nMissileId;
    int32_t nOneInChance;
    int32_t nLevel;
    int32_t nSkillId;
    int32_t nRadius;
    int32_t nX;
    int32_t nY;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltHitSubMissile[0] < 0)
        return 1;

    nOneInChance = (int32_t)pMissilesTxt->dwCltHitPar[1];
    nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
    nRadius = (int32_t)pMissilesTxt->dwCltHitPar[0];

    nY = UNITS_GetYPosition(pMissile);

    nX = UNITS_GetXPosition(pMissile);

    MISSILE_SpawnOverArea(pMissile, nX, nY, nRadius, 265, nSkillId, nLevel, nOneInChance, 1);
    return 1;
}


//1.10f: D2Client.0x6FB36350
//  pCltHitFunc 2: the plague javelins, the gas potions and the catapult plague ball -- MISSILE_SpawnFromMissile with CltHitSubMissile1 (their clouds)
//  and cHitPar1..3 as the second ring's step, the first ring's step and the loops. Nothing without a record or with a negative CltHitSubMissile1
//  (`test dx, dx / jl`). Answers 1.
//  The original inlines the Missiles.txt record lookup here.
int32_t __fastcall MISSILE_CltHitFunc2(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissilesTxt *pMissilesTxt;
    int32_t nMissileId;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt)
        return 1;

    if ((int16_t)pMissilesTxt->wCltHitSubMissile[0] >= 0)
    {
        MISSILE_SpawnFromMissile(pMissile, (int16_t)pMissilesTxt->wCltHitSubMissile[0], (int32_t)pMissilesTxt->dwCltHitPar[0], (int32_t)pMissilesTxt->dwCltHitPar[1], (int32_t)pMissilesTxt->dwCltHitPar[2]);
    }

    return 1;
}


//1.10f: D2Client.0x6FB363B0
//  pCltHitFunc 3: oilpotion, explosivepotion, fulminatingpotion and pantherpotorange -- an explosion and a piece of debris. CltHitSubMissile1 is
//  spawned, flags 0x20, origin the missile, target (0, 0) stored explicitly, with its skill and level, for the missile itself if a player or a
//  monster and else for its owner (no owner skips this spawn only). Then, when CltHitSubMissile2 is not negative: an id from CltHitSubMissile2 to
//  CltHitSubMissile3 -- the second plus a roll below (third - second + 1) when the third is greater, tested twice, as words (`cmp bx, ax / jle`) and
//  again as dwords (`cmp ebp, eax / jge`) -- spawned the same way, its owner picked again, and marked dead at once (CUNIT_SetUnitDead). For
//  fulminatingpotion that is explosivepotiondebris1..3. No record or a negative CltHitSubMissile1: nothing. Answers 1.
//  The original inlines the Missiles.txt record lookup and SEED_RollLimitedRandomNumber here.
int32_t __fastcall MISSILE_CltHitFunc3(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pOwner;
    D2UnitStrc *pSpawned;
    int32_t nMissileId;
    int32_t nLevel;
    int32_t nSkillId;
    int32_t nFirstId;
    int32_t nLo;
    int32_t nHi;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltHitSubMissile[0] < 0)
        return 1;

    nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
    nFirstId = (int16_t)pMissilesTxt->wCltHitSubMissile[0];
    if (pMissile->dwUnitType == UNIT_PLAYER || pMissile->dwUnitType == UNIT_MONSTER)
        pOwner = pMissile;
    else
        pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (pOwner)
    {
        memset(&tParams, 0, sizeof(tParams));
        tParams.pOwner = pOwner;
        tParams.nSkill = nSkillId;
        tParams.dwFlags = MISSILESPAWN_TARGETPOS;
        tParams.pOrigin = pMissile;
        tParams.nTargetX = 0;
        tParams.nTargetY = 0;
        tParams.nSkillLevel = nLevel;
        tParams.nMissile = nFirstId;
        MISSILE_ClientSpawn(&tParams);
    }

    if ((int16_t)pMissilesTxt->wCltHitSubMissile[1] < 0)
        return 1;

    nLo = (int16_t)pMissilesTxt->wCltHitSubMissile[1];
    nHi = (int16_t)pMissilesTxt->wCltHitSubMissile[2];
    nMissileId = nLo;
    if (nHi > nLo && nLo < nHi)
    {
        nMissileId = nLo + SEED_RollLimitedRandomNumber(&pMissile->pSeed, nHi - nLo + 1);
    }

    nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
    if (pMissile->dwUnitType == UNIT_PLAYER || pMissile->dwUnitType == UNIT_MONSTER)
        pOwner = pMissile;
    else
        pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
        return 1;

    memset(&tParams, 0, sizeof(tParams));
    tParams.pOrigin = pMissile;
    tParams.dwFlags = MISSILESPAWN_TARGETPOS;
    tParams.pOwner = pOwner;
    tParams.nTargetX = 0;
    tParams.nTargetY = 0;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nLevel;
    tParams.nMissile = nMissileId;
    pSpawned = MISSILE_ClientSpawn(&tParams);
    if (pSpawned)
        CUNIT_SetUnitDead(pSpawned);

    return 1;
}


//1.10f: D2Client.0x6FB36570
//  pCltHitFunc 9: holybolt and fistoftheheavensbolt (CltHitSubMissile1 teethexplode) -- a bolt that heals the owner's side and bursts on the others.
//  Answers 1 with no record or no target. With cHitPar1 set, an owner, and a target the owner may pick as a pet or an ally
//  (CUNIT_IsSelectableAsPetTarget, then ...AllyTarget): the row's ProgOverlay, when above 0 (healing for holybolt), is played once on the target
//  (GFX_AddOverlay, kind 2), and it answers 1. Otherwise by the target's type:
//    * a player: answers 0 when cHitPar2 is set, else bursts;
//    * a monster: bursts when a demon (cHitPar2 == 2) or else an undead (MONSTERS_IsDemon, MONSTERS_IsUndead); answers 0 when it is not;
//    * anything else: answers 0.
//  The burst, when CltHitSubMissile1 is not negative: missile 204, teethexplode -- a constant -- for the owner, origin the bolt, with its skill and
//  level, the owner possibly NULL (asked first, tested only on the overlay's way). It answers 1.
//  The original inlines the Missiles.txt record lookup here.
BOOL __fastcall MISSILE_CltHitFunc9(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pOwner;
    int32_t nMissileId;
    int16_t nOverlayId;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || !pTarget)
        return 1;

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (pMissilesTxt->dwCltHitPar[0] && pOwner && (CUNIT_IsSelectableAsPetTarget(pOwner, pTarget) || CUNIT_IsSelectableAsAllyTarget(pOwner, pTarget)))
    {
        nOverlayId = (int16_t)pMissilesTxt->wProgOverlay;
        if (nOverlayId > 0)
            GFX_AddOverlay(pTarget, (D2_OverlayIds)nOverlayId, OVERLAYTYPE_ONCE, 0, 0, 0, 0, 0);
        return 1;
    }

    switch (pTarget->dwUnitType)
    {
    case UNIT_PLAYER:
        if (pMissilesTxt->dwCltHitPar[1])
            return 0;
        break;

    case UNIT_MONSTER:
        if (pMissilesTxt->dwCltHitPar[1] == 2)
        {
            if (!D2COMMON_MONSTERS_IsDemon(pTarget))
                return 0;
        }
        else
        {
            if (!D2COMMON_MONSTERS_IsUndead(pTarget))
                return 0;
        }
        break;

    default:
        return 0;
    }

    if ((int16_t)pMissilesTxt->wCltHitSubMissile[0] >= 0)
    {
        memset(&tParams, 0, sizeof(tParams));
        tParams.pOwner = pOwner;
        tParams.pOrigin = pMissile;
        tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
        tParams.nMissile = 204;
        tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        MISSILE_ClientSpawn(&tParams);
    }

    return 1;
}


//1.10f: D2Client.0x6FB366D0
//  pCltHitFunc 10: chargedbolt, nova, frostnova, lightningjavelin, chargedstrikebolt, warcry and the lightning of monsters, sentries, towers and
//  traps -- the row's ProgOverlay played once on the unit hit (GFX_AddOverlay, kind 2). Nothing without a unit hit (tested FIRST, before the
//  missile), without a record, or with a ProgOverlay not above 0 (`test ax, ax / jle`). Answers 1.
//  The original inlines the Missiles.txt record lookup here.
int32_t __fastcall MISSILE_CltHitFunc10(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissilesTxt *pMissilesTxt;
    int32_t nMissileId;
    int16_t nOverlayId;

    if (!pTarget)
        return 1;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt)
        return 1;

    nOverlayId = (int16_t)pMissilesTxt->wProgOverlay;
    if (nOverlayId > 0)
        GFX_AddOverlay(pTarget, (D2_OverlayIds)nOverlayId, OVERLAYTYPE_ONCE, 0, 0, 0, 0, 0);

    return 1;
}


//1.10f: D2Client.0x6FB36740
//  pCltHitFunc 12: immolationarrow alone (cHitPar1..2 of 0 and 75; CltHitSubMissile1..3 firewall, firesmall, firemedium) -- the fire left where the
//  arrow bursts. The CltHitSubMissile ids up to the first negative one, three at most, go into a table on the stack (the original steps a word
//  pointer beside the count), and when there is at least one, MISSILE_SprayRandomOverArea scatters them:
//    * radius cHitPar1 when above 0 (`jg`), else the skill's Calc1 for the owner, at least 1 (`cmp edi, 1 / jge`);
//    * range MISSILE_EvaluateMissileFormula over the row's dwHitCalc when above 0 (`jg`), else Range + (level - 1) * LevRange;
//    * chance cHitPar2 per cent.
//  Nothing without a record or a skill record -- the skill is looked up even when cHitPar1 needs no formula -- or with no id. Answers 1.
//  BUG: the range formula is read from dwHitCalc (+0x88), the SERVER's SHitCalc1, where the client's own column is CHitCalc1 (+0x8C). INERT:
//  immolationarrow carries 75 in both.
//  The original inlines the Missiles.txt record lookup and DATATBLS_GetSkillsTxtRecordById here.
int32_t __fastcall MISSILE_CltHitFunc12(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissilesTxt *pMissilesTxt;
    D2SkillsTxt *pSkillsTxt;
    const uint16_t *pwSubMissile;
    int32_t anMissileIds[3];
    int32_t nMissileId;
    int32_t nSkillId;
    int32_t nLevel;
    int32_t nMissileCount;
    int32_t nRadius;
    int32_t nRange;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt)
        return 1;

    nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
    nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 1;

    nMissileCount = 0;
    pwSubMissile = pMissilesTxt->wCltHitSubMissile;
    do
    {
        if ((int16_t)*pwSubMissile < 0)
            break;
        anMissileIds[nMissileCount] = (int16_t)*pwSubMissile;
        ++nMissileCount;
        ++pwSubMissile;
    }
    while (nMissileCount < 3);

    if (nMissileCount <= 0)
        return 1;

    nRadius = (int32_t)pMissilesTxt->dwCltHitPar[0];
    if (nRadius <= 0)
    {
        nRadius = D2COMMON_SKILLS_EvaluateSkillFormula(CUNIT_GetOwnerUnit(pMissile), pSkillsTxt->dwCalc[0], (D2C_Skills)nSkillId, nLevel);
        if (nRadius < 1)
            nRadius = 1;
    }

    nRange = D2COMMON_MISSILE_EvaluateMissileFormula(pMissile, CUNIT_GetOwnerUnit(pMissile), pMissilesTxt->dwHitCalc, pMissile->dwClassId, nLevel);
    if (nRange <= 0)
        nRange = (int16_t)pMissilesTxt->wLevRange * (nLevel - 1) + (int16_t)pMissilesTxt->wRange;

    MISSILE_SprayRandomOverArea(pMissile, anMissileIds, nMissileCount, nRadius, nRange, (int32_t)pMissilesTxt->dwCltHitPar[1]);
    return 1;
}


//  The target masks of the homing arrow's search and of the chain's.
static const D2C_AuraFilters nHomingTargetMask = (D2C_AuraFilters)(AURAFILTER_FINDPLAYER | AURAFILTER_FINDMONSTER);
static const D2C_AuraFilters nChainTargetMask = (D2C_AuraFilters)(AURAFILTER_FINDPLAYER | AURAFILTER_FINDMONSTER | AURAFILTER_FINDISATT | AURAFILTER_IGNINTOWN | AURAFILTER_FINDISSEL | AURAFILTER_IGNALLY | AURAFILTER_IGNJUSTHIT);
static_assert(nHomingTargetMask == 3 && nChainTargetMask == 0x88583,
              "`push 3` (0x6FB369D9), `push 88583h` (0x6FB36F5B)");

//1.10f: D2Client.0x6FB36890
//  pCltHitFunc 13: guidedarrow, bonespirit and monbonespirit (pCltDoFunc 7, the homing step) -- the homing missile's hit, which says whether the hit
//  is taken (1) or the missile flies on (0: MISSILE_ClientOnHit then answers 0 at once, before its hit sound). MISSILE_GetTargetX is read as flags, a
//  byte (`test bl, 4`; `and eax, 0FFh`): bit 0 homing on the path's target unit, bit 1 flying at a point and not yet homing, bit 2 set here when it
//  picks.
//    * no owner, a dead owner, or a room in town: 1;
//    * no unit hit and bit 2: 1;
//    * bit 0: 1 for no unit hit or the path's target unit (the path read at entry), 0 for any other -- it flies through;
//    * neither bit 0 nor bit 1: 1;
//    * bit 1 while MISSILE_GetCurrentFrame is above 0 (`jg`) -- the count that runs DOWN: pCltDoFunc 7 ends in MISSILE_CltDoFunc1, which takes one
//      off it each frame -- bit 2: 0 until it has picked, so it flies through whatever it meets on the way to its point;
//    * bit 1 with that count at 0: no record, bit 2 asked again, or no owner answer 1; else it picks the owner's first player or monster by unit id
//      (SKILLS_FindNextNearbyTargetById, mask 3, after -1 -- the smallest id) within Param2 of the missile, dropped if dead; restarts its count at
//      Range + LevRange * (level - 1), the total and the current both; and homes -- on that unit (PATH_SetTargetUnit, TargetX 5) or, with none, on
//      the point TargetY holds as two signed words added to its position (low X, high Y: `shr ebp, 10h`, `movsx ebx, di`, `movsx edi, bp`; TargetX 6)
//      -- and within 25 of either starts the move at once (PATH_StartUnitMove). Answers 0.
//  pMissile is never tested: its path is read first thing, and MISSILE_ClientOnHit, the table's caller, has tested it. The record is looked up twice,
//  the second time after the search, and THAT lookup's failure answers 0 where the first's answers 1 -- the same class id in the same table, so it
//  cannot fail when the first did not.
//  The original inlines the Missiles.txt record lookup, UNITS_GetYPosition and UNITS_GetXPosition here.
BOOL __fastcall MISSILE_CltHitFunc13(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2DynamicPathStrc *pEntryPath;
    D2DynamicPathStrc *pPath;
    D2MissilesTxt *pMissilesTxt;
    D2ActiveRoomStrc *pRoom;
    D2UnitStrc *pOwner;
    D2UnitStrc *pNewTarget;
    uint8_t nFlags;
    int32_t nMissileId;
    int32_t nRadius;
    int32_t nRange;
    int32_t nAim;
    int32_t nX;
    int32_t nY;
    int32_t nDistance;

    pEntryPath = pMissile->pDynamicPath;
    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner || CUNIT_IsUnitDead(pOwner))
        return 1;

    pRoom = D2COMMON_UNITS_GetRoom(pMissile);
    if (pRoom && D2COMMON_DUNGEON_IsRoomInTown(pRoom))
        return 1;

    nFlags = (uint8_t)D2COMMON_MISSILE_GetTargetX(pMissile);
    if (!pTarget && (nFlags & 4))
        return 1;

    if (nFlags & 1)
    {
        if (!pTarget || pTarget == D2COMMON_PATH_GetTargetUnit(pEntryPath))
            return 1;
        return 0;
    }

    if (!(nFlags & 2))
        return 1;

    if (D2COMMON_MISSILE_GetCurrentFrame(pMissile) > 0)
        return (nFlags >> 2) & 1;

    pMissilesTxt = NULL;
    nMissileId = pMissile->dwClassId;
    if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
        pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    if (!pMissilesTxt)
        return 1;

    nRadius = pMissilesTxt->dwParam[1];
    if (D2COMMON_MISSILE_GetTargetX(pMissile) & 4)
        return 1;

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
        return 1;

    nY = UNITS_GetYPosition(pMissile);

    nX = UNITS_GetXPosition(pMissile);

    pNewTarget = SKILLS_FindNextNearbyTargetById(pOwner, nX, nY, nRadius, nHomingTargetMask, (uint32_t)-1, NULL);

    pMissilesTxt = NULL;
    nMissileId = pMissile->dwClassId;
    if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
        pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    if (!pMissilesTxt)
        return 0;

    if (pNewTarget && CUNIT_IsUnitDead(pNewTarget))
        pNewTarget = NULL;

    nRange = ((int32_t)D2COMMON_MISSILE_GetLevel(pMissile) - 1) * (int16_t)pMissilesTxt->wLevRange + (int16_t)pMissilesTxt->wRange;
    D2COMMON_MISSILE_SetTotalFrames(pMissile, nRange);
    D2COMMON_MISSILE_SetCurrentFrame(pMissile, nRange);

    pPath = pMissile->pDynamicPath;
    if (pNewTarget)
    {
        D2COMMON_PATH_SetTargetUnit(pPath, pNewTarget);
        D2COMMON_MISSILE_SetTargetX(pMissile, 5);
        nDistance = D2COMMON_UNITS_GetDistanceToOtherUnit(pMissile, pNewTarget);
    }
    else
    {
        nAim = D2COMMON_MISSILE_GetTargetY(pMissile);

        nX = UNITS_GetXPosition(pMissile) + (int16_t)nAim;

        nY = UNITS_GetYPosition(pMissile) + (int16_t)((uint32_t)nAim >> 16);

        D2COMMON_10170_PathSetTargetPos(pPath, nX, nY);
        D2COMMON_MISSILE_SetTargetX(pMissile, 6);
        nDistance = (int32_t)D2COMMON_UNITS_GetDistanceToCoordinates(pMissile, nX, nY);
    }

    if (nDistance < 25)
        D2COMMON_PATH_StartUnitMove(pPath, pMissile, 0);

    return 0;
}


//1.10f: D2Client.0x6FB36B50
//  pCltHitFunc 14: freezingarrow, glacialspike and monglacialspike (CltHitSubMissile1..2 freezingarrowexp1 and freezingarrowexp2) -- the ice burst.
//  The missile's position is taken first, X then Y. CltHitSubMissile1, when not negative, is spawned once, flags 0x20, origin the missile, target (0,
//  0) stored explicitly, with its skill and level, for the missile itself if a player or a monster and else for its owner (pCltHitFunc 3's pick; no
//  owner skips this spawn only). Then, when CltHitSubMissile2 is not negative, the shards: two steps of the missile's seed pick a row (the low dword
//  `% 3`, `div ecx`) and a turn (the next low dword `& 7`), and gnGlacialSpikeBurstParams (15 dwords, three rows of a count and four directions)
//  gives:
//      row 0   4 shards   directions 0, 1, 4, 5
//      row 1   4 shards   directions 0, 2, 4, 6
//      row 2   3 shards   directions 0, 3, 5      (its fourth, 0, unread)
//  Each shard is CltHitSubMissile2, spawned as the first -- its owner picked, its skill, level and id read again for each -- and faced at the burst's
//  position plus gnDirection8OffsetX/Y[(direction + turn) & 7], the eight neighbours from (0, +1) for 0 through (+1, 0) for 2, (0, -1) for 4 and (-1,
//  0) for 6. The count is re-read at every turn of the loop and the direction pointer stepped by 4 (`add edx, 4`), a shard with no owner included. No
//  record: nothing. Answers 1.
//  The original inlines the Missiles.txt record lookup, UNITS_GetXPosition, UNITS_GetYPosition and SEED_RollRandomNumber here.
int32_t __fastcall MISSILE_CltHitFunc14(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pOwner;
    D2UnitStrc *pSpawned;
    const int32_t *pnShardDir;
    int32_t nMissileId;
    int32_t nLevel;
    int32_t nSkillId;
    int32_t nX;
    int32_t nY;
    int32_t nRow;
    int32_t nTurn;
    int32_t nDirection;
    int32_t iShard;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt)
        return 1;

    nX = UNITS_GetXPosition(pMissile);

    nY = UNITS_GetYPosition(pMissile);

    if ((int16_t)pMissilesTxt->wCltHitSubMissile[0] >= 0)
    {
        nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
        nMissileId = (int16_t)pMissilesTxt->wCltHitSubMissile[0];
        if (pMissile->dwUnitType == UNIT_PLAYER || pMissile->dwUnitType == UNIT_MONSTER)
            pOwner = pMissile;
        else
            pOwner = CUNIT_GetOwnerUnit(pMissile);
        if (pOwner)
        {
            memset(&tParams, 0, sizeof(tParams));
            tParams.pOwner = pOwner;
            tParams.dwFlags = MISSILESPAWN_TARGETPOS;
            tParams.pOrigin = pMissile;
            tParams.nTargetX = 0;
            tParams.nTargetY = 0;
            tParams.nSkill = nSkillId;
            tParams.nSkillLevel = nLevel;
            tParams.nMissile = nMissileId;
            MISSILE_ClientSpawn(&tParams);
        }
    }

    if ((int16_t)pMissilesTxt->wCltHitSubMissile[1] < 0)
        return 1;

    nRow = (int32_t)((uint32_t)SEED_RollRandomNumber(&pMissile->pSeed) % 3);

    nTurn = (int32_t)((uint32_t)SEED_RollRandomNumber(&pMissile->pSeed) & 7);

    if (gnGlacialSpikeBurstParams[5 * nRow] <= 0)
        return 1;

    pnShardDir = &gnGlacialSpikeBurstParams[5 * nRow + 1];
    iShard = 0;
    do
    {
        nDirection = (*pnShardDir + nTurn) & 7;
        nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
        nMissileId = (int16_t)pMissilesTxt->wCltHitSubMissile[1];
        if (pMissile->dwUnitType == UNIT_PLAYER || pMissile->dwUnitType == UNIT_MONSTER)
            pOwner = pMissile;
        else
            pOwner = CUNIT_GetOwnerUnit(pMissile);
        if (pOwner)
        {
            memset(&tParams, 0, sizeof(tParams));
            tParams.pOwner = pOwner;
            tParams.nSkill = nSkillId;
            tParams.dwFlags = MISSILESPAWN_TARGETPOS;
            tParams.pOrigin = pMissile;
            tParams.nTargetX = 0;
            tParams.nTargetY = 0;
            tParams.nSkillLevel = nLevel;
            tParams.nMissile = nMissileId;
            pSpawned = MISSILE_ClientSpawn(&tParams);
            if (pSpawned)
            {
                D2COMMON_PATH_FaceCoords(pSpawned->pDynamicPath, nX + gnDirection8OffsetX[nDirection], nY + gnDirection8OffsetY[nDirection], 0);
            }
        }
        ++iShard;
        ++pnShardDir;
    }
    while (iShard < gnGlacialSpikeBurstParams[5 * nRow]);

    return 1;
}


//1.10f: D2Client.0x6FB36DF0
//  pCltHitFunc 16: chainlightning, lightningstrike and chainlightning2 (ProgOverlay lightning; LastCollide 1 on all three) -- the chain's next jump.
//  Nothing, answering 1, without a record, an owner or a unit hit. The row's ProgOverlay, when above 0, is played once on the unit hit (kind 2);
//  then, while MISSILE_GetTargetX -- a count here, each bolt's one less than its parent's -- is above 1 (`cmp eax, 1 / jle`):
//    * the radius is cHitPar1 when above 0, else the skill's AuraRangeCalc for the owner, at least 1; no skill record: nothing more;
//    * SKILLS_FindNextNearbyTargetById, around the missile's position, for the owner, mask 0x88583 (FINDPLAYER, FINDMONSTER, FINDISATT, IGNINTOWN,
//      FINDISSEL, IGNALLY, IGNJUSTHIT), AFTER the unit hit's id -- the next unit by id, wrapping, not the nearest;
//    * a unit found that is not the unit hit gets a bolt: the same missile id, flags 0x21, from the missile's position to that unit's, with its skill
//      and level; its TargetX the count less one, and the unit hit handed to MISSILE_SetOwner -- which in D2Common (#11129) writes the unit's type
//      and id into the missile data only for a LastCollide row.
//  Answers 1.
//  The original inlines the Missiles.txt record lookup, UNITS_GetXPosition, UNITS_GetYPosition and DATATBLS_GetSkillsTxtRecordById here.
int32_t __fastcall MISSILE_CltHitFunc16(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2SkillsTxt *pSkillsTxt;
    D2UnitStrc *pOwner;
    D2UnitStrc *pNext;
    D2UnitStrc *pSpawned;
    int32_t nMissileId;
    int32_t nSkillId;
    int32_t nLevel;
    int32_t nCount;
    int32_t nRadius;
    int32_t nX;
    int32_t nY;
    int16_t nOverlayId;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt)
        return 1;

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner || !pTarget)
        return 1;

    nOverlayId = (int16_t)pMissilesTxt->wProgOverlay;
    if (nOverlayId > 0)
        GFX_AddOverlay(pTarget, (D2_OverlayIds)nOverlayId, OVERLAYTYPE_ONCE, 0, 0, 0, 0, 0);

    nCount = D2COMMON_MISSILE_GetTargetX(pMissile);
    if (nCount <= 1)
        return 1;

    nX = UNITS_GetXPosition(pMissile);

    nY = UNITS_GetYPosition(pMissile);

    nRadius = (int32_t)pMissilesTxt->dwCltHitPar[0];
    if (nRadius <= 0)
    {
        nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
        nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
        if (!pSkillsTxt)
            return 1;
        nRadius = D2COMMON_SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxt->dwAuraRangeCalc, (D2C_Skills)nSkillId, nLevel);
        if (nRadius < 1)
            nRadius = 1;
    }

    pNext = SKILLS_FindNextNearbyTargetById(pOwner, nX, nY, nRadius, nChainTargetMask, pTarget->dwUnitId, NULL);
    if (!pNext || pNext == pTarget)
        return 1;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETPOS;
    tParams.pOwner = pOwner;
    tParams.pTarget = pNext;
    tParams.nMissile = pMissile->dwClassId;
    tParams.nX = nX;
    tParams.nY = nY;

    tParams.nTargetX = UNITS_GetXPosition(pNext);

    tParams.nTargetY = UNITS_GetYPosition(pNext);

    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
    tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    pSpawned = MISSILE_ClientSpawn(&tParams);
    if (pSpawned)
    {
        D2COMMON_MISSILE_SetTargetX(pSpawned, nCount - 1);
        D2COMMON_MISSILE_SetOwner(pSpawned, pTarget);
    }

    return 1;
}


//1.10f: D2Client.0x6FB37060
//  pCltHitFunc 18: meteorcenter, vampiremeteorcenter and royalstrikemeteorcenter (CltHitSubMissile1..4 meteorexplode -- royalstrikemeteorexplode for
//  the third --, whitelightmissile, firemedium, firesmall) -- the meteor's impact. One block, cleared once, with the owner, the skill and level,
//  serves every piece:
//    * flags 0x4001: cHitPar1 pieces of CltHitSubMissile1 on a cross, gnMeteorCrossOffsetX/Y[i % 5] -- (0, 0), (4, 0), (0, 4), (-4, 0), (0, -4);
//    * flags 0x8001: one CltHitSubMissile2 at the impact, its range the skill's Param3 + (level - 1) * Param4 when the record is found and the level
//      is above 0 (`jg`), else 0; the light of the missile spawned, if it has one, gets radius 12 (DLIGHTMAP_SetRadius);
//    * flags 0xC001 from there on, whether or not the flash was spawned: cHitPar2 pieces of CltHitSubMissile3 on gnMeteorSplashOffsetX/Y[i % 18],
//      then cHitPar3 of CltHitSubMissile4 CONTINUING the same eighteen where the third stopped -- for the stock rows 3 firemedium on positions 0..2
//      and 15 firesmall on 3..17, every position once.
//  No record or no skill record -- looked up twice, the first answer only tested -- nothing. Answers 1. The fourth starts at the third's count as
//  read, kept when it is not above 0.
//  BUG: a negative cHitPar2 would index the splash table below its start (`idiv`, signed). UNREACHABLE: no stock row has one.
//  The original inlines the Missiles.txt record lookup, DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall MISSILE_CltHitFunc18(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2SkillsTxt *pSkillsTxt;
    D2UnitStrc *pOwner;
    D2UnitStrc *pSpawned;
    D2ClientLightStrc *pLight;
    int32_t nMissileId;
    int32_t nSkillId;
    int32_t nLevel;
    int32_t nX;
    int32_t nY;
    int32_t nCount;
    int32_t nPiece;
    int32_t nSplashNext;
    int32_t nSplashEnd;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt)
        return 1;

    nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
    nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    if (!DATATBLS_GetSkillsTxtRecordById(nSkillId))
        return 1;

    pOwner = CUNIT_GetOwnerUnit(pMissile);

    nX = UNITS_GetXPosition(pMissile);

    nY = UNITS_GetYPosition(pMissile);

    memset(&tParams, 0, sizeof(tParams));
    tParams.nSkill = nSkillId;
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_NOLIGHT;
    tParams.pOwner = pOwner;
    tParams.nSkillLevel = nLevel;

    if ((int16_t)pMissilesTxt->wCltHitSubMissile[0] >= 0)
    {
        tParams.nMissile = (int16_t)pMissilesTxt->wCltHitSubMissile[0];
        nCount = (int32_t)pMissilesTxt->dwCltHitPar[0];
        for (nPiece = 0; nPiece < nCount; ++nPiece)
        {
            tParams.nY = nY + gnMeteorCrossOffsetY[nPiece % 5];
            tParams.nX = nX + gnMeteorCrossOffsetX[nPiece % 5];
            MISSILE_ClientSpawn(&tParams);
        }
    }

    if ((int16_t)pMissilesTxt->wCltHitSubMissile[1] >= 0)
    {
        tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_RANGE;
        tParams.nMissile = (int16_t)pMissilesTxt->wCltHitSubMissile[1];
        pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
        if (pSkillsTxt && nLevel > 0)
            tParams.nRange = pSkillsTxt->dwParam[3] * (nLevel - 1) + pSkillsTxt->dwParam[2];
        else
            tParams.nRange = 0;
        tParams.nX = nX;
        tParams.nY = nY;
        pSpawned = MISSILE_ClientSpawn(&tParams);
        if (pSpawned)
        {
            pLight = pSpawned->pLight;
            if (pLight)
                DLIGHTMAP_SetRadius(pLight, 12);
        }
    }

    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_NOLIGHT | MISSILESPAWN_RANGE;
    nSplashNext = 0;
    if ((int16_t)pMissilesTxt->wCltHitSubMissile[2] >= 0)
    {
        tParams.nMissile = (int16_t)pMissilesTxt->wCltHitSubMissile[2];
        nSplashNext = (int32_t)pMissilesTxt->dwCltHitPar[1];
        for (nPiece = 0; nPiece < nSplashNext; ++nPiece)
        {
            tParams.nY = nY + gnMeteorSplashOffsetY[nPiece % 18];
            tParams.nX = nX + gnMeteorSplashOffsetX[nPiece % 18];
            MISSILE_ClientSpawn(&tParams);
        }
    }

    if ((int16_t)pMissilesTxt->wCltHitSubMissile[3] >= 0)
    {
        tParams.nMissile = (int16_t)pMissilesTxt->wCltHitSubMissile[3];
        nSplashEnd = (int32_t)pMissilesTxt->dwCltHitPar[2] + nSplashNext;
        for (; nSplashNext < nSplashEnd; ++nSplashNext)
        {
            tParams.nY = nY + gnMeteorSplashOffsetY[nSplashNext % 18];
            tParams.nX = nX + gnMeteorSplashOffsetX[nSplashNext % 18];
            MISSILE_ClientSpawn(&tParams);
        }
    }

    return 1;
}


//1.10f: D2Client.0x6FB37330
//  pCltHitFunc 19: monbliz1..4, blizzard1..4 (CltHitSubMissile1..2 monblizexplode1 and 3, blizzardexplode1 and 3) and andyfallingdebris1..3
//  (andydebrisexplosion1 and 5) -- an explosion picked from a range of ids: CltHitSubMissile1, plus a roll below (second - first + 1) when the second
//  is greater, tested as words (`cmp ax, cx / jle`) and again as dwords (`cmp edi, eax / jge`) -- pCltHitFunc 3's second spawn with the columns one
//  to the left. Spawned once, flags 0x20, origin the missile, target (0, 0), with its skill and level, for the missile itself if a player or a
//  monster and else for its owner (none: nothing). No record or a negative CltHitSubMissile1: nothing. Answers 1.
//  The original inlines the Missiles.txt record lookup and SEED_RollLimitedRandomNumber here.
int32_t __fastcall MISSILE_CltHitFunc19(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pOwner;
    int32_t nMissileId;
    int32_t nLo;
    int32_t nHi;
    int32_t nLevel;
    int32_t nSkillId;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltHitSubMissile[0] < 0)
        return 1;

    nLo = (int16_t)pMissilesTxt->wCltHitSubMissile[0];
    nHi = (int16_t)pMissilesTxt->wCltHitSubMissile[1];
    nMissileId = nLo;
    if (nHi > nLo && nLo < nHi)
    {
        nMissileId = nLo + SEED_RollLimitedRandomNumber(&pMissile->pSeed, nHi - nLo + 1);
    }

    nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
    if (pMissile->dwUnitType == UNIT_PLAYER || pMissile->dwUnitType == UNIT_MONSTER)
        pOwner = pMissile;
    else
        pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
        return 1;

    memset(&tParams, 0, sizeof(tParams));
    tParams.nTargetX = 0;
    tParams.nTargetY = 0;
    tParams.dwFlags = MISSILESPAWN_TARGETPOS;
    tParams.pOwner = pOwner;
    tParams.pOrigin = pMissile;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nLevel;
    tParams.nMissile = nMissileId;
    MISSILE_ClientSpawn(&tParams);
    return 1;
}


//1.10f: D2Client.0x6FB37470
//  pCltHitFunc 24: bonespear and viper_bonespear (CltHitSubMissile1 teethexplode) -- the spear's splinters on the unit it passes. With a unit hit
//  (tested first), a record and a CltHitSubMissile1 not negative, one CltHitSubMissile1 is spawned as pCltHitFunc 19's piece is (flags 0x20, target
//  (0, 0), the missile or its owner), and it ANSWERS 0 -- the hit is not taken, and MISSILE_ClientOnHit answers 0 at once. It answers 0 as well when
//  there is no owner and nothing is spawned. Without a unit hit, a record or the id: 1.
//  The original inlines the Missiles.txt record lookup here.
int32_t __fastcall MISSILE_CltHitFunc24(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pOwner;
    int32_t nMissileId;
    int32_t nLevel;
    int32_t nSkillId;

    if (!pTarget)
        return 1;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltHitSubMissile[0] < 0)
        return 1;

    nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
    nMissileId = (int16_t)pMissilesTxt->wCltHitSubMissile[0];
    if (pMissile->dwUnitType == UNIT_PLAYER || pMissile->dwUnitType == UNIT_MONSTER)
        pOwner = pMissile;
    else
        pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
        return 0;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_TARGETPOS;
    tParams.nMissile = nMissileId;
    tParams.pOwner = pOwner;
    tParams.pOrigin = pMissile;
    tParams.nTargetX = 0;
    tParams.nTargetY = 0;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nLevel;
    MISSILE_ClientSpawn(&tParams);
    return 0;
}


//  The default target masks of pCltHitFunc 25 and 26 (`mov ecx, 0A783h`, `mov eax, 0A683h`); the second lacks IGNINTOWN.
static const D2C_AuraFilters nFuryDefaultMask = (D2C_AuraFilters)(AURAFILTER_FINDPLAYER | AURAFILTER_FINDMONSTER | AURAFILTER_FINDISATT | AURAFILTER_IGNINTOWN | AURAFILTER_USELOS | AURAFILTER_FINDISSEL | AURAFILTER_IGNINTOWN2 | AURAFILTER_IGNALLY);
static const D2C_AuraFilters nFistDefaultMask = (D2C_AuraFilters)(AURAFILTER_FINDPLAYER | AURAFILTER_FINDMONSTER | AURAFILTER_FINDISATT | AURAFILTER_USELOS | AURAFILTER_FINDISSEL | AURAFILTER_IGNINTOWN2 | AURAFILTER_IGNALLY);
static_assert(nFuryDefaultMask == 0xA783 && nFistDefaultMask == 0xA683,
              "`mov ecx, 0A783h` (0x6FB376E3), `mov eax, 0A683h` (0x6FB379A0)");

//1.10f: D2Client.0x6FB37560
//  pCltHitFunc 25: lightningfury alone (CltHitSubMissile1 furylightning) -- the bolts Lightning Fury forks to the units around its burst. With a
//  record, a CltHitSubMissile1 not negative, a skill record and an owner: the missile's position; a radius of cHitPar1 when above 0, else the skill's
//  AuraRangeCalc for the owner, at least 1; a cap of cHitPar2 when above 0, else its Calc1, at least 1; the skill's AuraFilter, or 0xA783 when that
//  is 0; and SKILLS_ForEachNearbyTarget, NOAURA monsters skipped (its last argument 1), hands MISSILE_SpawnOnUnitCallback each unit it accepts with a
//  block of the owner, the missile, the skill, the level, the cap and the id. Answers 1; it never reads the unit hit.
//  The original inlines the Missiles.txt record lookup, DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall MISSILE_CltHitFunc25(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2ClientSubMissileSpawnStrc tUserData;
    D2MissilesTxt *pMissilesTxt;
    D2SkillsTxt *pSkillsTxt;
    D2UnitStrc *pOwner;
    D2C_AuraFilters nMask;
    int32_t nMissileId;
    int32_t nSkillId;
    int32_t nLevel;
    int32_t nX;
    int32_t nY;
    int32_t nRadius;
    int32_t nMaxTargets;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltHitSubMissile[0] < 0)
        return 1;

    nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
    nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 1;

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
        return 1;

    nX = UNITS_GetXPosition(pMissile);

    nY = UNITS_GetYPosition(pMissile);

    nRadius = (int32_t)pMissilesTxt->dwCltHitPar[0];
    if (nRadius <= 0)
    {
        nRadius = D2COMMON_SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxt->dwAuraRangeCalc, (D2C_Skills)nSkillId, nLevel);
        if (nRadius < 1)
            nRadius = 1;
    }

    nMaxTargets = (int32_t)pMissilesTxt->dwCltHitPar[1];
    if (nMaxTargets <= 0)
    {
        nMaxTargets = D2COMMON_SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxt->dwCalc[0], (D2C_Skills)nSkillId, nLevel);
        if (nMaxTargets < 1)
            nMaxTargets = 1;
    }

    nMask = (D2C_AuraFilters)pSkillsTxt->dwAuraFilter;
    if (!nMask)
        nMask = nFuryDefaultMask;

    tUserData.pOwner = pOwner;
    tUserData.pMissile = pMissile;
    tUserData.nSkillId = nSkillId;
    tUserData.nSkillLevel = nLevel;
    tUserData.nMaxTargets = nMaxTargets;
    tUserData.nSubMissileId = (int16_t)pMissilesTxt->wCltHitSubMissile[0];
    SKILLS_ForEachNearbyTarget(pOwner, nX, nY, nRadius, nMask, MISSILE_SpawnOnUnitCallback, &tUserData, TRUE);
    return 1;
}


//1.10f: D2Client.0x6FB37740
//  The visitor pCltHitFunc 25 and 26 hand SKILLS_ForEachNearbyTarget. Answers 0, and spawns nothing, once the count of units accepted so far -- the
//  context's nAccepted, which the iterator keeps -- has reached the block's nMaxTargets (`cmp eax, [edx+10h] / jl`, signed); else one of the block's
//  sub-missile, flags 0x20, for its owner, origin its missile, with its skill and level, aimed at the unit visited, and answers 1, which the iterator
//  counts.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall MISSILE_SpawnOnUnitCallback(D2ClientNearbyTargetCtxStrc *pCtx, D2UnitStrc *pUnit)
{
    D2MissileStrc tParams;
    D2ClientSubMissileSpawnStrc *pUserData;

    pUserData = (D2ClientSubMissileSpawnStrc *)pCtx->pUserData;
    if (pCtx->nAccepted >= pUserData->nMaxTargets)
        return 0;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_TARGETPOS;
    tParams.pOwner = pUserData->pOwner;
    tParams.pOrigin = pUserData->pMissile;
    tParams.nSkill = pUserData->nSkillId;
    tParams.nSkillLevel = pUserData->nSkillLevel;
    tParams.nMissile = pUserData->nSubMissileId;

    tParams.nTargetX = UNITS_GetXPosition(pUnit);

    tParams.nTargetY = UNITS_GetYPosition(pUnit);

    MISSILE_ClientSpawn(&tParams);
    return 1;
}


//1.10f: D2Client.0x6FB37800
//  pCltHitFunc 26: fistoftheheavensdelay alone (CltHitSubMissile1 fistoftheheavensbolt) -- the holy bolts Fist of the Heavens throws at the units
//  around its strike. pCltHitFunc 25's shape, and where it differs:
//    * the gate is HitSubMissile1, the SERVER's column (`cmp word ptr [eax+24h], 0`), where the id spawned is CltHitSubMissile1 (`movsx ecx, word ptr
//      [edx+2Ch]`); the stock row sets both to fistoftheheavensbolt. The owner is asked before the skill;
//    * the unit struck is found again from the missile -- TargetX its type, TargetY its id (CUNIT_FindUnitByGuidAndType) -- and when it is gone the
//      handler ANSWERS 0, so ClientOnHit does;
//    * NOAURA monsters are not skipped (the last argument 0).
//  Answers 1.
//  The original inlines the Missiles.txt record lookup, DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall MISSILE_CltHitFunc26(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2ClientSubMissileSpawnStrc tUserData;
    D2MissilesTxt *pMissilesTxt;
    D2SkillsTxt *pSkillsTxt;
    D2UnitStrc *pOwner;
    D2C_AuraFilters nMask;
    int32_t nMissileId;
    int32_t nSkillId;
    int32_t nLevel;
    int32_t nUnitType;
    int32_t nUnitId;
    int32_t nX;
    int32_t nY;
    int32_t nRadius;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wHitSubMissile[0] < 0)
        return 1;

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
        return 1;

    nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
    nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 1;

    nUnitType = D2COMMON_MISSILE_GetTargetX(pMissile);
    nUnitId = D2COMMON_MISSILE_GetTargetY(pMissile);
    if (!CUNIT_FindUnitByGuidAndType((uint32_t)nUnitId, (D2C_UnitTypes)nUnitType))
        return 0;

    nX = UNITS_GetXPosition(pMissile);

    nY = UNITS_GetYPosition(pMissile);

    nRadius = (int32_t)pMissilesTxt->dwCltHitPar[0];
    if (nRadius <= 0)
    {
        nRadius = D2COMMON_SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxt->dwAuraRangeCalc, (D2C_Skills)nSkillId, nLevel);
        if (nRadius < 1)
            nRadius = 1;
    }

    //  The cap the server keeps, evaluated and dropped.
    if ((int32_t)pMissilesTxt->dwCltHitPar[1] <= 0)
        D2COMMON_SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxt->dwCalc[3], (D2C_Skills)nSkillId, nLevel);

    //  BUG: the mask test is the server's inverted: a ZERO AuraFilter is kept and any other replaced with 0xA683 (the server keeps a non-zero one).
    //  REACHABLE: Fist of the Heavens carries 42375 (0xA587), so the client searches with 0xA683 -- USELOS added, FINDUNDEAD and IGNINTOWN gone.
    nMask = (D2C_AuraFilters)pSkillsTxt->dwAuraFilter;
    if (nMask)
        nMask = nFistDefaultMask;

    //  BUG: tUserData.nMaxTargets is never stored, and the visitor compares its count with whatever the stack holds there; with
    //  cHitPar2 not above 0 the skill's Calc4 is evaluated and dropped. REACHABLE: the stock row reaches it.
    //  DIFFERENCE: our field is left unwritten too, but our stack holds different garbage from the original's; the block's address
    //  goes to SKILLS_ForEachNearbyTarget in another translation unit, so the compiler cannot treat the read as undefined.
    tUserData.pOwner = pOwner;
    tUserData.pMissile = pMissile;
    tUserData.nSkillId = nSkillId;
    tUserData.nSkillLevel = nLevel;
    tUserData.nSubMissileId = (int16_t)pMissilesTxt->wCltHitSubMissile[0];
    SKILLS_ForEachNearbyTarget(pOwner, nX, nY, nRadius, nMask, MISSILE_SpawnOnUnitCallback, &tUserData, FALSE);
    return 1;
}


//1.10f: D2Client.0x6FB379F0
//  A ring of a missile around a unit -- pCltHitFunc 28's, its one caller. For an id with a record (else it answers 0): one block, flags 0x1F, the
//  owner, origin pOrigin, the id, pOrigin's position, the skill, the level, nLoops the level less one and a velocity of the record's Param1 << 7,
//  spawned at every nStep-th of the eight gaMissileRingOffsetX/Y as its target -- (0, 2), (2, 2), (2, 0), (2, -2), (0, -2), (-2, -2), (-2, 0), (-2,
//  2) -- a byte offset stepped by 4 * nStep below 0x20, the first spawned before any test. Answers 1; the caller discards the answer. A step not
//  above 0 would never leave the loop; the caller floors it at 1. The fourth parameter is stored as the block's skill.
//  The original inlines the Missiles.txt record lookup, UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall MISSILE_SpawnRingOfMissiles(D2UnitStrc *pOwner, D2UnitStrc *pOrigin, int32_t nMissileId, int32_t nSkillId, int32_t nLevel, int32_t nStep)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    int32_t nOffset;

    pMissilesTxt = NULL;
    if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
        pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    if (!pMissilesTxt)
        return 0;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETOFFSET | MISSILESPAWN_VELOCITY | MISSILESPAWN_SUBLOOPS | MISSILESPAWN_VELOCITYSCALED;
    tParams.pOwner = pOwner;
    tParams.pOrigin = pOrigin;
    tParams.nMissile = nMissileId;

    tParams.nX = UNITS_GetXPosition(pOrigin);

    tParams.nY = UNITS_GetYPosition(pOrigin);

    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nLevel;
    tParams.nLoops = nLevel - 1;
    tParams.nVelocity = pMissilesTxt->dwParam[0] << 7;
    for (nOffset = 0; nOffset < 0x20; nOffset += 4 * nStep)
    {
        tParams.nTargetX = gaMissileRingOffsetX[nOffset / 4];
        tParams.nTargetY = gaMissileRingOffsetY[nOffset / 4];
        MISSILE_ClientSpawn(&tParams);
    }

    return 1;
}


//1.10f: D2Client.0x6FB37B20
//  pCltHitFunc 28: pantherpotgreen alone (CltHitSubMissile1 rancidgascloud) -- the green panther pot's ring of gas. With a record, a
//  CltHitSubMissile1 not negative and an owner: MISSILE_SpawnRingOfMissiles for the owner, around the unit hit -- the missile itself when none (`test
//  ebp, ebp / jnz`) --, of CltHitSubMissile1, every max(cHitPar1, 1)-th direction (the row's cHitPar1 is blank: all eight), with the missile's skill
//  and level; the level is asked first. Answers 1.
//  The original inlines the Missiles.txt record lookup here.
int32_t __fastcall MISSILE_CltHitFunc28(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pOwner;
    D2UnitStrc *pOrigin;
    int32_t nMissileId;
    int32_t nStep;
    int32_t nLevel;
    int32_t nSkillId;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltHitSubMissile[0] < 0)
        return 1;

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
        return 1;

    pOrigin = pTarget;
    if (!pOrigin)
        pOrigin = pMissile;
    nStep = (int32_t)pMissilesTxt->dwCltHitPar[0];
    if (nStep < 1)
        nStep = 1;
    nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
    MISSILE_SpawnRingOfMissiles(pOwner, pOrigin, (int16_t)pMissilesTxt->wCltHitSubMissile[0], nSkillId, nLevel, nStep);
    return 1;
}


//1.10f: D2Client.0x6FB37BB0
//  pCltHitFunc 29: the six grim ward rows -- grimwardsmall, medium and large and their -start rows, each row's CltHitSubMissile1 the next of its
//  chain (grimwardsmallstart -> grimwardsmall -> grimwardsmallstop) -- the next stage of a Grim Ward. With a record, a CltHitSubMissile1 not
//  negative, an owner and a skill record: one CltHitSubMissile1 at the missile's position, flags 1, for the owner, and turned to the missile's
//  direction (PATH_GetDirection of pMissile->pDynamicPath, read with no type test). Its range is cHitPar1 when that is not ZERO (`test ecx, ecx /
//  jnz` -- a negative column is taken as it is), else the skill's Calc1, at least 5; a range above 0 is stored with flag 0x8000 (`or ch, 80h` on the
//  flags' dword). Answers 1.
//  The original inlines the Missiles.txt record lookup, DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall MISSILE_CltHitFunc29(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2SkillsTxt *pSkillsTxt;
    D2UnitStrc *pOwner;
    D2UnitStrc *pSpawned;
    int32_t nMissileId;
    int32_t nSkillId;
    int32_t nLevel;
    int32_t nRange;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltHitSubMissile[0] < 0)
        return 1;

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
        return 1;

    nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
    nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 1;

    nRange = (int32_t)pMissilesTxt->dwCltHitPar[0];
    if (!nRange)
    {
        nRange = D2COMMON_SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxt->dwCalc[0], (D2C_Skills)nSkillId, nLevel);
        if (nRange < 5)
            nRange = 5;
    }

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS;
    tParams.pOwner = pOwner;
    tParams.nMissile = (int16_t)pMissilesTxt->wCltHitSubMissile[0];
    //  BUG: the level is stored into nSkill as well as nSkillLevel (the skill id was in esi). REACHABLE: every spawn of this handler stores it.
    tParams.nSkill = nLevel;
    tParams.nSkillLevel = nLevel;

    tParams.nX = UNITS_GetXPosition(pMissile);

    tParams.nY = UNITS_GetYPosition(pMissile);

    if (nRange > 0)
    {
        tParams.nRange = nRange;
        tParams.dwFlags |= MISSILESPAWN_RANGE;
    }

    pSpawned = MISSILE_ClientSpawn(&tParams);
    if (pSpawned)
        D2COMMON_PATH_SetDirection(pSpawned->pDynamicPath, D2COMMON_PATH_GetDirection(pMissile->pDynamicPath));

    return 1;
}


//1.10f: D2Client.0x6FB37D70
//  pCltHitFunc 30: frozenorb alone (cHitPar1 4; CltHitSubMissile1 frozenorbnova) -- the orb's closing burst. With a record, a CltHitSubMissile1 not
//  negative and an owner: one block, flags 2, the owner, origin the missile, its skill and level, the id; then every max(cHitPar1, 1)-th of the 64
//  points of gnFrozenOrbRingX/Y -- a circle of radius 28.8 to 30.0 -- as the target, a byte offset stepped by 4 * the stride below 0x100, and each
//  piece spawned gets that point again as its TargetX/Y. Answers 1. The burst happens on any hit. pCltHitFunc 4, next, is this function over a second
//  copy of the ring.
//  The original inlines the Missiles.txt record lookup here.
int32_t __fastcall MISSILE_CltHitFunc30(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pOwner;
    D2UnitStrc *pSpawned;
    int32_t nMissileId;
    int32_t nStride;
    int32_t nOffset;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltHitSubMissile[0] < 0)
        return 1;

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
        return 1;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_TARGETOFFSET;
    tParams.pOwner = pOwner;
    tParams.pOrigin = pMissile;
    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
    tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    tParams.nMissile = (int16_t)pMissilesTxt->wCltHitSubMissile[0];
    nStride = (int32_t)pMissilesTxt->dwCltHitPar[0];
    if (nStride < 1)
        nStride = 1;
    for (nOffset = 0; nOffset < 0x100; nOffset += 4 * nStride)
    {
        tParams.nTargetX = gnFrozenOrbRingX[nOffset / 4];
        tParams.nTargetY = gnFrozenOrbRingY[nOffset / 4];
        pSpawned = MISSILE_ClientSpawn(&tParams);
        if (pSpawned)
        {
            D2COMMON_MISSILE_SetTargetX(pSpawned, gnFrozenOrbRingX[nOffset / 4]);
            D2COMMON_MISSILE_SetTargetY(pSpawned, gnFrozenOrbRingY[nOffset / 4]);
        }
    }

    return 1;
}


//1.10f: D2Client.0x6FB37E80
//  pCltHitFunc 4 -- no stock row carries it. pCltHitFunc 30 instruction for instruction, over gnHitFunc4RingX/Y, a second copy of the same 64 points.
//  Two definitions, written as the original has them: the linker did not fold identical functions.
//  The original inlines the Missiles.txt record lookup here.
int32_t __fastcall MISSILE_CltHitFunc4(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pOwner;
    D2UnitStrc *pSpawned;
    int32_t nMissileId;
    int32_t nStride;
    int32_t nOffset;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltHitSubMissile[0] < 0)
        return 1;

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
        return 1;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_TARGETOFFSET;
    tParams.pOwner = pOwner;
    tParams.pOrigin = pMissile;
    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
    tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    tParams.nMissile = (int16_t)pMissilesTxt->wCltHitSubMissile[0];
    nStride = (int32_t)pMissilesTxt->dwCltHitPar[0];
    if (nStride < 1)
        nStride = 1;
    for (nOffset = 0; nOffset < 0x100; nOffset += 4 * nStride)
    {
        tParams.nTargetX = gnHitFunc4RingX[nOffset / 4];
        tParams.nTargetY = gnHitFunc4RingY[nOffset / 4];
        pSpawned = MISSILE_ClientSpawn(&tParams);
        if (pSpawned)
        {
            D2COMMON_MISSILE_SetTargetX(pSpawned, gnHitFunc4RingX[nOffset / 4]);
            D2COMMON_MISSILE_SetTargetY(pSpawned, gnHitFunc4RingY[nOffset / 4]);
        }
    }

    return 1;
}

static_assert(OVERLAY_STEALLIFE == 151, "MISSILE_CltHitFunc32: `mov edx, 97h` (0x6FB3810F)");


//1.10f: D2Client.0x6FB37F90
//  pCltHitFunc 32: firehead alone (cHitPar1..2 of 1 and 2; CltHitSubMissile1..2 explodingarrowexp and fireexplosion2; ProgOverlay vampiresteal) --
//  the fire head's bite. With a record:
//    * CltHitSubMissile1, when not negative, spawned once, flags 0x20, target (0, 0), with the skill and level, for the missile itself if a player or
//      a monster and else for its owner (none: this spawn only skipped);
//    * CltHitSubMissile2, when not negative, through MISSILE_SpawnOverArea at the missile's position (Y read before X), radius cHitPar1, one in
//      cHitPar2, step 1, with the same skill and level;
//    * with a unit hit, overlay 151 once on it (`mov edx, 97h`) -- steallife, CurseRevVampireHit;
//    * with an owner (asked again) and a unit hit, the row's ProgOverlay, vampiresteal, once on the OWNER.
//  Answers 1.
//  The original inlines the Missiles.txt record lookup, UNITS_GetYPosition and UNITS_GetXPosition here.
int32_t __fastcall MISSILE_CltHitFunc32(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pOwner;
    int32_t nMissileId;
    int32_t nSkillId;
    int32_t nLevel;
    int32_t nX;
    int32_t nY;
    int16_t nOverlayId;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt)
        return 1;

    nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
    nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);

    if ((int16_t)pMissilesTxt->wCltHitSubMissile[0] >= 0)
    {
        nMissileId = (int16_t)pMissilesTxt->wCltHitSubMissile[0];
        if (pMissile->dwUnitType == UNIT_PLAYER || pMissile->dwUnitType == UNIT_MONSTER)
            pOwner = pMissile;
        else
            pOwner = CUNIT_GetOwnerUnit(pMissile);
        if (pOwner)
        {
            memset(&tParams, 0, sizeof(tParams));
            tParams.nTargetX = 0;
            tParams.nTargetY = 0;
            tParams.nSkillLevel = nLevel;
            tParams.dwFlags = MISSILESPAWN_TARGETPOS;
            tParams.pOwner = pOwner;
            tParams.pOrigin = pMissile;
            tParams.nSkill = nSkillId;
            tParams.nMissile = nMissileId;
            MISSILE_ClientSpawn(&tParams);
        }
    }

    if ((int16_t)pMissilesTxt->wCltHitSubMissile[1] >= 0)
    {
        nY = UNITS_GetYPosition(pMissile);

        nX = UNITS_GetXPosition(pMissile);

        MISSILE_SpawnOverArea(pMissile, nX, nY, (int32_t)pMissilesTxt->dwCltHitPar[0], (int16_t)pMissilesTxt->wCltHitSubMissile[1], nSkillId, nLevel, (int32_t)pMissilesTxt->dwCltHitPar[1], 1);
    }

    //  Overlay.txt row 151, steallife.
    if (pTarget)
        GFX_AddOverlay(pTarget, OVERLAY_STEALLIFE, OVERLAYTYPE_ONCE, 0, 0, 0, 0, 0);

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (pOwner && pTarget)
    {
        nOverlayId = (int16_t)pMissilesTxt->wProgOverlay;
        if (nOverlayId > 0)
            GFX_AddOverlay(pOwner, (D2_OverlayIds)nOverlayId, OVERLAYTYPE_ONCE, 0, 0, 0, 0, 0);
    }

    return 1;
}


//1.10f: D2Client.0x6FB38160
//  pCltHitFunc 33: mephistoflyingrocksbig alone (cHitPar1 20; CltHitSubMissile1 mephistoflyingrockssmall) -- the big rock breaking into small ones.
//  With a record, a CltHitSubMissile1 not negative and an owner: one block, flags 0x102, the id, the owner, origin the missile, its skill; a spread
//  of cHitPar1, at least 3 (`cmp ebx, 3 / jge`); and MISSILE_GetTargetX -- a count here -- pieces (none when it is not above 0), each drawing on the
//  missile's seed:
//    * its level: the low dword `% 5`, plus 1 -- 1 to 5;
//    * its TargetX and TargetY: a roll below 2 * spread + 1, less the spread -- -spread to +spread each;
//    * and, when both came out 0, TargetX again: a roll below the spread, plus 1 -- so no piece is aimed at (0, 0).
//  Answers 1.
//  The original inlines the Missiles.txt record lookup, SEED_RollRandomNumber and SEED_RollLimitedRandomNumber here.
int32_t __fastcall MISSILE_CltHitFunc33(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pOwner;
    int32_t nMissileId;
    int32_t nSpread;
    int32_t nCount;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltHitSubMissile[0] < 0)
        return 1;

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
        return 1;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_TARGETOFFSET | MISSILESPAWN_ARC;
    tParams.nMissile = (int16_t)pMissilesTxt->wCltHitSubMissile[0];
    tParams.pOwner = pOwner;
    tParams.pOrigin = pMissile;
    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
    nSpread = (int32_t)pMissilesTxt->dwCltHitPar[0];
    if (nSpread < 3)
        nSpread = 3;
    nCount = D2COMMON_MISSILE_GetTargetX(pMissile);
    if (nCount <= 0)
        return 1;

    do
    {
        tParams.nSkillLevel = (int32_t)((uint32_t)SEED_RollRandomNumber(&pMissile->pSeed) % 5) + 1;

        tParams.nTargetX = SEED_RollLimitedRandomNumber(&pMissile->pSeed, 2 * nSpread + 1) - nSpread;

        tParams.nTargetY = SEED_RollLimitedRandomNumber(&pMissile->pSeed, 2 * nSpread + 1) - nSpread;

        if (!tParams.nTargetX && !tParams.nTargetY)
        {
            tParams.nTargetX = SEED_RollLimitedRandomNumber(&pMissile->pSeed, nSpread) + 1;
        }

        MISSILE_ClientSpawn(&tParams);
    }
    while (--nCount);

    return 1;
}


//1.10f: D2Client.0x6FB38360
//  pCltHitFunc 34: mephistodonotdraw alone (pCltDoFunc 28) -- the missile that hides Mephisto: on a hit its owner, when it has one, takes
//  UNITFLAGEX_NODRAW (`or dword ptr [eax+0C8h], 40000h`). Answers 1.
int32_t __fastcall MISSILE_CltHitFunc34(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2UnitStrc *pOwner;

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (pOwner)
        pOwner->dwFlagEx |= UNITFLAGEX_NODRAW;

    return 1;
}


//1.10f: D2Client.0x6FB38380
//  pCltHitFunc 36: hellmeteordown alone (Param1 its radius, blank in the stock row) -- the fire a falling meteor leaves. It ANSWERS 0 when it hit a
//  unit (`test edx, edx`) and does nothing more. Else, where D2Common's COLLISION_CheckMaskWithSize finds no wall at the missile's position (size 3,
//  mask 1; its answer a word, `test ax, ax`), MISSILE_SprayRandomOverArea scatters gaFireSprayMissiles, 3 entries, over a radius of Param1, range 25
//  and chance 75.
//  No record: nothing. Answers 1. pMissile is not tested -- its type is read first and MISSILE_ClientOnHit has tested it -- and the record is looked
//  up only after the collision test.
//  The original inlines UNITS_GetYPosition, UNITS_GetXPosition and the Missiles.txt record lookup here.
int32_t __fastcall MISSILE_CltHitFunc36(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissilesTxt *pMissilesTxt;
    int32_t nMissileId;
    int32_t nX;
    int32_t nY;

    if (pTarget)
        return 0;

    nY = UNITS_GetYPosition(pMissile);

    nX = UNITS_GetXPosition(pMissile);

    if (!D2COMMON_COLLISION_CheckMaskWithSize(D2COMMON_UNITS_GetRoom(pMissile), nX, nY, 3, COLLIDE_WALL))
    {
        pMissilesTxt = NULL;
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
        if (pMissilesTxt)
            MISSILE_SprayRandomOverArea(pMissile, gaFireSprayMissiles, 3, pMissilesTxt->dwParam[0], 25, 75);
    }

    return 1;
}


//1.10f: D2Client.0x6FB38450
//  pCltHitFunc 44: bomb, shock field, catapult spike and sentryspike "in air" (CltHitSubMissile1 their "on ground"), death mauler trail and
//  baalcorpseexplodeexpl (their fades) -- a missile landing. It ANSWERS 0 when it hit a unit (tested before the missile). Else, with a record and a
//  CltHitSubMissile1 not negative: one CltHitSubMissile1, flags 0x20, target (0, 0), with the skill and level -- the level asked first --, for the
//  missile itself if a player or a monster and else for its owner (none: nothing); the piece spawned takes the missile's direction, its path's rotate
//  flag cleared (D2Common #10203, bReset 0), and the missile's TargetX. Answers 1.
//  The original inlines the Missiles.txt record lookup here.
int32_t __fastcall MISSILE_CltHitFunc44(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pOwner;
    D2UnitStrc *pSpawned;
    D2DynamicPathStrc *pPath;
    int32_t nMissileId;
    int32_t nLevel;
    int32_t nSkillId;

    if (pTarget)
        return 0;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltHitSubMissile[0] < 0)
        return 1;

    nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
    nMissileId = (int16_t)pMissilesTxt->wCltHitSubMissile[0];
    if (pMissile->dwUnitType == UNIT_PLAYER || pMissile->dwUnitType == UNIT_MONSTER)
        pOwner = pMissile;
    else
        pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
        return 1;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_TARGETPOS;
    tParams.nMissile = nMissileId;
    tParams.pOwner = pOwner;
    tParams.pOrigin = pMissile;
    tParams.nTargetX = 0;
    tParams.nTargetY = 0;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nLevel;
    pSpawned = MISSILE_ClientSpawn(&tParams);
    if (pSpawned)
    {
        pPath = pSpawned->pDynamicPath;
        D2COMMON_PATH_SetDirection(pPath, D2COMMON_PATH_GetDirection(pMissile->pDynamicPath));
        D2COMMON_10203_PATH_SetRotateFlag(pPath, FALSE);
        D2COMMON_MISSILE_SetTargetX(pSpawned, D2COMMON_MISSILE_GetTargetX(pMissile));
    }

    return 1;
}


//1.10f: D2Client.0x6FB38570
//  The charged ball's bolts' initialiser: pCltHitFunc 46's block stores it, and MISSILE_ClientSpawn calls it with the block's argument; it reads only
//  ecx.
//  Nothing without a missile. The count of steps is the missile's total frames, at most 255 (`cmp eax, 0FFh / jg`, signed); the seed's low dword is
//  set from the first point of the missile's path (#10175) -- no seed argument, unlike SKILLS_MissileInitClientNovaBolt and
//  SKILLS_MissileStartSeededPath --; and the path becomes a charged bolt's (PATHTYPE_CHARGEDBOLT, 10) of that many steps, and starts. D2Common's
//  PATH_SetNewDistance takes the count as a byte; the original pushes the whole register.
void __fastcall MISSILE_StartSeededPathFromOrigin(D2UnitStrc *pMissile)
{
    D2DynamicPathStrc *pPath;
    int32_t nFrames;
    int32_t nSteps;

    if (!pMissile)
        return;

    nFrames = D2COMMON_MISSILE_GetTotalFrames(pMissile);
    nSteps = 255;
    if (nFrames <= 255)
        nSteps = nFrames;

    pPath = pMissile->pDynamicPath;
    D2COMMON_SEED_InitLowSeed(&pMissile->pSeed, D2COMMON_10175_PathGetFirstPointX(pPath));
    D2COMMON_PATH_SetType(pPath, PATHTYPE_CHARGEDBOLT);
    D2COMMON_PATH_SetNewDistance(pPath, (uint8_t)nSteps);
    D2COMMON_PATH_StartUnitMove(pPath, pMissile, 0);
}


//1.10f: D2Client.0x6FB385C0
//  pCltHitFunc 46: catapultchargedball alone (HitSubMissile1 and CltHitSubMissile1 both catapultchargedballbolt) -- the charged ball's bolts. It
//  gates on and spawns HitSubMissile1, the SERVER's column. With a record and an owner: a block, flags 3, the missile's position, the owner, the id,
//  the skill and level, and MISSILE_StartSeededPathFromOrigin as its initialiser; a count of cHitPar1 + (level - 1) * cHitPar2, else, not above 0,
//  the skill's Calc4 for the owner, at least 1 -- with no skill record nothing is spawned --; and SKILLS_SpawnMissileArray spawns that many. The
//  skill and the level are asked again for the formula. Answers 1.
//  The original inlines the Missiles.txt record lookup, UNITS_GetXPosition, UNITS_GetYPosition and DATATBLS_GetSkillsTxtRecordById here.
int32_t __fastcall MISSILE_CltHitFunc46(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2SkillsTxt *pSkillsTxt;
    D2UnitStrc *pOwner;
    int32_t nMissileId;
    int32_t nSkillId;
    int32_t nLevel;
    int32_t nMissiles;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wHitSubMissile[0] < 0)
        return 1;

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
        return 1;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETOFFSET;

    tParams.nX = UNITS_GetXPosition(pMissile);

    tParams.nY = UNITS_GetYPosition(pMissile);

    tParams.pOwner = pOwner;
    tParams.nMissile = (int16_t)pMissilesTxt->wHitSubMissile[0];
    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
    tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    tParams.pInitFunc = (void (__fastcall *)(D2UnitStrc *, int32_t))MISSILE_StartSeededPathFromOrigin;

    nMissiles = ((int32_t)D2COMMON_MISSILE_GetLevel(pMissile) - 1) * (int32_t)pMissilesTxt->dwCltHitPar[1] + (int32_t)pMissilesTxt->dwCltHitPar[0];
    if (nMissiles <= 0)
    {
        nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
        nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
        if (!pSkillsTxt)
            return 1;
        nMissiles = D2COMMON_SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxt->dwCalc[3], (D2C_Skills)nSkillId, nLevel);
        if (nMissiles < 1)
            nMissiles = 1;
    }

    SKILLS_SpawnMissileArray(nMissiles, &tParams);
    return 1;
}


//1.10f: D2Client.0x6FB38740
//  pCltHitFunc 47: catapult spike ball alone (CltHitSubMissile1..2 catapult spike in air and fireexplode) -- the spike ball bursting. With a record,
//  a CltHitSubMissile1 not negative and an owner: a count of cHitPar1 + (level - 1) * cHitPar2, else, not above 0, the skill's Calc4 for the owner,
//  at least 1 (no skill record: nothing more); SKILLS_SpawnClientEffectScatter of CltHitSubMissile1, that count over a radius of the count / 4 -- a
//  signed division (`cdq / and edx, 3 / add eax, edx / sar eax, 2`) --, with the skill and level. Then, when CltHitSubMissile2 is not negative: one
//  CltHitSubMissile2, flags 0x20, origin the missile, AIMED at the missile's own position (TargetX/Y its X and Y, Y read first), for the missile
//  itself if a player or a monster and else for its owner (none: nothing). Answers 1.
//  The original inlines the Missiles.txt record lookup, DATATBLS_GetSkillsTxtRecordById, UNITS_GetYPosition and UNITS_GetXPosition here.
int32_t __fastcall MISSILE_CltHitFunc47(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2SkillsTxt *pSkillsTxt;
    D2UnitStrc *pOwner;
    int32_t nMissileId;
    int32_t nSkillId;
    int32_t nLevel;
    int32_t nCount;
    int32_t nX;
    int32_t nY;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltHitSubMissile[0] < 0)
        return 1;

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
        return 1;

    nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    nCount = (nLevel - 1) * (int32_t)pMissilesTxt->dwCltHitPar[1] + (int32_t)pMissilesTxt->dwCltHitPar[0];
    if (nCount <= 0)
    {
        nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
        pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
        if (!pSkillsTxt)
            return 1;
        nCount = D2COMMON_SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxt->dwCalc[3], (D2C_Skills)nSkillId, nLevel);
        if (nCount < 1)
            nCount = 1;
    }

    nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
    SKILLS_SpawnClientEffectScatter(pMissile, (D2C_MissilesHcIdx)(int16_t)pMissilesTxt->wCltHitSubMissile[0], nCount, nCount / 4, (D2C_Skills)nSkillId, nLevel);

    if ((int16_t)pMissilesTxt->wCltHitSubMissile[1] < 0)
        return 1;

    nY = UNITS_GetYPosition(pMissile);

    nX = UNITS_GetXPosition(pMissile);

    nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
    nMissileId = (int16_t)pMissilesTxt->wCltHitSubMissile[1];
    if (pMissile->dwUnitType == UNIT_PLAYER || pMissile->dwUnitType == UNIT_MONSTER)
        pOwner = pMissile;
    else
        pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
        return 1;

    memset(&tParams, 0, sizeof(tParams));
    tParams.pOwner = pOwner;
    tParams.nSkill = nSkillId;
    tParams.dwFlags = MISSILESPAWN_TARGETPOS;
    tParams.pOrigin = pMissile;
    tParams.nTargetX = nX;
    tParams.nTargetY = nY;
    tParams.nSkillLevel = nLevel;
    tParams.nMissile = nMissileId;
    MISSILE_ClientSpawn(&tParams);
    return 1;
}


//1.10f: D2Client.0x6FB38940
//  pCltHitFunc 50: catapult meteor ball alone (CltHitSubMissile1..2 moltenboulder-flyingrocks and catapult meteor fire) --
//  MISSILE_SpawnBoulderImpactDebris with cHitPar1..2 and the two sub-missile ids, the ids as words (`movsx`). No record: nothing. Answers 1.
//  The original inlines the Missiles.txt record lookup here.
int32_t __fastcall MISSILE_CltHitFunc50(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissilesTxt *pMissilesTxt;
    int32_t nMissileId;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (pMissilesTxt)
    {
        MISSILE_SpawnBoulderImpactDebris(pMissile, (int32_t)pMissilesTxt->dwCltHitPar[0], (int32_t)pMissilesTxt->dwCltHitPar[1], (int16_t)pMissilesTxt->wCltHitSubMissile[0], (int16_t)pMissilesTxt->wCltHitSubMissile[1]);
    }

    return 1;
}


//1.10f: D2Client.0x6FB389A0
//  A boulder's impact, for pCltHitFunc 50 and 52. For the missile's owner (none: nothing), with its skill and level, at the missile's position:
//    * when nFlyingRockMissile is not negative: nRockCount rocks, flags 0x524, velocity nRockVelocity, of missile 456, moltenboulder-flyingrocks -- a
//      CONSTANT (the argument is tested and never used) --, each aimed at the impact plus two rolls below 4 * nRockCount + 1, less 2 * nRockCount,
//      and spawned only when that point is more than 3 from the missile (`cmp eax, 3 / jle`);
//    * when nGroundFireMissile is not negative: 18 of it, flags 1, at the impact plus gnBoulderFireOffsetX/Y -- entry for entry the eighteen of
//      gnMeteorSplashOffsetX/Y that pCltHitFunc 18 uses, a second copy of the same points.
//  void: eax at the exits is a leftover; both callers discard it.
//  The original inlines UNITS_GetXPosition, UNITS_GetYPosition and SEED_RollLimitedRandomNumber here.
void __fastcall MISSILE_SpawnBoulderImpactDebris(D2UnitStrc *pMissile, int32_t nRockCount, int32_t nRockVelocity, int32_t nFlyingRockMissile, int32_t nGroundFireMissile)
{
    D2MissileStrc tParams;
    D2UnitStrc *pOwner;
    int32_t nSkillId;
    int32_t nLevel;
    int32_t nImpactX;
    int32_t nImpactY;
    int32_t nSpread;
    int32_t nRocksLeft;
    int32_t nRockX;
    int32_t nRockY;
    int32_t nFire;

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
        return;

    nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
    nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);

    nImpactX = UNITS_GetXPosition(pMissile);

    nImpactY = UNITS_GetYPosition(pMissile);

    if (nFlyingRockMissile >= 0)
    {
        memset(&tParams, 0, sizeof(tParams));
        tParams.nVelocity = nRockVelocity;
        tParams.dwFlags = MISSILESPAWN_VELOCITY | MISSILESPAWN_TARGETPOS | MISSILESPAWN_ARC | MISSILESPAWN_FRAMESTOTARGET;
        tParams.pOwner = pOwner;
        tParams.pOrigin = pMissile;
        tParams.nMissile = 456;
        tParams.nSkill = nSkillId;
        tParams.nSkillLevel = nLevel;
        nSpread = 2 * nRockCount;
        if (nRockCount > 0)
        {
            nRocksLeft = nRockCount;
            do
            {
                nRockX = SEED_RollLimitedRandomNumber(&pMissile->pSeed, 2 * nSpread + 1) - nSpread + nImpactX;

                nRockY = SEED_RollLimitedRandomNumber(&pMissile->pSeed, 2 * nSpread + 1) - nSpread + nImpactY;

                if ((int32_t)D2COMMON_UNITS_GetDistanceToCoordinates(pMissile, nRockX, nRockY) > 3)
                {
                    tParams.nTargetX = nRockX;
                    tParams.nTargetY = nRockY;
                    MISSILE_ClientSpawn(&tParams);
                }
            }
            while (--nRocksLeft);
        }
    }

    if (nGroundFireMissile < 0)
        return;

    for (nFire = 0; nFire < 18; ++nFire)
    {
        memset(&tParams, 0, sizeof(tParams));
        tParams.pOwner = pOwner;
        tParams.nY = nImpactY + gnBoulderFireOffsetY[nFire];
        tParams.dwFlags = MISSILESPAWN_STARTPOS;
        tParams.nX = nImpactX + gnBoulderFireOffsetX[nFire];
        tParams.nSkill = nSkillId;
        tParams.nSkillLevel = nLevel;
        tParams.nMissile = nGroundFireMissile;
        MISSILE_ClientSpawn(&tParams);
    }
}


//1.10f: D2Client.0x6FB38BF0
//  pCltHitFunc 51: lightingtrailingjavalin and advlighttrailingjav (CltHitSubMissile1 their explosions) -- the trailing javelins' bolts. With a
//  record, a CltHitSubMissile1 not negative and a cHitPar1 above 0 (`jle`): a block, flags 3, the missile's position, its skill, its owner -- asked
//  and not tested --, the id and the level, and, when cHitPar2 is not zero, SKILLS_MissileStartSeededPath as its initialiser; and
//  SKILLS_SpawnMissileArray spawns cHitPar1 of them. Answers 1.
//  The original inlines the Missiles.txt record lookup, UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall MISSILE_CltHitFunc51(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    int32_t nMissileId;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltHitSubMissile[0] < 0 || (int32_t)pMissilesTxt->dwCltHitPar[0] <= 0)
        return 1;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETOFFSET;

    tParams.nX = UNITS_GetXPosition(pMissile);

    tParams.nY = UNITS_GetYPosition(pMissile);

    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
    tParams.pOwner = CUNIT_GetOwnerUnit(pMissile);
    tParams.nMissile = (int16_t)pMissilesTxt->wCltHitSubMissile[0];
    tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    if (pMissilesTxt->dwCltHitPar[1])
        tParams.pInitFunc = SKILLS_MissileStartSeededPath;

    SKILLS_SpawnMissileArray((int32_t)pMissilesTxt->dwCltHitPar[0], &tParams);
    return 1;
}


//1.10f: D2Client.0x6FB38D20
//  pCltHitFunc 52: moltenboulder alone (CltHitSubMissile1..3 moltenboulderexplode, moltenboulder-flyingrocks and moltenboulderfirepath) -- the
//  rolling boulder's impact. With a record: a UNIT hit counts only on a large monster -- a unit that is not a monster, a class with no monstats2 row,
//  or a row without the LARGE flag (the byte at +5 against gdwBitMasks[3], flag index 11) answers 0, and the boulder rolls on; terrain or a large
//  monster goes on.
//  Then, when CltHitSubMissile1 is not negative, one CltHitSubMissile1, flags 1, at the missile's position (Y read first), with its skill and level,
//  for its owner (none: no spawn); and either way MISSILE_SpawnBoulderImpactDebris with cHitPar1, cHitPar2, CltHitSubMissile2 and CltHitSubMissile3.
//  On the stock row the third is 456, moltenboulder-flyingrocks: the constant the boulder spawns whatever it is passed. Answers 1.
//  The original inlines the Missiles.txt record lookup, DATATBLS_GetMonStats2TxtRecord, UNITS_GetYPosition and UNITS_GetXPosition here.
int32_t __fastcall MISSILE_CltHitFunc52(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2MonStats2Txt *pMonStats2Txt;
    D2UnitStrc *pOwner;
    int32_t nMissileId;
    int32_t nSkillId;
    int32_t nLevel;
    int32_t nX;
    int32_t nY;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt)
        return 1;

    if (pTarget)
    {
        if (pTarget->dwUnitType != UNIT_MONSTER)
            return 0;

        pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord(pTarget->dwClassId);
        if (!pMonStats2Txt || !(pMonStats2Txt->nFlags[MONSTATS2FLAGINDEX_LARGE >> 3] & gdwBitMasks[MONSTATS2FLAGINDEX_LARGE & 7]))
            return 0;
    }

    if ((int16_t)pMissilesTxt->wCltHitSubMissile[0] >= 0)
    {
        nY = UNITS_GetYPosition(pMissile);

        nX = UNITS_GetXPosition(pMissile);

        nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
        nMissileId = (int16_t)pMissilesTxt->wCltHitSubMissile[0];
        pOwner = CUNIT_GetOwnerUnit(pMissile);
        if (pOwner)
        {
            memset(&tParams, 0, sizeof(tParams));
            tParams.pOwner = pOwner;
            tParams.nX = nX;
            tParams.dwFlags = MISSILESPAWN_STARTPOS;
            tParams.nY = nY;
            tParams.nSkill = nSkillId;
            tParams.nSkillLevel = nLevel;
            tParams.nMissile = nMissileId;
            MISSILE_ClientSpawn(&tParams);
        }
    }

    MISSILE_SpawnBoulderImpactDebris(pMissile, (int32_t)pMissilesTxt->dwCltHitPar[0], (int32_t)pMissilesTxt->dwCltHitPar[1], (int16_t)pMissilesTxt->wCltHitSubMissile[1], (int16_t)pMissilesTxt->wCltHitSubMissile[2]);
    return 1;
}


//1.10f: D2Client.0x6FB38F00
//  pCltHitFunc 53: moltenboulderemerge alone (CltHitSubMissile1 moltenboulder) -- the boulder coming out of the ground. With a record and a
//  CltHitSubMissile1 not negative: one CltHitSubMissile1, flags 0x21, for the owner -- asked and not tested --, at the missile's position (X read
//  first), with its skill and level, aimed at the first point of the missile's path (D2Common #10175 and #10176). The boulder it spawns, when there
//  is one, starts a bounce of cHitPar1 and cHitPar2 (nBouncesLeft and nRestitution of PARTICLE_StartUnitBounce), then an arc of peak 0 over cHitPar3
//  frames, and has its timed motion cleared. Answers 1 whatever was hit.
//  The original inlines the Missiles.txt record lookup, UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall MISSILE_CltHitFunc53(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pBoulder;
    int32_t nMissileId;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltHitSubMissile[0] < 0)
        return 1;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETPOS;
    tParams.pOwner = CUNIT_GetOwnerUnit(pMissile);

    tParams.nX = UNITS_GetXPosition(pMissile);

    tParams.nY = UNITS_GetYPosition(pMissile);

    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
    tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    tParams.nMissile = (int16_t)pMissilesTxt->wCltHitSubMissile[0];
    tParams.nTargetX = D2COMMON_10175_PathGetFirstPointX(pMissile->pDynamicPath);
    tParams.nTargetY = D2COMMON_10176_PathGetFirstPointY(pMissile->pDynamicPath);
    pBoulder = MISSILE_ClientSpawn(&tParams);
    if (pBoulder)
    {
        PARTICLE_StartUnitBounce(pBoulder, (int32_t)pMissilesTxt->dwCltHitPar[0], (int32_t)pMissilesTxt->dwCltHitPar[1]);
        PARTICLE_StartUnitArc(pBoulder, 0, (int32_t)pMissilesTxt->dwCltHitPar[2]);
        PARTICLE_ClearUnitTimedMotion(pBoulder);
    }

    return 1;
}


//1.10f: D2Client.0x6FB39050
//  pCltHitFunc 54: vines trail and plague vines trail (CltHitSubMissile1 vines wither and plague vines wither) -- a vine withering. With a record and
//  a CltHitSubMissile1 not negative: a UNIT hit answers 0; anything else spawns one CltHitSubMissile1, flags 0x20, from the missile (the origin),
//  with its skill and level, for the missile itself if a player or a monster and else for its owner (none: nothing), and turns it to the missile's
//  own direction (D2Common's PATH_GetDirection and PATH_SetDirection). TargetX and TargetY are stored 0 over the zeroed block. Answers 1.
//  The original inlines the Missiles.txt record lookup here.
int32_t __fastcall MISSILE_CltHitFunc54(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pOwner;
    D2UnitStrc *pWither;
    int32_t nMissileId;
    int32_t nSkillId;
    int32_t nLevel;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltHitSubMissile[0] < 0)
        return 1;

    if (pTarget)
        return 0;

    nMissileId = (int16_t)pMissilesTxt->wCltHitSubMissile[0];
    nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
    if (pMissile->dwUnitType == UNIT_PLAYER || pMissile->dwUnitType == UNIT_MONSTER)
        pOwner = pMissile;
    else
        pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
        return 1;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_TARGETPOS;
    tParams.nSkill = nSkillId;
    tParams.pOwner = pOwner;
    tParams.pOrigin = pMissile;
    tParams.nTargetX = 0;
    tParams.nTargetY = 0;
    tParams.nSkillLevel = nLevel;
    tParams.nMissile = nMissileId;
    pWither = MISSILE_ClientSpawn(&tParams);
    if (pWither)
        D2COMMON_PATH_SetDirection(pWither->pDynamicPath, D2COMMON_PATH_GetDirection(pMissile->pDynamicPath));

    return 1;
}


//1.10f: D2Client.0x6FB39150
//  pCltHitFunc 55: volcano debris 2, armageddonrock and diablogeddonrock (CltHitSubMissile1..2 their explosion and their fire) -- a rock landing.
//  With a record and an owner (none: nothing): one block from the missile (the origin, flags 0) with its skill and level, and a spawn of each of
//  CltHitSubMissile1, 2 and 3 that is not negative, in that order, the block reused with only nMissile changed. Answers 1 whatever was hit.
//  The original inlines the Missiles.txt record lookup here.
int32_t __fastcall MISSILE_CltHitFunc55(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pOwner;
    int32_t nMissileId;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt)
        return 1;

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
        return 1;

    memset(&tParams, 0, sizeof(tParams));
    tParams.pOwner = pOwner;
    tParams.pOrigin = pMissile;
    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
    tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);

    if ((int16_t)pMissilesTxt->wCltHitSubMissile[0] >= 0)
    {
        tParams.nMissile = (int16_t)pMissilesTxt->wCltHitSubMissile[0];
        MISSILE_ClientSpawn(&tParams);
    }

    if ((int16_t)pMissilesTxt->wCltHitSubMissile[1] >= 0)
    {
        tParams.nMissile = (int16_t)pMissilesTxt->wCltHitSubMissile[1];
        MISSILE_ClientSpawn(&tParams);
    }

    if ((int16_t)pMissilesTxt->wCltHitSubMissile[2] >= 0)
    {
        tParams.nMissile = (int16_t)pMissilesTxt->wCltHitSubMissile[2];
        MISSILE_ClientSpawn(&tParams);
    }

    return 1;
}


//1.10f: D2Client.0x6FB39230
//  pCltHitFunc 56: recycler vine alone (CltHitSubMissile1 recycler fade) -- the vine fading. With a record, a CltHitSubMissile1 not negative and an
//  owner (none: nothing): one CltHitSubMissile1 from the missile (the origin, flags 0), with its skill and level, turned to the missile's direction
//  as pCltHitFunc 54's is. Answers 1 whatever was hit.
//  The original inlines the Missiles.txt record lookup here.
int32_t __fastcall MISSILE_CltHitFunc56(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pOwner;
    D2UnitStrc *pFade;
    int32_t nMissileId;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltHitSubMissile[0] < 0)
        return 1;

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
        return 1;

    memset(&tParams, 0, sizeof(tParams));
    tParams.nMissile = (int16_t)pMissilesTxt->wCltHitSubMissile[0];
    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
    tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    tParams.pOwner = pOwner;
    tParams.pOrigin = pMissile;
    pFade = MISSILE_ClientSpawn(&tParams);
    if (pFade)
        D2COMMON_PATH_SetDirection(pFade->pDynamicPath, D2COMMON_PATH_GetDirection(pMissile->pDynamicPath));

    return 1;
}


//1.10f: D2Client.0x6FB392F0
//  pCltHitFunc 57: baal spawn monsters alone (cHitPar1..2 of 10 and 20; CltHitSubMissile1 baal spawn monsters exp) -- Baal's summoning, seen. With a
//  record and a CltHitSubMissile1 not negative: a UNIT hit answers 0; otherwise MISSILE_SpawnOverArea around the missile's position (Y read first) of
//  CltHitSubMissile1, cHitPar1 as the radius and cHitPar2 as nOneInChance, step 1, with the missile's skill and level. Answers 1.
//  The original inlines the Missiles.txt record lookup, UNITS_GetYPosition and UNITS_GetXPosition here.
int32_t __fastcall MISSILE_CltHitFunc57(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissilesTxt *pMissilesTxt;
    int32_t nMissileId;
    int32_t nOneInChance;
    int32_t nRadius;
    int32_t nSkillId;
    int32_t nLevel;
    int32_t nX;
    int32_t nY;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltHitSubMissile[0] < 0)
        return 1;

    if (pTarget)
        return 0;

    nOneInChance = (int32_t)pMissilesTxt->dwCltHitPar[1];
    nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
    nMissileId = (int16_t)pMissilesTxt->wCltHitSubMissile[0];
    nRadius = (int32_t)pMissilesTxt->dwCltHitPar[0];

    nY = UNITS_GetYPosition(pMissile);

    nX = UNITS_GetXPosition(pMissile);

    MISSILE_SpawnOverArea(pMissile, nX, nY, nRadius, nMissileId, nSkillId, nLevel, nOneInChance, 1);
    return 1;
}


//1.10f: D2Client.0x6FB39400
//  pCltHitFunc 60: nihlithak1 alone (cHitPar1 of 85; CltHitSubMissile1..2 nihlithak2 and nehlithakbonechips) -- Nihlathak's corpse bursting. With a
//  record: one block from the missile (the origin, flags 0), for its owner -- asked and not tested --, with its skill and level; CltHitSubMissile1
//  when not negative; then CltHitSubMissile2 when not negative, the unit it spawns, when there is one, given the height cHitPar1
//  (PARTICLE_SetUnitGfxCoords, nX and nY 0, nZ the column). Answers 1 whatever was hit.
//  The original inlines the Missiles.txt record lookup here.
int32_t __fastcall MISSILE_CltHitFunc60(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pChips;
    int32_t nMissileId;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt)
        return 1;

    memset(&tParams, 0, sizeof(tParams));
    tParams.pOwner = CUNIT_GetOwnerUnit(pMissile);
    tParams.pOrigin = pMissile;
    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
    tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);

    if ((int16_t)pMissilesTxt->wCltHitSubMissile[0] >= 0)
    {
        tParams.nMissile = (int16_t)pMissilesTxt->wCltHitSubMissile[0];
        MISSILE_ClientSpawn(&tParams);
    }

    if ((int16_t)pMissilesTxt->wCltHitSubMissile[1] >= 0)
    {
        tParams.nMissile = (int16_t)pMissilesTxt->wCltHitSubMissile[1];
        pChips = MISSILE_ClientSpawn(&tParams);
        if (pChips)
            PARTICLE_SetUnitGfxCoords(pChips, 0, 0, (int32_t)pMissilesTxt->dwCltHitPar[0]);
    }

    return 1;
}


//1.10f: D2Client.0x6FB394D0
//  pCltHitFunc 61: worldstone shake alone. A UNIT hit answers 0; any other ends the screen shake in progress (VIEWEFFECTS_ResetScreenShake) and
//  answers 1.
int32_t __fastcall MISSILE_CltHitFunc61(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    if (pTarget)
        return 0;

    VIEWEFFECTS_ResetScreenShake();
    return 1;
}


//1.10f: D2Client.0x6FB394F0
//  Sets gnLightQuality (`mov gnLightQuality, ecx / retn`); called by ESCMENU_LightQualityOnChange and ESCMENU_LightQualityRefresh.
//  MISSILE_ClientSpawn reads gnLightQuality too.
void __fastcall MISSILE_EnvSetLightQuality(int32_t nQuality)
{
    gnLightQuality = nQuality;
}


//1.10f: D2Client.0x6FB39500
//  Returns gnLightQuality (`mov eax, gnLightQuality / retn`).
int32_t __cdecl MISSILE_GetLightQuality()
{
    return gnLightQuality;
}


//1.10f: D2Client.0x6FB39510
//  A missile's target test, monsters only: one of the three predicates in gaMissileTargetTests (0x6FB91218), which MISSILE_CltDoFunc1 reads; there is
//  no code caller. The table has nine records of eight bytes, a test and a collision mask; this one is records 2 and 5.
//  A unit that is not a monster, or none, is refused. Then the tail all three predicates share (here, in MISSILE_IsEnemyTarget and in
//  MISSILE_IsPlayerOrMonsterTarget): the unit's dwFlags bit 3, UNITFLAG_ISVALIDTARGET, then bit 2, UNITFLAG_CANBEATTACKED (`shr` 3 and 2, `and 1`);
//  when the row's NextHit is set, not in STATE_JUSTHIT; not the missile's own owner by D2Common's MISSILE_CheckUnitIfOwner; and, when the context
//  names an owner, CUNIT_IsUnitSelectableTarget from it or the row's CollideFriend. No out-of-line body holds the tail, so it is written in each of
//  the three. The asserts below pin the context's layout.
static_assert(offsetof(D2ClientMissileTargetCtxStrc, pMissile) == 0
                  && offsetof(D2ClientMissileTargetCtxStrc, pOwner) == 4
                  && offsetof(D2ClientMissileTargetCtxStrc, pMissilesTxt) == 8
                  && offsetof(D2MissilesTxt, nCollideFriend) == 0x187
                  && offsetof(D2MissilesTxt, nNextHit) == 0x188,
              "target tests: pCtx at [edx], [edx+4], [edx+8] (0x6FB39530..0x6FB39536); "
              "NextHit at +188h (0x6FB39552), CollideFriend at +187h (0x6FB39584)");

BOOL __fastcall MISSILE_IsMonsterTarget(D2UnitStrc *pUnit, D2ClientMissileTargetCtxStrc *pCtx)
{
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pOwner;
    D2UnitStrc *pMissile;

    if (!pUnit || pUnit->dwUnitType != UNIT_MONSTER)
        return FALSE;

    pMissilesTxt = pCtx->pMissilesTxt;
    pOwner = pCtx->pOwner;
    pMissile = pCtx->pMissile;
    if (!(pUnit->dwFlags & UNITFLAG_ISVALIDTARGET) || !(pUnit->dwFlags & UNITFLAG_CANBEATTACKED))
        return FALSE;
    if (pMissilesTxt->nNextHit && D2COMMON_STATES_CheckState(pUnit, STATE_JUSTHIT))
        return FALSE;
    if (D2COMMON_MISSILE_CheckUnitIfOwner(pMissile, pUnit))
        return FALSE;
    if (pOwner && !CUNIT_IsUnitSelectableTarget(pOwner, pUnit) && !pMissilesTxt->nCollideFriend)
        return FALSE;

    return TRUE;
}


//1.10f: D2Client.0x6FB395B0
//  A missile's target test, "enemies": record 1 of gaMissileTargetTests. A monster is accepted when D2Common's STATLIST_GetUnitAlignment answers 2,
//  UNIT_ALIGNMENT_GOOD, and refused on any other answer, with no more tests; a player goes through the tail MISSILE_IsMonsterTarget describes;
//  anything else, or no unit, is refused.
BOOL __fastcall MISSILE_IsEnemyTarget(D2UnitStrc *pUnit, D2ClientMissileTargetCtxStrc *pCtx)
{
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pOwner;
    D2UnitStrc *pMissile;

    if (!pUnit)
        return FALSE;

    if (pUnit->dwUnitType == UNIT_MONSTER)
        return D2COMMON_STATLIST_GetUnitAlignment(pUnit) == UNIT_ALIGNMENT_GOOD;

    if (pUnit->dwUnitType != UNIT_PLAYER)
        return FALSE;

    pMissilesTxt = pCtx->pMissilesTxt;
    pOwner = pCtx->pOwner;
    pMissile = pCtx->pMissile;
    if (!(pUnit->dwFlags & UNITFLAG_ISVALIDTARGET) || !(pUnit->dwFlags & UNITFLAG_CANBEATTACKED))
        return FALSE;
    if (pMissilesTxt->nNextHit && D2COMMON_STATES_CheckState(pUnit, STATE_JUSTHIT))
        return FALSE;
    if (D2COMMON_MISSILE_CheckUnitIfOwner(pMissile, pUnit))
        return FALSE;
    if (pOwner && !CUNIT_IsUnitSelectableTarget(pOwner, pUnit) && !pMissilesTxt->nCollideFriend)
        return FALSE;

    return TRUE;
}


//1.10f: D2Client.0x6FB39650
//  A missile's target test, players and monsters: records 3 and 8 of gaMissileTargetTests: a player or a monster (`test eax, eax` and `cmp eax, 1`)
//  through the tail MISSILE_IsMonsterTarget describes; anything else, or no unit, is refused.
BOOL __fastcall MISSILE_IsPlayerOrMonsterTarget(D2UnitStrc *pUnit, D2ClientMissileTargetCtxStrc *pCtx)
{
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pOwner;
    D2UnitStrc *pMissile;

    if (!pUnit || (pUnit->dwUnitType != UNIT_PLAYER && pUnit->dwUnitType != UNIT_MONSTER))
        return FALSE;

    pMissilesTxt = pCtx->pMissilesTxt;
    pOwner = pCtx->pOwner;
    pMissile = pCtx->pMissile;
    if (!(pUnit->dwFlags & UNITFLAG_ISVALIDTARGET) || !(pUnit->dwFlags & UNITFLAG_CANBEATTACKED))
        return FALSE;
    if (pMissilesTxt->nNextHit && D2COMMON_STATES_CheckState(pUnit, STATE_JUSTHIT))
        return FALSE;
    if (D2COMMON_MISSILE_CheckUnitIfOwner(pMissile, pUnit))
        return FALSE;
    if (pOwner && !CUNIT_IsUnitSelectableTarget(pOwner, pUnit) && !pMissilesTxt->nCollideFriend)
        return FALSE;

    return TRUE;
}


//  Layout checks: the two adjacent row bytes MISSILE_InitClientUnit reads, the missiles.txt flag index the original carries as gdwBitMasks[4], and
//  the missile ids the hf* chain and pCltHitFunc 31 carry as literals.
static_assert(offsetof(D2MissilesTxt, nCollideType) == 0x183 && offsetof(D2MissilesTxt, nCollision) == 0x184
                  && MISSILESFLAGINDEX_CANDESTROY == 4,
              "CollideType at +183h, Collision at +184h (0x6FB397A5, 0x6FB397C7); "
              "`test [ecx+10h], al` against the flags' low byte (0x6FB39734)");
static_assert(MISSILE_HFCONTROL == 373 && MISSILE_HFFRAGMENT1 == 374 && MISSILE_HFFRAGMENT2 == 375
                  && MISSILE_HFFRAGMENT3 == 376 && MISSILE_HFSPIRIT1 == 377,
              "the hf* chain: 175h, 176h, 177h, 178h, 179h in the bytes");
static_assert(MISSILE_ICEBREAKSMALL == 272 && MISSILE_ICEBREAKMEDIUM == 273 && MISSILE_ICEBREAKLARGE == 274
                  && MISSILE_CATAPULT_COLD_EXPLOSION == 417 && MISSILE_ICEBREAKSMALLMELT == 344
                  && MISSILE_ICEBREAKLARGEMELT == 345,
              "pCltHitFunc 31's switch: base 110h, 146 cases (0x6FB398A3); 158h and 159h stored");


//1.10f: D2Client.0x6FB396F0
//  A missile's target test, missiles: record 7 of gaMissileTargetTests (CollideType 7, MISSILESCOLLIDE_MISSILE), the only one of the four tests that
//  takes no context. A unit that is a missile, with a missiles.txt record whose CanDestroy flag is set: the flags' LOW BYTE against gdwBitMasks[4]'s
//  (`mov al, [eax+4]` / `test [ecx+10h], al`), written at that width. Anything else is refused.
//  The original inlines the Missiles.txt record lookup here.
BOOL __fastcall MISSILE_IsTargetableMissile(D2UnitStrc *pUnit)
{
    D2MissilesTxt *pMissilesTxt;
    int32_t nMissileId;

    if (!pUnit || pUnit->dwUnitType != UNIT_MISSILE)
        return FALSE;

    pMissilesTxt = NULL;
    nMissileId = pUnit->dwClassId;
    if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
        pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    if (!pMissilesTxt)
        return FALSE;

    if ((uint8_t)pMissilesTxt->dwMissileFlags & (uint8_t)gdwBitMasks[MISSILESFLAGINDEX_CANDESTROY])
        return TRUE;

    return FALSE;
}


//1.10f: D2Client.0x6FB39750
//  A missile unit's client half: CUNIT_InitUnit's UNIT_MISSILE arm. With a missiles.txt record: D2Common allocates the missile data; the unit's anim
//  mode is set to the row's CollideType; a dynamic path at nX/nY in pRoom; the footprint collision mask 0x40 when the row's Collision byte is set,
//  else 0; the move-test mask from the CollideType's record of gaMissileTargetTests (its second dword); then dwFlags' bit 2, UNITFLAG_CANBEATTACKED,
//  from the row's CanDestroy flag (the low bytes again), stored, and stored again with bits 1 and 3 cleared and bit 5 set (`and ecx, 0FFFFFFF5h / or
//  ecx, 20h`): not targetable, not a valid target, drawn with a shadow. `retn 0Ch`: three stack arguments, and the third is never read -- the caller
//  passes the class id there.
//  The original inlines the Missiles.txt record lookup here.
void __fastcall MISSILE_InitClientUnit(D2UnitStrc *pUnit, D2ActiveRoomStrc *pRoom, int32_t nX, int32_t nY, int32_t nUnused)
{
    D2MissilesTxt *pMissilesTxt;
    D2DynamicPathStrc *pDynamicPath;
    uint32_t dwFlags;
    int32_t nMissileId;

    UNREFERENCED_PARAMETER(nUnused);

    pMissilesTxt = NULL;
    if (pUnit)
    {
        nMissileId = pUnit->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }
    if (!pMissilesTxt)
        return;

    D2COMMON_MISSILE_AllocMissileData(pUnit);
    D2COMMON_UNITS_ChangeAnimMode(pUnit, pMissilesTxt->nCollideType);
    D2COMMON_PATH_AllocDynamicPath(NULL, pRoom, nX, nY, pUnit, TRUE);
    pDynamicPath = pUnit->pDynamicPath;
    D2COMMON_PATH_SetFootprintCollisionMask(pDynamicPath, pMissilesTxt->nCollision ? 0x40 : 0);
    D2COMMON_PATH_SetMoveTestCollisionMask(pDynamicPath, gaMissileTargetTests[pMissilesTxt->nCollideType].nCollisionMask);

    dwFlags = pUnit->dwFlags;
    if ((uint8_t)pMissilesTxt->dwMissileFlags & (uint8_t)gdwBitMasks[MISSILESFLAGINDEX_CANDESTROY])
        dwFlags |= UNITFLAG_CANBEATTACKED;
    else
        dwFlags &= ~UNITFLAG_CANBEATTACKED;
    pUnit->dwFlags = dwFlags;
    pUnit->dwFlags = (dwFlags & ~(UNITFLAG_TARGETABLE | UNITFLAG_ISVALIDTARGET)) | UNITFLAG_DRAWSHADOW;
}


//1.10f: D2Client.0x6FB39830
//  A missile unit's client half, freed: CUNIT_DestroyUnit's UNIT_MISSILE arm. The missile stream, the missile data, the unit's particles and its gfx
//  info; while an act is loaded (gpAct), D2Common #10228 and the unit's removal from its room; then the dynamic path, and the pointer cleared.
void __fastcall MISSILE_FreeUnitData(D2UnitStrc *pUnit)
{
    D2COMMON_MISSTREAM_FreeMissileStream(pUnit);
    D2COMMON_MISSILE_FreeMissileData(pUnit);
    PARTICLE_FreeUnitParticles(pUnit);
    GFX_FreeUnitGfxInfo(pUnit);
    if (gpAct)
    {
        D2COMMON_10228(pUnit);
        D2COMMON_UNITROOM_RemoveUnitFromRoom(pUnit);
    }
    D2COMMON_PATH_FreeDynamicPath(NULL, pUnit->pDynamicPath);
    pUnit->pDynamicPath = NULL;
}


//1.10f: D2Client.0x6FB39880
//  pCltHitFunc 31: icebreaksmall, icebreakmedium, icebreaklarge and catapult cold explosion -- ice breaking, melting. A UNIT hit answers 0. Otherwise
//  the melt is chosen by the missile's OWN class, a switch (no column): icebreaksmall spawns icebreaksmallmelt, the other three icebreaklargemelt,
//  any other class -1. With an owner (none: nothing): that id, flags 1, at the missile's position (Y read first), with its skill and level -- the -1
//  is passed to the spawn untested --, and the melt, when spawned, turned to the missile's direction and run at animation speed 128. Answers 1.
//  The original inlines UNITS_GetYPosition and UNITS_GetXPosition here.
int32_t __fastcall MISSILE_CltHitFunc31(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissileStrc tParams;
    D2UnitStrc *pOwner;
    D2UnitStrc *pMelt;
    int32_t nMeltMissile;
    int32_t nSkillId;
    int32_t nLevel;
    int32_t nX;
    int32_t nY;

    if (pTarget)
        return 0;

    nMeltMissile = -1;
    switch (D2COMMON_MISSILE_GetClassId(pMissile))
    {
    case MISSILE_ICEBREAKSMALL:
        nMeltMissile = MISSILE_ICEBREAKSMALLMELT;
        break;

    case MISSILE_ICEBREAKMEDIUM:
    case MISSILE_ICEBREAKLARGE:
    case MISSILE_CATAPULT_COLD_EXPLOSION:
        nMeltMissile = MISSILE_ICEBREAKLARGEMELT;
        break;

    default:
        break;
    }

    nY = UNITS_GetYPosition(pMissile);

    nX = UNITS_GetXPosition(pMissile);

    nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
        return 1;

    memset(&tParams, 0, sizeof(tParams));
    tParams.pOwner = pOwner;
    tParams.nY = nY;
    tParams.dwFlags = MISSILESPAWN_STARTPOS;
    tParams.nX = nX;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nLevel;
    tParams.nMissile = nMeltMissile;
    pMelt = MISSILE_ClientSpawn(&tParams);
    if (pMelt)
    {
        D2COMMON_PATH_SetDirection(pMelt->pDynamicPath, D2COMMON_PATH_GetDirection(pMissile->pDynamicPath));
        D2COMMON_UNITS_SetAnimationSpeed(pMelt, 128);
    }

    return 1;
}


//1.10f: D2Client.0x6FB39A70
//  pCltHitFunc 40: healingbolt alone -- the bolt that heals a pet. No unit hit: 1. An owner (none: 1) that is not a player: 0. A unit hit that is not
//  a monster: 0. A monster: whether ROSTERPETS_GetOwnerId names the owner as the pet's (`cmp eax, esi / setz cl`) -- so the bolt is taken only by the
//  owner's own pet, and passes through anything else.
int32_t __fastcall MISSILE_CltHitFunc40(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2UnitStrc *pOwner;

    if (!pTarget)
        return 1;

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
        return 1;

    if (pOwner->dwUnitType != UNIT_PLAYER)
        return 0;

    if (pTarget->dwUnitType != UNIT_MONSTER)
        return 0;

    return ROSTERPETS_GetOwnerId(pTarget->dwUnitId) == (int32_t)pOwner->dwUnitId;
}


//1.10f: D2Client.0x6FB39AC0
//  pCltDoFunc 37: diablo appears alone. The missile's frame counter -- D2Common's MISSILE_GetCurrentFrame, which runs DOWN from the range -- at 150
//  starts a screen shake of magnitude 25, no ramp up, held 4000 ms, no ramp down; at 50 plays sound 4638, sounds.txt's monster_diablo_taunt_1, on no
//  unit. Then MISSILE_CltDoFunc1, tail-jumped from both arms.
void __fastcall MISSILE_CltDoFunc37(D2UnitStrc *pMissile)
{
    int32_t nFrame;

    nFrame = D2COMMON_MISSILE_GetCurrentFrame(pMissile);
    if (nFrame == 150)
        VIEWEFFECTS_StartScreenShake(25, 0, 4000, 0);
    else if (nFrame == 50)
        SOUNDINST_PlaySound(SOUND_MONSTER_DIABLO_TAUNT_1, NULL, 0, 0, 0);

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB39B10
//  pCltDoFunc 41: hfspirit1 alone, the hf* chain's spirit. At 8 frames lived (D2Common's MISSILE_GetRemainingFrames): PARTICLE_SetUnitGfxVector2 with
//  nZ 256, and the animation speed halved -- the word, sign-extended, `sar 1`. At 35: the speed doubled (`add eax, eax`). Then MISSILE_CltDoFunc1,
//  tail-jumped from every arm.
void __fastcall MISSILE_CltDoFunc41(D2UnitStrc *pMissile)
{
    int32_t nFramesLived;

    nFramesLived = D2COMMON_MISSILE_GetRemainingFrames(pMissile);
    if (nFramesLived == 8)
    {
        PARTICLE_SetUnitGfxVector2(pMissile, 0, 0, 256, 0);
        D2COMMON_UNITS_SetAnimationSpeed(pMissile, pMissile->wAnimSpeed >> 1);
    }
    else if (nFramesLived == 35)
    {
        D2COMMON_UNITS_SetAnimationSpeed(pMissile, 2 * pMissile->wAnimSpeed);
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB39B60
//  pCltDoFunc 40: hffragment3 alone, the hf* chain's last fragment. TargetX holds an OBJECT's GUID -- pCltHitFunc 42 and 43 pass it down the chain
//  from hfcontrol's -- and TargetY a countdown. With an owner (none: nothing): while TargetY is not 0, it is taken down by one. At 0: the object
//  looked up; if found, one hfspirit1 from the missile (the origin, flags 0), with its skill and level, its TargetX and TargetY set to the missile's
//  position less the object's -- an OFFSET, not a point --, its particles allocated and its gfx coordinates, vector and second vector set (0, 0, 0;
//  0, 0, 2, scaled; 0, 0, 0, scaled). Found or not, the countdown restarts at a seed roll below 35, plus 10, less 1. Then MISSILE_CltDoFunc1.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition for three of the four coordinates (the object's Y is a call) and
//  SEED_RollRandomNumber here.
void __fastcall MISSILE_CltDoFunc40(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2UnitStrc *pOwner;
    D2UnitStrc *pObject;
    D2UnitStrc *pSpirit;
    int32_t nCountdown;
    int32_t nMissileX;
    int32_t nMissileY;
    int32_t nDeltaX;
    int32_t nDeltaY;

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (pOwner)
    {
        nCountdown = D2COMMON_MISSILE_GetTargetY(pMissile);
        if (!nCountdown)
        {
            pObject = CUNIT_FindUnitByGuidAndType((uint32_t)D2COMMON_MISSILE_GetTargetX(pMissile), UNIT_OBJECT);
            if (pObject)
            {
                nMissileX = UNITS_GetXPosition(pMissile);

                nDeltaX = nMissileX - UNITS_GetXPosition(pObject);

                nMissileY = UNITS_GetYPosition(pMissile);
                nDeltaY = nMissileY - UNITS_GetYPosition(pObject);

                memset(&tParams, 0, sizeof(tParams));
                tParams.pOrigin = pMissile;
                tParams.pOwner = pOwner;
                tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
                tParams.nMissile = MISSILE_HFSPIRIT1;
                tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
                pSpirit = MISSILE_ClientSpawn(&tParams);
                if (pSpirit)
                {
                    PARTICLE_AllocUnitParticles(pSpirit);
                    PARTICLE_SetUnitGfxCoords(pSpirit, 0, 0, 0);
                    PARTICLE_SetUnitGfxVector(pSpirit, 0, 0, 2, 1);
                    PARTICLE_SetUnitGfxVector2(pSpirit, 0, 0, 0, 1);
                    D2COMMON_MISSILE_SetTargetX(pSpirit, nDeltaX);
                    D2COMMON_MISSILE_SetTargetY(pSpirit, nDeltaY);
                }
            }

            nCountdown = (int32_t)((uint32_t)SEED_RollRandomNumber(&pMissile->pSeed) % 35) + 10;
        }
        D2COMMON_MISSILE_SetTargetY(pMissile, nCountdown - 1);
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB39D00
//  pCltDoFunc 39: hffragment1 and hffragment2 -- with an owner, the unit's nSeqCurrentFramePrecise (+0x44) zeroed; then, owner or not,
//  MISSILE_CltDoFunc1, tail-jumped.
void __fastcall MISSILE_CltDoFunc39(D2UnitStrc *pMissile)
{
    if (CUNIT_GetOwnerUnit(pMissile))
        pMissile->nSeqCurrentFramePrecise = 0;

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB39D20
//  pCltHitFunc 42: hffragment1 alone -- the first fragment's end. With an owner (none: nothing): one hffragment2 from the missile (the origin, flags
//  0), with its skill and level, handed the missile's TargetX -- the object's GUID, down the chain. Answers 1 whatever was hit.
int32_t __fastcall MISSILE_CltHitFunc42(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissileStrc tParams;
    D2UnitStrc *pOwner;
    D2UnitStrc *pFragment;

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
        return 1;

    memset(&tParams, 0, sizeof(tParams));
    tParams.pOwner = pOwner;
    tParams.pOrigin = pMissile;
    tParams.nMissile = MISSILE_HFFRAGMENT2;
    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
    tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    pFragment = MISSILE_ClientSpawn(&tParams);
    if (pFragment)
        D2COMMON_MISSILE_SetTargetX(pFragment, D2COMMON_MISSILE_GetTargetX(pMissile));

    return 1;
}


//1.10f: D2Client.0x6FB39D90
//  pCltHitFunc 43: hffragment2 alone -- the second fragment's end. The record of hfcontrol, missile 373 by a CONSTANT index, is taken first. With an
//  owner (none: nothing): one hffragment3, flags 0x8005, at the missile's position (X read first), with its skill and level, velocity stored 0 over
//  the zeroed block, and the range of hfcontrol's row, a word (`movsx`). The fragment spawned, when there is one, takes the missile's TargetX, the
//  object's GUID, and a TargetY of a seed roll below 35, plus 10: pCltDoFunc 40's countdown. Answers 1 whatever was hit.
//  The original inlines UNITS_GetXPosition, UNITS_GetYPosition and SEED_RollRandomNumber here.
int32_t __fastcall MISSILE_CltHitFunc43(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pControlTxt;
    D2UnitStrc *pOwner;
    D2UnitStrc *pFragment;

    if (sgptDataTables->nMissilesTxtRecordCount > MISSILE_HFCONTROL)
        pControlTxt = &sgptDataTables->pMissilesTxt[MISSILE_HFCONTROL];
    else
        pControlTxt = NULL;

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
        return 1;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_VELOCITY | MISSILESPAWN_RANGE;
    tParams.pOwner = pOwner;
    tParams.nMissile = MISSILE_HFFRAGMENT3;
    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
    tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    tParams.nVelocity = 0;

    //  BUG: hfcontrol's record is read with no NULL test; a missiles.txt of 373 rows or fewer faults here.
    tParams.nRange = (int16_t)pControlTxt->wRange;

    tParams.nX = UNITS_GetXPosition(pMissile);

    tParams.nY = UNITS_GetYPosition(pMissile);

    pFragment = MISSILE_ClientSpawn(&tParams);
    if (pFragment)
    {
        D2COMMON_MISSILE_SetTargetX(pFragment, D2COMMON_MISSILE_GetTargetX(pMissile));

        D2COMMON_MISSILE_SetTargetY(pFragment, (int32_t)((uint32_t)SEED_RollRandomNumber(&pMissile->pSeed) % 35) + 10);
    }

    return 1;
}


//1.10f: D2Client.0x6FB39ED0
//  pCltDoFunc 38: hfcontrol alone (Range 200), the hf* chain's start. TargetX holds an object's GUID. When the object is found AND the missile has
//  lived 10 frames (both asked first): the record of hffragment1, missile 374 by a constant index (NULL when the table has 374 rows or fewer);
//  fourteen offsets built on the stack; and, with an owner, fourteen hffragment1s from the missile (the origin, flags 0x8102), with its skill and
//  level, each aimed at one offset (TargetX/Y), its range the offset's rough length halved, times hffragment1's range, over 6 -- a signed division
//  (`imul` by 2AAAAAABh) --, and each spawned fragment handed the object's GUID and a countdown of a seed roll below 35, plus 10. Then, owner or not,
//  a screen shake of magnitude 4, ramps of 80 and 400 ms, held 80. Then MISSILE_CltDoFunc1.
//  The original inlines SEED_RollRandomNumber here.
void __fastcall MISSILE_CltDoFunc38(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pFragmentTxt;
    D2UnitStrc *pOwner;
    D2UnitStrc *pFragment;
    uint32_t dwObjectGUID;
    int32_t nFramesLived;
    int32_t nSpawn;
    int32_t nAbsX;
    int32_t nAbsY;
    int32_t nLength;

    dwObjectGUID = (uint32_t)D2COMMON_MISSILE_GetTargetX(pMissile);
    nFramesLived = D2COMMON_MISSILE_GetRemainingFrames(pMissile);
    if (CUNIT_FindUnitByGuidAndType(dwObjectGUID, UNIT_OBJECT) && nFramesLived == 10)
    {
        if (sgptDataTables->nMissilesTxtRecordCount > MISSILE_HFFRAGMENT1)
            pFragmentTxt = &sgptDataTables->pMissilesTxt[MISSILE_HFFRAGMENT1];
        else
            pFragmentTxt = NULL;

        int32_t aOffsetX[14] = { -4, 2, 4, -3, -2, 3, 5, -3, -5, 5, -6, -1, 0, 6 };
        int32_t aOffsetY[14] = { 6, 4, -4, -2, 1, 7, 0, 3, -4, -3, 4, -5, 5, -3 };

        pOwner = CUNIT_GetOwnerUnit(pMissile);
        if (pOwner)
        {
            memset(&tParams, 0, sizeof(tParams));
            tParams.dwFlags = MISSILESPAWN_TARGETOFFSET | MISSILESPAWN_ARC | MISSILESPAWN_RANGE;
            tParams.nMissile = MISSILE_HFFRAGMENT1;
            tParams.pOwner = pOwner;
            tParams.pOrigin = pMissile;
            tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
            tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);

            //  Stepped as a byte offset in the original (`add ebx, 4 / cmp ebx, 38h / jl`): fourteen entries, an index here.
            for (nSpawn = 0; nSpawn < 14; ++nSpawn)
            {
                tParams.nTargetX = aOffsetX[nSpawn];
                tParams.nTargetY = aOffsetY[nSpawn];
                nAbsX = tParams.nTargetX < 0 ? -tParams.nTargetX : tParams.nTargetX;
                nAbsY = tParams.nTargetY < 0 ? -tParams.nTargetY : tParams.nTargetY;

                //  BUG: the rough length compares the SIGNED offsets (`cmp eax, ecx / jge`), not their magnitudes, and takes
                //  |x| + 2|y| when x >= y -- the other way round from D2Common's UNITS_GetDistanceToCoordinates, which takes
                //  |x| + 2|y| when |x| <= |y|.
                if (tParams.nTargetX >= tParams.nTargetY)
                    nLength = nAbsX + 2 * nAbsY;
                else
                    nLength = nAbsY + 2 * nAbsX;

                //  BUG: hffragment1's record is read with no NULL test.
                tParams.nRange = (nLength >> 1) * (int16_t)pFragmentTxt->wRange / 6;
                pFragment = MISSILE_ClientSpawn(&tParams);
                if (pFragment)
                {
                    D2COMMON_MISSILE_SetTargetX(pFragment, (int32_t)dwObjectGUID);

                    D2COMMON_MISSILE_SetTargetY(pFragment, (int32_t)((uint32_t)SEED_RollRandomNumber(&pMissile->pSeed) % 35) + 10);
                }
            }
        }

        VIEWEFFECTS_StartScreenShake(4, 80, 80, 400);
    }

    MISSILE_CltDoFunc1(pMissile);
}


//  The missile ids carried as literals in the bytes (0x17B, 0x17C, 0x17E; 0x154, 0x153, 0x96; 0x15C, 0x15D).
static_assert(MISSILE_IZUAL_CONTROL == 379 && MISSILE_IZUAL_MIST_LOOP == 380 && MISSILE_IZUAL_LIGHTNING == 382
                  && MISSILE_HORADRICLIGHT == 340 && MISSILE_HORADRICLIGHTNING == 339 && MISSILE_DUST == 150
                  && MISSILE_RADAMENTHANDOFGOD == 348 && MISSILE_RADAMENTHOLYBOLT == 349,
              "batch 18's missile ids");


//1.10f: D2Client.0x6FB3A130
//  pCltDoFunc 42: izual control alone (Range 200). Every frame: izual control's own row, missile 379 by a CONSTANT index (NULL when the table has 379
//  rows or fewer); two jitters of SEED_RollLimitedRandomNumber below 20, less 10 -- CALLS --; and one izual mist loop, flags 0x21, for the owner
//  (asked, not tested), at the missile's position, aimed at it plus the jitters. Its X is a copy, the other three coordinates CALLS to
//  UNITS_GetYPosition, GetXPosition and GetYPosition.
//  Then, while the frame counter (MISSILE_GetCurrentFrame, which runs DOWN) lies strictly between the row's Range less Param1 and its Range less
//  Param2 -- on the stock row between 50 and 175 -- and with an owner: one izual lightning from the missile, flags 0x20, aimed at the owner's
//  position plus two seed rolls below 10, less 5; the bolt spawned is given a second vector of Z -3 less a third roll below 3, an arc of peak 0 over
//  15 frames, and its timed motion cleared. Then MISSILE_CltDoFunc1.
//  The original inlines some of the UNITS_GetXPosition and UNITS_GetYPosition reads (the rest are calls) and SEED_RollRandomNumber here.
void __fastcall MISSILE_CltDoFunc42(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pControlTxt;
    D2UnitStrc *pOwner;
    D2UnitStrc *pBolt;
    D2SeedStrc *pSeed;
    int32_t nFrame;
    int32_t nJitterX;
    int32_t nJitterY;
    int32_t nOwnerX;
    int32_t nOwnerY;
    int32_t nBoltX;
    uint32_t nRollY;

    nFrame = D2COMMON_MISSILE_GetCurrentFrame(pMissile);
    if (sgptDataTables->nMissilesTxtRecordCount > MISSILE_IZUAL_CONTROL)
        pControlTxt = &sgptDataTables->pMissilesTxt[MISSILE_IZUAL_CONTROL];
    else
        pControlTxt = NULL;

    pSeed = &pMissile->pSeed;
    nJitterX = SEED_RollLimitedRandomNumber(pSeed, 20) - 10;
    nJitterY = SEED_RollLimitedRandomNumber(pSeed, 20) - 10;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETPOS;
    tParams.pOwner = CUNIT_GetOwnerUnit(pMissile);

    tParams.nX = UNITS_GetXPosition(pMissile);

    tParams.nY = UNITS_GetYPosition(pMissile);
    tParams.nTargetX = UNITS_GetXPosition(pMissile) + nJitterX;
    tParams.nTargetY = UNITS_GetYPosition(pMissile) + nJitterY;
    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
    tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    tParams.nMissile = MISSILE_IZUAL_MIST_LOOP;
    MISSILE_ClientSpawn(&tParams);

    //  BUG: izual control's row is read with no NULL test. The Param columns are taken as int in the signed compares (`jge`, `jle`).
    if (nFrame < (int16_t)pControlTxt->wRange - pControlTxt->dwParam[0] && nFrame > (int16_t)pControlTxt->wRange - pControlTxt->dwParam[1])
    {
        pOwner = CUNIT_GetOwnerUnit(pMissile);
        if (pOwner)
        {
            nOwnerX = UNITS_GetXPosition(pOwner);

            nBoltX = (int32_t)((uint32_t)SEED_RollRandomNumber(pSeed) % 10) + nOwnerX - 5;

            nOwnerY = UNITS_GetYPosition(pOwner);

            nRollY = (uint32_t)SEED_RollRandomNumber(pSeed) % 10;

            memset(&tParams, 0, sizeof(tParams));
            tParams.dwFlags = MISSILESPAWN_TARGETPOS;
            tParams.pOwner = pOwner;
            tParams.pOrigin = pMissile;
            tParams.nTargetX = nBoltX;
            tParams.nTargetY = (int32_t)nRollY + nOwnerY - 5;
            tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
            tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
            tParams.nMissile = MISSILE_IZUAL_LIGHTNING;
            pBolt = MISSILE_ClientSpawn(&tParams);
            if (pBolt)
            {
                PARTICLE_SetUnitGfxVector2(pBolt, 0, 0, -3 - (int32_t)((uint32_t)SEED_RollRandomNumber(pSeed) % 3), 1);
                PARTICLE_StartUnitArc(pBolt, 0, 15);
                PARTICLE_ClearUnitTimedMotion(pBolt);
            }
        }
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB3A3E0
//  pCltDoFunc 31: horadricstaff alone (Range 440). The frame counter (MISSILE_GetCurrentFrame, running DOWN) and the owner are read first; at 150 a
//  screen shake of magnitude 25, held 4000 ms, no ramps.
//  The seven lights: gnHoradricLightSteps, seven records of three dwords {X offset, Y offset, direction}: {3,-2,4} {1,-3,58} {3,1,13} {-2,-3,49}
//  {1,3,22}
//  {-3,0,40} {-2,2,31}, walked with a threshold of 325 stepping down by 25. The original steps a POINTER at each record's direction and stops it one
//  record past the seventh; here it is an index. A frame above the threshold ends the walk; a frame equal to it spawns, for an owner, one horadric
//  light, flags 1, at the missile's position (Y read first) plus the record's offsets, with the missile's skill and level; the light is turned to the
//  record's direction (its low byte, `mov dl, [ecx]`) and given the frame as both its total and its current frame.
//  At an even frame below 165 (a SIGNED % 2, `and ecx, 80000001h`): one horadric lightning, flags 0x21, at the missile's position, aimed at X - 13, Y
//  + 3 of it, given height 70, a vector of Z -1, direction 21 and its rotate flag cleared. Below frame 150: one dust, flags 0x21, at X - 13, Y + 3,
//  aimed at that plus a roll below 5, plus 1, in X and a roll below 10, less 5, in Y, at animation speed 64. Then MISSILE_CltDoFunc1.
//  The original inlines UNITS_GetXPosition, UNITS_GetYPosition and SEED_RollRandomNumber here.
void __fastcall MISSILE_CltDoFunc31(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2UnitStrc *pOwner;
    D2UnitStrc *pSpawned;
    uint64_t lSeed;
    uint32_t nRollX;
    int32_t nFrame;
    int32_t nStep;
    int32_t nThreshold;
    int32_t nSkillId;
    int32_t nLevel;
    int32_t nX;
    int32_t nY;

    nFrame = D2COMMON_MISSILE_GetCurrentFrame(pMissile);
    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (nFrame == 150)
        VIEWEFFECTS_StartScreenShake(25, 0, 4000, 0);

    for (nStep = 0, nThreshold = 325; nStep < 7; ++nStep, nThreshold -= 25)
    {
        if (nFrame > nThreshold)
            break;

        if (nFrame != nThreshold)
            continue;

        nY = UNITS_GetYPosition(pMissile);

        nX = UNITS_GetXPosition(pMissile);

        nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
        if (!pOwner)
            continue;

        memset(&tParams, 0, sizeof(tParams));
        tParams.dwFlags = MISSILESPAWN_STARTPOS;
        tParams.pOwner = pOwner;
        tParams.nSkill = nSkillId;
        tParams.nSkillLevel = nLevel;
        tParams.nMissile = MISSILE_HORADRICLIGHT;
        tParams.nX = nX + gnHoradricLightSteps[3 * nStep];
        tParams.nY = nY + gnHoradricLightSteps[3 * nStep + 1];
        pSpawned = MISSILE_ClientSpawn(&tParams);
        if (pSpawned)
        {
            D2COMMON_PATH_SetDirection(pSpawned->pDynamicPath, (uint8_t)gnHoradricLightSteps[3 * nStep + 2]);
            D2COMMON_MISSILE_SetTotalFrames(pSpawned, nFrame);
            D2COMMON_MISSILE_SetCurrentFrame(pSpawned, nFrame);
        }
    }

    if (nFrame < 165 && nFrame % 2 == 0)
    {
        memset(&tParams, 0, sizeof(tParams));
        tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETPOS;
        tParams.pOwner = pOwner;

        tParams.nX = UNITS_GetXPosition(pMissile);

        tParams.nY = UNITS_GetYPosition(pMissile);

        tParams.nTargetX = UNITS_GetXPosition(pMissile) - 13;

        tParams.nTargetY = UNITS_GetYPosition(pMissile) + 3;

        tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
        tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        tParams.nMissile = MISSILE_HORADRICLIGHTNING;
        pSpawned = MISSILE_ClientSpawn(&tParams);
        if (pSpawned)
        {
            PARTICLE_SetUnitGfxCoords(pSpawned, 0, 0, 70);
            PARTICLE_SetUnitGfxVector(pSpawned, 0, 0, -1, 1);
            D2COMMON_PATH_SetDirection(pSpawned->pDynamicPath, 21);
            D2COMMON_10203_PATH_SetRotateFlag(pSpawned->pDynamicPath, FALSE);
        }
    }

    if (nFrame < 150)
    {
        nX = UNITS_GetXPosition(pMissile) - 13;

        nY = UNITS_GetYPosition(pMissile) + 3;

        memset(&tParams, 0, sizeof(tParams));
        tParams.pOwner = pOwner;

        lSeed = (uint64_t)pMissile->pSeed.nLowSeed * 0x6AC690C5u + pMissile->pSeed.nHighSeed;
        pMissile->pSeed.nLowSeed = (uint32_t)lSeed;
        nRollX = (uint32_t)lSeed % 5;
        lSeed = (uint64_t)(uint32_t)lSeed * 0x6AC690C5u + (uint32_t)(lSeed >> 32);
        pMissile->pSeed.nHighSeed = (uint32_t)(lSeed >> 32);
        pMissile->pSeed.nLowSeed = (uint32_t)lSeed;

        tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETPOS;
        tParams.nX = nX;
        tParams.nY = nY;
        tParams.nTargetX = (int32_t)nRollX + nX + 1;
        tParams.nTargetY = (int32_t)((uint32_t)lSeed % 10) + nY - 5;
        tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
        tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        tParams.nMissile = MISSILE_DUST;
        pSpawned = MISSILE_ClientSpawn(&tParams);
        if (pSpawned)
            D2COMMON_UNITS_SetAnimationSpeed(pSpawned, 64);
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB3A7D0
//  pCltHitFunc 37: radamenthandofgod alone -- the hand of god's holy bolts. A UNIT hit answers 0. Otherwise the missile's seed is re-initialised from
//  its TargetY (D2Common's SEED_InitLowSeed), and TargetX is the count: pCltDoFunc 32 stores both, the frame and the stage's count. With the
//  missile's position (X read first), one block, flags 0x21, for the owner (asked, not tested), of radamentholybolt; per bolt, two seed rolls below
//  20, less 10, aim it around the position; a bolt aimed at the position itself is not spawned and the counter is stepped BACK (`dec`), so the count
//  is of bolts spawned; each bolt spawned starts a seeded path from its TargetX (SKILLS_MissileStartSeededPath). Answers 1.
//  The original inlines UNITS_GetXPosition, UNITS_GetYPosition and SEED_RollRandomNumber here.
int32_t __fastcall MISSILE_CltHitFunc37(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissileStrc tParams;
    D2SeedStrc *pSeed;
    D2UnitStrc *pBolt;
    uint64_t lSeed;
    int32_t nBolts;
    int32_t nBolt;
    int32_t nX;
    int32_t nY;

    if (pTarget)
        return 0;

    pSeed = &pMissile->pSeed;
    D2COMMON_SEED_InitLowSeed(pSeed, D2COMMON_MISSILE_GetTargetY(pMissile));
    nBolts = D2COMMON_MISSILE_GetTargetX(pMissile);

    nX = UNITS_GetXPosition(pMissile);

    nY = UNITS_GetYPosition(pMissile);

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETPOS;
    tParams.pOwner = CUNIT_GetOwnerUnit(pMissile);
    tParams.nX = nX;
    tParams.nY = nY;
    tParams.nMissile = MISSILE_RADAMENTHOLYBOLT;
    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
    tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);

    for (nBolt = 0; nBolt < nBolts; ++nBolt)
    {
        lSeed = (uint64_t)pSeed->nLowSeed * 0x6AC690C5u + pSeed->nHighSeed;
        pSeed->nLowSeed = (uint32_t)lSeed;
        tParams.nTargetX = (int32_t)((uint32_t)lSeed % 20) + nX - 10;
        lSeed = (uint64_t)(uint32_t)lSeed * 0x6AC690C5u + (uint32_t)(lSeed >> 32);
        pSeed->nHighSeed = (uint32_t)(lSeed >> 32);
        pSeed->nLowSeed = (uint32_t)lSeed;
        tParams.nTargetY = nY + (int32_t)((uint32_t)lSeed % 20) - 10;

        if (tParams.nTargetX == nX && tParams.nTargetY == nY)
        {
            --nBolt;
            continue;
        }

        pBolt = MISSILE_ClientSpawn(&tParams);
        if (pBolt)
            SKILLS_MissileStartSeededPath(pBolt, tParams.nTargetX);
    }

    return 1;
}


//1.10f: D2Client.0x6FB3A960
//  pCltDoFunc 32: radamentdeath alone (Range 400) -- Radament's death. The frame counter (running DOWN) and the owner first. The stages:
//  gRadamentDeathFx, six records {frame bound, period, bolt count}: {300, 25, 1} {250, 10, 3} {100, 8, 4} {75, 6, 4} {30, 4, 3} and {0, 100, 0}. The
//  original walks the first five and stops at the first whose bound the frame is ABOVE; when none is -- the last 30 frames -- the walk ends on the
//  SIXTH and reads it: a period of 100 and a count of 0.
//  When the frame is a multiple of the stage's period (a SIGNED remainder, `cdq / idiv`), with an owner: one radamenthandofgod, flags 1, at the
//  missile's position (Y read first), with its skill and level; the hand spawned takes the stage's count as TargetX and the frame as TargetY --
//  pCltHitFunc 37's count and seed. Then MISSILE_CltDoFunc1.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall MISSILE_CltDoFunc32(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2UnitStrc *pOwner;
    D2UnitStrc *pHand;
    int32_t nFrame;
    int32_t nStage;
    int32_t nSkillId;
    int32_t nLevel;
    int32_t nX;
    int32_t nY;

    nFrame = D2COMMON_MISSILE_GetCurrentFrame(pMissile);
    pOwner = CUNIT_GetOwnerUnit(pMissile);

    for (nStage = 0; nStage < 5; ++nStage)
    {
        if (nFrame > gRadamentDeathFx[nStage].nFrameBound)
            break;
    }

    if (nFrame % gRadamentDeathFx[nStage].nPeriod != 0)
    {
        MISSILE_CltDoFunc1(pMissile);
        return;
    }

    nY = UNITS_GetYPosition(pMissile);

    nX = UNITS_GetXPosition(pMissile);

    nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
    if (pOwner)
    {
        memset(&tParams, 0, sizeof(tParams));
        tParams.nY = nY;
        tParams.dwFlags = MISSILESPAWN_STARTPOS;
        tParams.pOwner = pOwner;
        tParams.nX = nX;
        tParams.nSkill = nSkillId;
        tParams.nSkillLevel = nLevel;
        tParams.nMissile = MISSILE_RADAMENTHANDOFGOD;
        pHand = MISSILE_ClientSpawn(&tParams);
        if (pHand)
        {
            D2COMMON_MISSILE_SetTargetX(pHand, gRadamentDeathFx[nStage].nParam);
            D2COMMON_MISSILE_SetTargetY(pHand, nFrame);
        }
    }

    MISSILE_CltDoFunc1(pMissile);
}


//  The missile ids carried as literals (0x15F, 0x160, 0x162), and the two splats pCltHitFunc 39 reaches as the glob's class plus 1 or 2 -- no
//  literal, so the order is what is pinned.
static_assert(MISSILE_TAINTEDSUNCONTROL == 350 && MISSILE_TAINTEDSUNFLASH == 351 && MISSILE_TAINTEDSUNBALL == 352
                  && MISSILE_QUEENDEATHCENTER == 353 && MISSILE_QUEENDEATHGLOB == 354
                  && MISSILE_QUEENDEATHSPLAT1 == MISSILE_QUEENDEATHGLOB + 1
                  && MISSILE_QUEENDEATHSPLAT2 == MISSILE_QUEENDEATHGLOB + 2,
              "batch 19's missile ids");
static_assert(OVERLAY_HORADRIC_LIGHT == 182, "MISSILE_CltDoFunc33: `mov edx, 0B6h` (0x6FB3AC98)");


//1.10f: D2Client.0x6FB3AAA0
//  pCltDoFunc 33: taintedsuncontrol alone (Range 300) -- the Tainted Sun's control, which QUESTSFX_TriggerTaintedSun spawns at object 149
//  taintedsunaltar with the altar's unit id in TargetY. The frame counter first (running DOWN). The stages: gTaintedSunFx, five records {frame bound,
//  period, flash count}:
//  {350, 10, 1} {300, 5, 1} {200, 4, 1} {175, 3, 1} and {0, 100, 0}. The original walks the first four and stops at the first whose bound the frame
//  is ABOVE; when none is, the walk ends on the FIFTH: a period of 100 and a count of 0. The row's Range is 300, so unless the spawn sets a longer
//  one the first two records never answer.
//  When the frame is a multiple of the stage's period (a SIGNED remainder, `cdq / idiv`): the control's position (X read first), one block, flags 1,
//  for the owner (asked, not tested), of taintedsunflash, with the skill -- MISSILE_GetSkill is CALLED TWICE into the one field, reproduced -- and
//  the level. Per flash, two seed rolls, each below 10 less 5, and 5 where that lands within one of zero (`cmp eax, 2 / jge` on its absolute value),
//  doubled and added to the position; each flash spawned takes the control's unit id as TargetX, by which pCltHitFunc 38 finds the control again.
//  Then, every frame: TargetX 3 below frame 25, else 2 below 75 -- the phase pCltDoFunc 34 reads; and on frame 1 or below, the altar by the id in
//  TargetY: overlay 182 horadric_light, looping, and, when it carries no light yet (pLight, +0x64), a light of type 2 (LIGHT_TYPE_STATIC), radius 60,
//  intensity and colour all 255. Then MISSILE_CltDoFunc1.
//  The original inlines UNITS_GetXPosition, UNITS_GetYPosition and SEED_RollRandomNumber here.
void __fastcall MISSILE_CltDoFunc33(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2UnitStrc *pFlash;
    D2UnitStrc *pAltar;
    int32_t nFrame;
    int32_t nStage;
    int32_t nFlash;
    int32_t nJitterX;
    int32_t nJitterY;
    int32_t nAbs;
    int32_t nX;
    int32_t nY;

    nFrame = D2COMMON_MISSILE_GetCurrentFrame(pMissile);

    for (nStage = 0; nStage < 4; ++nStage)
    {
        if (nFrame > gTaintedSunFx[nStage].nFrameBound)
            break;
    }

    if (nFrame % gTaintedSunFx[nStage].nPeriod == 0)
    {
        nX = UNITS_GetXPosition(pMissile);

        nY = UNITS_GetYPosition(pMissile);

        memset(&tParams, 0, sizeof(tParams));
        tParams.dwFlags = MISSILESPAWN_STARTPOS;
        tParams.pOwner = CUNIT_GetOwnerUnit(pMissile);
        tParams.nMissile = MISSILE_TAINTEDSUNFLASH;
        //  Asked twice and stored twice into the one field, as the original does.
        tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
        tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
        tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);

        for (nFlash = 0; nFlash < gTaintedSunFx[nStage].nParam; ++nFlash)
        {
            nJitterX = (int32_t)((uint32_t)SEED_RollRandomNumber(&pMissile->pSeed) % 10) - 5;
            nAbs = nJitterX;
            if (nAbs < 0)
                nAbs = -nAbs;
            if (nAbs < 2)
                nJitterX = 5;

            nJitterY = (int32_t)((uint32_t)SEED_RollRandomNumber(&pMissile->pSeed) % 10) - 5;
            nAbs = nJitterY;
            if (nAbs < 0)
                nAbs = -nAbs;
            if (nAbs < 2)
                nJitterY = 5;

            tParams.nX = nX + 2 * nJitterX;
            tParams.nY = nY + 2 * nJitterY;
            pFlash = MISSILE_ClientSpawn(&tParams);
            if (pFlash)
                D2COMMON_MISSILE_SetTargetX(pFlash, (int32_t)pMissile->dwUnitId);
        }
    }

    if (nFrame < 25)
        D2COMMON_MISSILE_SetTargetX(pMissile, 3);
    else if (nFrame < 75)
        D2COMMON_MISSILE_SetTargetX(pMissile, 2);

    if (nFrame <= 1)
    {
        pAltar = CUNIT_FindUnitByGuidAndType((uint32_t)D2COMMON_MISSILE_GetTargetY(pMissile), UNIT_OBJECT);
        if (pAltar)
        {
            GFX_AddOverlay(pAltar, OVERLAY_HORADRIC_LIGHT, OVERLAYTYPE_LOOP, 0, 0, 0, 0, 0);
            if (!pAltar->pLight)
                pAltar->pLight = DLIGHTMAP_AddLight(pAltar, 2, 60, 255, 255, 255, 255);
        }
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB3ACE0
//  pCltHitFunc 38: taintedsunflash alone (Range 12). A UNIT hit answers 0. Otherwise the control, by the id pCltDoFunc 33 left in the flash's
//  TargetX, looked up in the client-id table as a missile (CUNIT_FindUnitByClientIdAndType); none, and it answers 1 having spawned nothing. With it:
//  one block, flags 0xA021, for the flash's owner (asked, not tested), of taintedsunball, with the skill and the level, a range of the control's
//  current frame (MISSILE_GetCurrentFrame, the counter pCltDoFunc 33 reads), from the FLASH's position and aimed at the CONTROL's (each read X
//  first); the ball spawned takes the control's unit id as TargetX. Answers 1.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall MISSILE_CltHitFunc38(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissileStrc tParams;
    D2UnitStrc *pControl;
    D2UnitStrc *pBall;

    if (pTarget)
        return 0;

    pControl = CUNIT_FindUnitByClientIdAndType(D2COMMON_MISSILE_GetTargetX(pMissile), UNIT_MISSILE);
    if (pControl)
    {
        memset(&tParams, 0, sizeof(tParams));
        tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETPOS | MISSILESPAWN_RANDOMDIR | MISSILESPAWN_RANGE;
        tParams.pOwner = CUNIT_GetOwnerUnit(pMissile);
        tParams.nMissile = MISSILE_TAINTEDSUNBALL;
        tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
        tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        tParams.nRange = D2COMMON_MISSILE_GetCurrentFrame(pControl);

        tParams.nX = UNITS_GetXPosition(pMissile);

        tParams.nY = UNITS_GetYPosition(pMissile);

        tParams.nTargetX = UNITS_GetXPosition(pControl);

        tParams.nTargetY = UNITS_GetYPosition(pControl);

        pBall = MISSILE_ClientSpawn(&tParams);
        if (pBall)
            D2COMMON_MISSILE_SetTargetX(pBall, (int32_t)pControl->dwUnitId);
    }

    return 1;
}


//1.10f: D2Client.0x6FB3AE40
//  pCltDoFunc 34: taintedsunball alone (Range 300) -- the balls pCltHitFunc 38 spawns, driven by the control's phase. The ball's own STATE, its
//  TargetY, first -- 0 at the spawn, which sets no TargetY and whose missile data D2Common clears. No dynamic path, or no control by the id in its
//  TargetX (the client-id table, as a missile), and only MISSILE_CltDoFunc1 runs. Then the control's PHASE, its TargetX -- pCltDoFunc 33 writes 2
//  below frame 75 and 3 below 25, and no other value -- the ball's distance to the control (UNITS_GetDistanceToOtherUnit), and a seed roll below 5,
//  raised to 1 when it is 1 or less (`cmp edx, 1 / jg`, SIGNED), taken on every frame that gets this far.
//  A new phase -- above 1 and not the state -- is stored as the state and started once. 2: velocity 0, the particle flag, gfx vector (0, 0, 6,
//  scaled), particle vector (0, 0, 170 + a roll below 40), the target ignored; then aimed at the control (Y read first) at velocity 768 and started.
//  3: the particle flag, gfx vector (0, 0, -6, scaled), particle vector (0, 0, a roll below 40), the target no longer ignored, started on the path it
//  has. 4: the particle flag, gfx vector (0, 0, 20, scaled), particle vector (0, 0, 400), ignored, started -- written as the original has it;
//  pCltDoFunc 33 never writes 4. Otherwise: a ball in state 0 nearer the control than the roll moves to state 1 (the particle flag, gfx vector (0, 0,
//  1, scaled)); in state 1, unless it sits on the control, it is steered at its own position plus its offset from the control turned a quarter --
//  (ballX - dy, ballY + dx) with (dx, dy) the ball less the control -- at velocity 2560; in state 3 it is aimed at the control at velocity 512, the
//  control's X by a CALL of UNITS_GetXPosition where every other read here is inlined. Every path ends in a tail jump to MISSILE_CltDoFunc1.
//  The original inlines UNITS_GetXPosition, UNITS_GetYPosition and SEED_RollRandomNumber here.
void __fastcall MISSILE_CltDoFunc34(D2UnitStrc *pMissile)
{
    D2DynamicPathStrc *pPath;
    D2UnitStrc *pControl;
    int32_t nState;
    int32_t nPhase;
    int32_t nDistance;
    int32_t nRoll;
    int32_t nDeltaX;
    int32_t nDeltaY;
    int32_t nX;
    int32_t nY;

    nState = D2COMMON_MISSILE_GetTargetY(pMissile);
    pPath = pMissile->pDynamicPath;
    if (!pPath)
    {
        MISSILE_CltDoFunc1(pMissile);
        return;
    }

    pControl = CUNIT_FindUnitByClientIdAndType(D2COMMON_MISSILE_GetTargetX(pMissile), UNIT_MISSILE);
    if (!pControl)
    {
        MISSILE_CltDoFunc1(pMissile);
        return;
    }

    nPhase = D2COMMON_MISSILE_GetTargetX(pControl);
    nDistance = D2COMMON_UNITS_GetDistanceToOtherUnit(pMissile, pControl);

    nRoll = (int32_t)((uint32_t)SEED_RollRandomNumber(&pMissile->pSeed) % 5);
    if (nRoll <= 1)
        nRoll = 1;

    if (nState != nPhase && nPhase > 1)
    {
        D2COMMON_MISSILE_SetTargetY(pMissile, nPhase);
        switch (nPhase)
        {
        case 2:
            D2COMMON_PATH_SetVelocity(pPath, 0, szMissileCpp, 6004);
            PARTICLE_SetUnitParticleFlag(pMissile);
            PARTICLE_SetUnitGfxVector(pMissile, 0, 0, 6, 1);
            PARTICLE_SetUnitParticleVector(pMissile, 0, 0, (int32_t)((uint32_t)SEED_RollRandomNumber(&pMissile->pSeed) % 40) + 170, 0);
            PARTICLE_SetUnitRisesToTarget(pMissile, 1);

            nY = UNITS_GetYPosition(pControl);

            nX = UNITS_GetXPosition(pControl);

            D2COMMON_10170_PathSetTargetPos(pPath, nX, nY);
            D2COMMON_PATH_SetVelocity(pPath, 768, szMissileCpp, 6012);
            D2COMMON_PATH_StartUnitMove(pPath, pMissile, 0);
            break;

        case 3:
            PARTICLE_SetUnitParticleFlag(pMissile);
            PARTICLE_SetUnitGfxVector(pMissile, 0, 0, -6, 1);
            PARTICLE_SetUnitParticleVector(pMissile, 0, 0, (int32_t)((uint32_t)SEED_RollRandomNumber(&pMissile->pSeed) % 40), 0);
            PARTICLE_SetUnitRisesToTarget(pMissile, 0);
            D2COMMON_PATH_StartUnitMove(pPath, pMissile, 0);
            break;

        case 4:
            PARTICLE_SetUnitParticleFlag(pMissile);
            PARTICLE_SetUnitGfxVector(pMissile, 0, 0, 20, 1);
            PARTICLE_SetUnitParticleVector(pMissile, 0, 0, 400, 0);
            PARTICLE_SetUnitRisesToTarget(pMissile, 1);
            D2COMMON_PATH_StartUnitMove(pPath, pMissile, 0);
            break;
        }

        MISSILE_CltDoFunc1(pMissile);
        return;
    }

    if (nDistance < nRoll && nState == 0)
    {
        D2COMMON_MISSILE_SetTargetY(pMissile, 1);
        PARTICLE_SetUnitParticleFlag(pMissile);
        PARTICLE_SetUnitGfxVector(pMissile, 0, 0, 1, 1);
    }
    else if (nState == 1)
    {
        nX = UNITS_GetXPosition(pMissile);
        nDeltaX = nX - UNITS_GetXPosition(pControl);

        nY = UNITS_GetYPosition(pMissile);
        nDeltaY = nY - UNITS_GetYPosition(pControl);

        if (nDeltaX || nDeltaY)
        {
            nY = UNITS_GetYPosition(pMissile);
            nX = UNITS_GetXPosition(pMissile);

            D2COMMON_10170_PathSetTargetPos(pPath, nX - nDeltaY, nY + nDeltaX);
            D2COMMON_PATH_SetVelocity(pPath, 2560, szMissileCpp, 6046);
            D2COMMON_PATH_StartUnitMove(pPath, pMissile, 0);
        }
    }
    else if (nState == 3)
    {
        nY = UNITS_GetYPosition(pControl);
        D2COMMON_10170_PathSetTargetPos(pPath, UNITS_GetXPosition(pControl), nY);
        D2COMMON_PATH_SetVelocity(pPath, 512, szMissileCpp, 6055);
        D2COMMON_PATH_StartUnitMove(pPath, pMissile, 0);
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB3B280
//  pCltDoFunc 35: queendeathcenter alone (Range 100), which QUESTSFX_OnMaggotQueenDeath spawns at the Maggot Queen. On every fourth frame -- a SIGNED
//  `% 4`, built from `and eax, 80000003h` and the `jns / dec / or / inc` fix-up -- one queendeathglob: flags 0x2521, for the owner (asked, not
//  tested), with the skill and the level, from the missile's position (X read first) aimed at X less a roll below 12 and Y plus a roll below 13, less
//  6. Its answer is not kept. Then MISSILE_CltDoFunc1.
//  The original inlines UNITS_GetXPosition, UNITS_GetYPosition and SEED_RollRandomNumber here.
void __fastcall MISSILE_CltDoFunc35(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    int32_t nX;
    int32_t nY;

    if (D2COMMON_MISSILE_GetCurrentFrame(pMissile) % 4 == 0)
    {
        memset(&tParams, 0, sizeof(tParams));
        tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETPOS | MISSILESPAWN_ARC | MISSILESPAWN_FRAMESTOTARGET | MISSILESPAWN_RANDOMDIR;
        tParams.pOwner = CUNIT_GetOwnerUnit(pMissile);
        tParams.nMissile = MISSILE_QUEENDEATHGLOB;
        tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
        tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);

        nX = UNITS_GetXPosition(pMissile);
        tParams.nX = nX;

        nY = UNITS_GetYPosition(pMissile);
        tParams.nY = nY;

        tParams.nTargetX = nX - (int32_t)((uint32_t)SEED_RollRandomNumber(&pMissile->pSeed) % 12);
        tParams.nTargetY = (int32_t)((uint32_t)SEED_RollRandomNumber(&pMissile->pSeed) % 13) + nY - 6;
        MISSILE_ClientSpawn(&tParams);
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB3B3B0
//  pCltHitFunc 39: queendeathglob alone (Range 20). A seed roll first; an ODD one answers 1 and spawns nothing (`and edx, 1 / jg`). An even one: the
//  glob's class id, or -1 for a NULL glob; a second roll, whose parity makes the splat the class plus 1 or plus 2 -- queendeathsplat1 or
//  queendeathsplat2; the level, then the skill. With the glob (tested again) and an owner -- the glob itself were it a player or a monster, else
//  CUNIT_GetOwnerUnit's answer, and none answers 1 -- one block, flags 0x20, the glob as origin, TargetX and TargetY written 0 over the clear; the
//  splat spawned takes the glob's direction, with PATH_SetRotateFlag's flag cleared. The unit hit is never read. Answers 1.
//  The original inlines SEED_RollRandomNumber here.
int32_t __fastcall MISSILE_CltHitFunc39(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissileStrc tParams;
    D2UnitStrc *pOwner;
    D2UnitStrc *pSplat;
    D2DynamicPathStrc *pSplatPath;
    int32_t nOdd;
    int32_t nClassId;
    int32_t nSplat;
    int32_t nLevel;
    int32_t nSkillId;

    UNREFERENCED_PARAMETER(pTarget);

    nOdd = (int32_t)((uint32_t)SEED_RollRandomNumber(&pMissile->pSeed) & 1);
    if (nOdd > 0)
        return 1;

    //  BUG: a NULL test of the glob after its seed was already used (an inlined accessor's own check). UNREACHABLE: the use above would have faulted
    //  first.
    nClassId = pMissile ? pMissile->dwClassId : -1;

    nSplat = (int32_t)((uint32_t)SEED_RollRandomNumber(&pMissile->pSeed) & 1) + nClassId + 1;
    nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);

    //  BUG: the same NULL test again. UNREACHABLE: as above.
    if (!pMissile)
        return 1;

    if (pMissile->dwUnitType == UNIT_PLAYER || pMissile->dwUnitType == UNIT_MONSTER)
    {
        pOwner = pMissile;
    }
    else
    {
        pOwner = CUNIT_GetOwnerUnit(pMissile);
        if (!pOwner)
            return 1;
    }

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_TARGETPOS;
    tParams.pOwner = pOwner;
    tParams.pOrigin = pMissile;
    tParams.nTargetX = 0;
    tParams.nTargetY = 0;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nLevel;
    tParams.nMissile = nSplat;
    pSplat = MISSILE_ClientSpawn(&tParams);
    if (pSplat)
    {
        pSplatPath = pSplat->pDynamicPath;
        D2COMMON_PATH_SetDirection(pSplatPath, D2COMMON_PATH_GetDirection(pMissile->pDynamicPath));
        D2COMMON_10203_PATH_SetRotateFlag(pSplatPath, FALSE);
    }

    return 1;
}


//  The missile ids carried as literals (0x2A, 0x16C, 0x16D, 0x16E, 0x1A0, 0x1A6, 0x11D, 0x269, 0x26A and 0x25F..0x263 in the bytes).
static_assert(MISSILE_EXPLODINGARROWEXP == 42 && MISSILE_DURIELDEATHROCK == 364 && MISSILE_DURIELDEATHDEBRIS == 365
                  && MISSILE_DURIELDEATHSMOKE == 366 && MISSILE_CATAPULT_COLD_BALL == 416
                  && MISSILE_CATAPULT_METEOR_BALL == 422 && MISSILE_BRDEATHLIGHTNINGBOLT == 285
                  && MISSILE_NEHLITHAKHOLE == 617 && MISSILE_NEHLITHAKHOLELIGHT == 618
                  && MISSILE_NEHLITHAKSWOOSH == 607 && MISSILE_NEHLITHAKDEBRIS1 == 608
                  && MISSILE_NEHLITHAKDEBRIS2 == 609 && MISSILE_NEHLITHAKDEBRIS3 == 610
                  && MISSILE_NEHLITHAKDEBRIS4 == 611,
              "batch 20's missile ids");


//1.10f: D2Client.0x6FB3B4B0
//  pCltDoFunc 36: durieldeathcontrol alone (Range 400), which QUESTSFX_OnDurielDeath spawns at Duriel, owned by the player. The frame counter first
//  (running DOWN); on frame 325 exactly, a screen shake (VIEWEFFECTS_StartScreenShake: magnitude 20, no ramp up, 6000 held, no ramp down). The
//  stages: gDurielDeathFx, five records {frame bound, first period, second period}: {350, 3, 5} {300, 2, 3} {200, 3, 2} {175, 5, 1} and {0, 100, 5}.
//  The original walks the first four and stops at the first whose bound the frame is ABOVE; when none is, the walk ends on the FIFTH.
//  The missile's position (X read first) is read on every frame, before either test. On a frame that is a multiple of the first period (SIGNED, `cdq
//  / idiv`): one explodingarrowexp, flags 0x2001, for the owner (asked, not tested), with the skill and the level, at the position moved by two seed
//  rolls below 40, less 20. On a multiple of the second: one durieldeathrock, the same block and rolls; the rock spawned gets gfx coords (0, 0, 150),
//  gfx vector (0, 0,
//      -10,
//  scaled), 15 frames total and current, and animation speed 128. Then MISSILE_CltDoFunc1.
//  The original inlines UNITS_GetXPosition, UNITS_GetYPosition and SEED_RollRandomNumber here.
void __fastcall MISSILE_CltDoFunc36(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2UnitStrc *pRock;
    uint64_t lSeed;
    int32_t nFrame;
    int32_t nStage;
    int32_t nX;
    int32_t nY;

    nFrame = D2COMMON_MISSILE_GetCurrentFrame(pMissile);
    if (nFrame == 325)
        VIEWEFFECTS_StartScreenShake(20, 0, 6000, 0);

    for (nStage = 0; nStage < 4; ++nStage)
    {
        if (nFrame > gDurielDeathFx[nStage].nFrameBound)
            break;
    }

    nX = UNITS_GetXPosition(pMissile);

    nY = UNITS_GetYPosition(pMissile);

    if (nFrame % gDurielDeathFx[nStage].nPeriod == 0)
    {
        memset(&tParams, 0, sizeof(tParams));
        tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_RANDOMDIR;
        tParams.pOwner = CUNIT_GetOwnerUnit(pMissile);
        tParams.nMissile = MISSILE_EXPLODINGARROWEXP;
        tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
        tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);

        lSeed = (uint64_t)pMissile->pSeed.nLowSeed * 0x6AC690C5u + pMissile->pSeed.nHighSeed;
        pMissile->pSeed.nLowSeed = (uint32_t)lSeed;
        tParams.nX = nX + (int32_t)((uint32_t)lSeed % 40) - 20;
        lSeed = (uint64_t)(uint32_t)lSeed * 0x6AC690C5u + (uint32_t)(lSeed >> 32);
        pMissile->pSeed.nHighSeed = (uint32_t)(lSeed >> 32);
        pMissile->pSeed.nLowSeed = (uint32_t)lSeed;
        tParams.nY = (int32_t)((uint32_t)lSeed % 40) + nY - 20;
        MISSILE_ClientSpawn(&tParams);
    }

    if (nFrame % gDurielDeathFx[nStage].nParam == 0)
    {
        memset(&tParams, 0, sizeof(tParams));
        tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_RANDOMDIR;
        tParams.pOwner = CUNIT_GetOwnerUnit(pMissile);
        tParams.nMissile = MISSILE_DURIELDEATHROCK;
        tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
        tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);

        lSeed = (uint64_t)pMissile->pSeed.nLowSeed * 0x6AC690C5u + pMissile->pSeed.nHighSeed;
        pMissile->pSeed.nLowSeed = (uint32_t)lSeed;
        tParams.nX = nX + (int32_t)((uint32_t)lSeed % 40) - 20;
        lSeed = (uint64_t)(uint32_t)lSeed * 0x6AC690C5u + (uint32_t)(lSeed >> 32);
        pMissile->pSeed.nHighSeed = (uint32_t)(lSeed >> 32);
        pMissile->pSeed.nLowSeed = (uint32_t)lSeed;
        tParams.nY = (int32_t)((uint32_t)lSeed % 40) + nY - 20;
        pRock = MISSILE_ClientSpawn(&tParams);
        if (pRock)
        {
            PARTICLE_SetUnitGfxCoords(pRock, 0, 0, 150);
            PARTICLE_SetUnitGfxVector(pRock, 0, 0, -10, 1);
            D2COMMON_MISSILE_SetTotalFrames(pRock, 15);
            D2COMMON_MISSILE_SetCurrentFrame(pRock, 15);
            D2COMMON_MISSILE_SetAnimSpeed(pRock, 128);
        }
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB3B720
//  pCltHitFunc 41: durieldeathrock alone (Range 40) -- the rock pCltDoFunc 36 drops. A UNIT hit answers 0. Otherwise: the level and the skill (asked
//  in that order), and three durieldeathdebris scattered within 6 of the rock's aim point (SKILLS_SpawnClientEffectScatter); the level and the skill
//  asked again; and, with the rock (tested) and an owner -- the rock itself were it a player or a monster, else CUNIT_GetOwnerUnit's answer, and none
//  answers 1 -- one durieldeathsmoke, flags 0x20, the rock as origin, TargetX and TargetY written 0 over the clear. Its answer is not kept. Answers
//  1.
int32_t __fastcall MISSILE_CltHitFunc41(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissileStrc tParams;
    D2UnitStrc *pOwner;
    int32_t nLevel;
    int32_t nSkillId;

    if (pTarget)
        return 0;

    nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
    SKILLS_SpawnClientEffectScatter(pMissile, MISSILE_DURIELDEATHDEBRIS, 3, 6, (D2C_Skills)nSkillId, nLevel);

    nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);

    //  BUG: the NULL test of the rock comes after SKILLS_SpawnClientEffectScatter has used it (it seeds from `&pUnit->pSeed`). UNREACHABLE: that use
    //  would have faulted first.
    if (!pMissile)
        return 1;

    if (pMissile->dwUnitType == UNIT_PLAYER || pMissile->dwUnitType == UNIT_MONSTER)
    {
        pOwner = pMissile;
    }
    else
    {
        pOwner = CUNIT_GetOwnerUnit(pMissile);
        if (!pOwner)
            return 1;
    }

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_TARGETPOS;
    tParams.pOwner = pOwner;
    tParams.pOrigin = pMissile;
    tParams.nTargetX = 0;
    tParams.nTargetY = 0;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nLevel;
    tParams.nMissile = MISSILE_DURIELDEATHSMOKE;
    MISSILE_ClientSpawn(&tParams);

    return 1;
}


//1.10f: D2Client.0x6FB3B7D0
//  pCltDoFunc 61: overseercontrol alone (Range 500), which QUESTSFX_OnOverseerDeath spawns at the dying Overseer, owned by him, with TargetX 25 -- a
//  countdown.
//  The owner and the countdown first; with no owner, only MISSILE_CltDoFunc1. A countdown above 0 is stepped down by one. At 0: a seed roll picks the
//  shot -- odd, catapult cold ball; even, catapult meteor ball -- MISSILE_LaunchCatapultShot fires it for the owner, and the countdown restarts from
//  the frames the control has LIVED (MISSILE_GetRemainingFrames): 20 below 150, 15 below 200, 10 below 225, 25 from 375 on and 5 between, each stored
//  less one. The original loads each delay and decrements it, and picks 25 or 5 without a branch (`setnl / dec / and 0FFFFFFECh / add 19h`). Every
//  path ends in a tail jump to MISSILE_CltDoFunc1.
//  The original inlines SEED_RollRandomNumber here.
void __fastcall MISSILE_CltDoFunc61(D2UnitStrc *pMissile)
{
    D2UnitStrc *pOwner;
    int32_t nDelay;
    int32_t nLived;
    int32_t nShot;

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    nDelay = D2COMMON_MISSILE_GetTargetX(pMissile);
    if (!pOwner)
    {
        MISSILE_CltDoFunc1(pMissile);
        return;
    }

    if (!nDelay)
    {
        nShot = ((uint32_t)SEED_RollRandomNumber(&pMissile->pSeed) & 1) ? MISSILE_CATAPULT_COLD_BALL : MISSILE_CATAPULT_METEOR_BALL;
        MISSILE_LaunchCatapultShot(pOwner, pMissile, nShot);

        nLived = D2COMMON_MISSILE_GetRemainingFrames(pMissile);
        if (nLived < 150)
            nDelay = 20;
        else if (nLived < 200)
            nDelay = 15;
        else if (nLived < 225)
            nDelay = 10;
        else
            nDelay = nLived >= 375 ? 25 : 5;
    }

    D2COMMON_MISSILE_SetTargetX(pMissile, nDelay - 1);
    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB3B8C0
//  One catapult shot, for pCltDoFunc 61, for the owner in pMonster; the missile id is the one stack argument. The owner's direction first, D2Common
//  #11066 (one of four fixed pairs by the monster's chain index); a zero one asserts ("nDX || nDY", line 6224) and exits. One block, flags 1, owner
//  pMonster, the id, the control's skill and level, from the control's position moved by a seed roll below 20, less 10, on each axis (X, its roll, Y,
//  its roll, in that order). The shot spawned: its missiles.txt record must have a Range of 25 ("SHENK_CATAPULT_FRAMES == ptStats->nRange", line
//  6241; both shots' rows do) or it asserts and exits. Then, with the direction (dX, dY): gfx coords (-25 dX, -25 dY), 25 subtiles against the
//  direction, through DUNGEON_GameSubtileToClientCoords, at height 400; gfx vector (dX, dY) through the same conversion, -16, scaled; and
//  PARTICLE_UpdateUnitParticle.
//  The original inlines UNITS_GetXPosition, UNITS_GetYPosition, SEED_RollRandomNumber and the Missiles.txt record lookup here.
void __fastcall MISSILE_LaunchCatapultShot(D2UnitStrc *pMonster, D2UnitStrc *pMissile, int32_t nMissileId)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pShot;
    int32_t nDirX;
    int32_t nDirY;
    int32_t nStartX;
    int32_t nStartY;
    int32_t nVectorX;
    int32_t nVectorY;
    int32_t nX;
    int32_t nY;

    nDirX = 0;
    nDirY = 0;
    D2COMMON_11066(pMonster, &nDirX, &nDirY);
    if (!nDirX && !nDirY)
    {
        FOG_DisplayAssert("nDX || nDY", szMissileCpp, 6224);
        exit(-1);
    }

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS;
    tParams.pOwner = pMonster;
    tParams.nMissile = nMissileId;
    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
    tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);

    nX = UNITS_GetXPosition(pMissile);

    tParams.nX = (int32_t)((uint32_t)SEED_RollRandomNumber(&pMissile->pSeed) % 20) + nX - 10;

    nY = UNITS_GetYPosition(pMissile);

    tParams.nY = (int32_t)((uint32_t)SEED_RollRandomNumber(&pMissile->pSeed) % 20) + nY - 10;

    pShot = MISSILE_ClientSpawn(&tParams);
    if (!pShot)
        return;

    pMissilesTxt = NULL;
    if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
        pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    //  BUG: no NULL test on the record (the lookup gives NULL for an id out of range). LATENT: its one caller passes only the two catapult ball ids.
    if (pMissilesTxt->wRange != 25)
    {
        FOG_DisplayAssert("SHENK_CATAPULT_FRAMES == ptStats->nRange", szMissileCpp, 6241);
        exit(-1);
    }

    nVectorX = nDirX;
    nStartX = -25 * nDirX;
    nVectorY = nDirY;
    nStartY = -25 * nDirY;
    D2COMMON_DUNGEON_GameSubtileToClientCoords(&nStartX, &nStartY);
    D2COMMON_DUNGEON_GameSubtileToClientCoords(&nVectorX, &nVectorY);
    PARTICLE_SetUnitGfxCoords(pShot, nStartX, nStartY, 400);
    PARTICLE_SetUnitGfxVector(pShot, nVectorX, nVectorY, -16, 1);
    PARTICLE_UpdateUnitParticle(pShot);
}


//1.10f: D2Client.0x6FB3BAF0
//  pCltDoFunc 62: nehlithakcontrol alone (Range 224), which QUESTSFX_TriggerNihlathak spawns at Nihlathak, owned by the player. The owner and the
//  frames LIVED first (MISSILE_GetRemainingFrames); with no owner, only MISSILE_CltDoFunc1. Every frame, two pieces of debris
//  (MISSILE_SpawnNihlathakDebris). Past frame 60, on every 20th (a SIGNED remainder): one brdeathlightningbolt, flags 2, for the owner, the control
//  as origin, aimed at an OFFSET -- bit 1 of the flags, MISSILE_ClientSpawn's `test al, 2`, adds nTargetX and nTargetY to the spawn point -- of 20
//  less a roll below 40 on X, where SEED_RollRandomNumber is CALLED and not inlined, and 20 less SEED_RollLimitedRandomNumber's roll below 40 on Y;
//  with the skill and the level; an offset of (0, 0) takes 1 plus a limited roll below 19 on X. The bolt spawned takes the offset as its TargetX and
//  TargetY, and gfx coords (0, 0, -10). On frame 25 lived: nehlithakholelight and then nehlithakhole, each flags 1, for the owner, at the control's
//  position plus (2, 2) (Y read first, each time), with the level and the skill asked again for each; neither answer is kept. Then
//  MISSILE_CltDoFunc1.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall MISSILE_CltDoFunc62(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2UnitStrc *pOwner;
    D2UnitStrc *pBolt;
    D2SeedStrc *pSeed;
    int32_t nLived;
    int32_t nLevel;
    int32_t nSkillId;
    int32_t nX;
    int32_t nY;

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    nLived = D2COMMON_MISSILE_GetRemainingFrames(pMissile);
    if (!pOwner)
    {
        MISSILE_CltDoFunc1(pMissile);
        return;
    }

    MISSILE_SpawnNihlathakDebris(pOwner, pMissile);
    MISSILE_SpawnNihlathakDebris(pOwner, pMissile);

    if (nLived > 60 && nLived % 20 == 0)
    {
        memset(&tParams, 0, sizeof(tParams));
        pSeed = &pMissile->pSeed;
        tParams.dwFlags = MISSILESPAWN_TARGETOFFSET;
        tParams.pOwner = pOwner;
        tParams.pOrigin = pMissile;
        tParams.nTargetX = 20 - (int32_t)((uint32_t)SEED_RollRandomNumber(pSeed) % 40);
        tParams.nTargetY = 20 - SEED_RollLimitedRandomNumber(pSeed, 40);
        tParams.nMissile = MISSILE_BRDEATHLIGHTNINGBOLT;
        tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
        tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        if (!tParams.nTargetX && !tParams.nTargetY)
            tParams.nTargetX = SEED_RollLimitedRandomNumber(pSeed, 19) + 1;
        pBolt = MISSILE_ClientSpawn(&tParams);
        if (pBolt)
        {
            D2COMMON_MISSILE_SetTargetX(pBolt, tParams.nTargetX);
            D2COMMON_MISSILE_SetTargetY(pBolt, tParams.nTargetY);
            PARTICLE_SetUnitGfxCoords(pBolt, 0, 0, -10);
        }
    }

    if (nLived == 25)
    {
        nY = UNITS_GetYPosition(pMissile);

        nX = UNITS_GetXPosition(pMissile);

        nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
        memset(&tParams, 0, sizeof(tParams));
        tParams.pOwner = pOwner;
        tParams.dwFlags = MISSILESPAWN_STARTPOS;
        tParams.nX = nX + 2;
        tParams.nY = nY + 2;
        tParams.nSkill = nSkillId;
        tParams.nSkillLevel = nLevel;
        tParams.nMissile = MISSILE_NEHLITHAKHOLELIGHT;
        MISSILE_ClientSpawn(&tParams);

        nY = UNITS_GetYPosition(pMissile);

        nX = UNITS_GetXPosition(pMissile);

        nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
        memset(&tParams, 0, sizeof(tParams));
        tParams.pOwner = pOwner;
        tParams.dwFlags = MISSILESPAWN_STARTPOS;
        tParams.nX = nX + 2;
        tParams.nY = nY + 2;
        tParams.nSkill = nSkillId;
        tParams.nSkillLevel = nLevel;
        tParams.nMissile = MISSILE_NEHLITHAKHOLE;
        MISSILE_ClientSpawn(&tParams);
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB3BDA0
//  One piece of Nihlathak's debris -- pCltDoFunc 62 calls it twice a frame with the control's owner and the control. Two four-entry tables on the
//  stack, X {1, 0, -1, 0} and Y {0, 1, 0, -1}: the four neighbours of a point. A seed roll (inlined), its remainder below 125 the piece's height; a
//  second, a CALL of SEED_RollRandomNumber, whose low two bits pick the neighbour; the control's position (X read first) plus that neighbour is the
//  spawn point. A third roll (inlined) below 30 picks the piece -- 0..3 nehlithakdebris1..4, anything else nehlithakswoosh, a jump-table switch. One
//  block, flags 3, for pOwner, with the control's skill and level, and the neighbour turned a quarter, (dY, -dX), as the aim -- an offset from the
//  spawn point, bit 1 of the flags (MISSILE_ClientSpawn). The piece spawned: gfx coords (0, 0, the height), and the control's position as its TargetX
//  and TargetY (X read first).
//  The original derives the aim's two offsets again as the spawn point less the position: the table's values, identically in 32 bits. The C reads
//  them from the table.
//  The original inlines SEED_RollRandomNumber, UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall MISSILE_SpawnNihlathakDebris(D2UnitStrc *pOwner, D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2UnitStrc *pDebris;
    D2SeedStrc *pSeed;
    int32_t anOffsetX[4];
    int32_t anOffsetY[4];
    uint32_t nHeight;
    int32_t nDir;
    int32_t nX;
    int32_t nY;
    int32_t nSpawnX;
    int32_t nSpawnY;
    int32_t nMissileId;

    pSeed = &pMissile->pSeed;
    anOffsetX[0] = 1;
    anOffsetX[2] = -1;
    anOffsetY[1] = 1;
    anOffsetY[3] = -1;

    nHeight = (uint32_t)SEED_RollRandomNumber(pSeed) % 125;
    anOffsetX[1] = 0;
    anOffsetX[3] = 0;
    anOffsetY[0] = 0;
    anOffsetY[2] = 0;

    nDir = (int32_t)((uint32_t)SEED_RollRandomNumber(pSeed) & 3);

    nX = UNITS_GetXPosition(pMissile);

    nY = UNITS_GetYPosition(pMissile);

    nSpawnY = nY + anOffsetY[nDir];
    nSpawnX = nX + anOffsetX[nDir];

    switch ((uint32_t)SEED_RollRandomNumber(pSeed) % 30)
    {
    case 0:
        nMissileId = MISSILE_NEHLITHAKDEBRIS1;
        break;
    case 1:
        nMissileId = MISSILE_NEHLITHAKDEBRIS2;
        break;
    case 2:
        nMissileId = MISSILE_NEHLITHAKDEBRIS3;
        break;
    case 3:
        nMissileId = MISSILE_NEHLITHAKDEBRIS4;
        break;
    default:
        nMissileId = MISSILE_NEHLITHAKSWOOSH;
        break;
    }

    memset(&tParams, 0, sizeof(tParams));
    tParams.nMissile = nMissileId;
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETOFFSET;
    tParams.pOwner = pOwner;
    tParams.nX = nSpawnX;
    tParams.nY = nSpawnY;
    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
    tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    tParams.nTargetX = anOffsetY[nDir];
    tParams.nTargetY = -anOffsetX[nDir];
    pDebris = MISSILE_ClientSpawn(&tParams);
    if (!pDebris)
        return;

    PARTICLE_SetUnitGfxCoords(pDebris, 0, 0, (int32_t)nHeight);

    D2COMMON_MISSILE_SetTargetX(pDebris, UNITS_GetXPosition(pMissile));

    D2COMMON_MISSILE_SetTargetY(pDebris, UNITS_GetYPosition(pMissile));
}


//  The missile and monster ids carried as literals (0x272, 0x277..0x27A, 0x27B and 0x209 in the bytes); the two other debris rows are 0x27B plus a
//  roll below 3, so their order is what is pinned.
static_assert(MISSILE_BAALFX_SPIRIT_1 == 626 && MISSILE_BAALFX_BAAL_HEAD_APPEAR == 631
                  && MISSILE_BAALFX_BAAL_HEAD_1 == 632 && MISSILE_BAALFX_BAAL_HEAD_2 == 633
                  && MISSILE_BAALFX_BAAL_HEAD_3 == 634 && MISSILE_BAALFX_TYREAL_DEBRIS_1 == 635
                  && MISSILE_BAALFX_TYREAL_DEBRIS_2 == MISSILE_BAALFX_TYREAL_DEBRIS_1 + 1
                  && MISSILE_BAALFX_TYREAL_DEBRIS_3 == MISSILE_BAALFX_TYREAL_DEBRIS_1 + 2
                  && MONSTER_TYRAEL3 == 521,
              "batch 21's missile and monster ids");


//1.10f: D2Client.0x6FB3BFE0
//  pCltDoFunc 63: the swoosh and the four debris (607..611, Range 22 and 16) MISSILE_SpawnNihlathakDebris spawns -- the swirl. The missile's position
//  (X read first); its TargetX and TargetY are the CONTROL's position as the spawn found it, so (dX, dY), the position less that point, is the offset
//  from it; the path is aimed at the position plus the offset turned a quarter, (X + dY, Y - dX) -- the turn the spawn gave the first aim -- and
//  started again, every frame, so the piece turns about that point. The path is read once, with the X, and kept for both calls. Then a tail jump to
//  MISSILE_CltDoFunc1.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall MISSILE_CltDoFunc63(D2UnitStrc *pMissile)
{
    D2DynamicPathStrc *pPath;
    int32_t nX;
    int32_t nY;
    int32_t nDeltaX;
    int32_t nDeltaY;

    pPath = pMissile->pDynamicPath;

    nX = UNITS_GetXPosition(pMissile);

    nY = UNITS_GetYPosition(pMissile);

    nDeltaX = nX - D2COMMON_MISSILE_GetTargetX(pMissile);
    nDeltaY = nY - D2COMMON_MISSILE_GetTargetY(pMissile);
    D2COMMON_10170_PathSetTargetPos(pPath, nX + nDeltaY, nY - nDeltaX);
    D2COMMON_PATH_StartUnitMove(pPath, pMissile, 0);
    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB3C080
//  pCltDoFunc 64: baalfx control alone (Range 650), which QUESTSFX_OnBaalDeath spawns at Baal (class 544, baalcrab), owned by the player. The owner
//  first; with none, only MISSILE_CltDoFunc1. The frame counter (running DOWN) and the stages: gBaalFx, five records {frame bound, period, spirit
//  flag}: {420, 64, 0} {435, 20, 1} {470, 8, 1}
//  {270, 5, 1} and {0, 64, 0}, walked as pCltDoFunc 32, 33 and 36 walk theirs.
//  On a frame whose stage has the spirit flag and whose period divides it: a seed roll CALLED and its answer discarded; one block, flags 2, for the
//  owner, the control as origin, aimed at an offset (MISSILE_ClientSpawn's bit 1) of two more called rolls below 40, less 20; baalfx spirit 1, with
//  the skill and the level; the spirit spawned takes the control's unit id as TargetX, by way of a NULL test on the control made after its seed was
//  used. Then, every frame, the control's own TargetX: 1 from frame 210 up, 2 below -- the phase pCltDoFunc 65 reads. Then a switch on the frame,
//  through a byte index table and five jump slots: on 450, baalfx baal head appear at the position plus (1, 1); on 425 and 375, baal head 1 and 2 at
//  the position (the three each flags 1, for the owner, with the level and the skill; Y read first); on 325, baal head 3 at the position through
//  MISSILE_ClientSpawnSimple for the owner (the Y inlined, the X a CALL). Then, every frame: with a TargetY -- QUESTSFX_OnTyrael3Created sets 1 when
//  Tyrael (monster 521, tyrael3) appears near the control -- Tyrael from the control's neighbouring rooms, and one falling piece of debris
//  (MISSILE_SpawnFallingSubMissile) at his position moved by two limited rolls below 10, less 5. Then MISSILE_CltDoFunc1.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall MISSILE_CltDoFunc64(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2UnitStrc *pOwner;
    D2UnitStrc *pSpirit;
    D2UnitStrc *pTyrael;
    D2SeedStrc *pSeed;
    int32_t nFrame;
    int32_t nStage;
    int32_t nLevel;
    int32_t nSkillId;
    int32_t nOffsetX;
    int32_t nOffsetY;
    int32_t nX;
    int32_t nY;

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
    {
        MISSILE_CltDoFunc1(pMissile);
        return;
    }

    nFrame = D2COMMON_MISSILE_GetCurrentFrame(pMissile);
    for (nStage = 0; nStage < 4; ++nStage)
    {
        if (nFrame > gBaalFx[nStage].nFrameBound)
            break;
    }

    //  BUG: the stage bounds do not descend, so the walk stops at record 0 for any frame above 420 and records 1 and 2 are never
    //  chosen; only record 3 sets the spirit flag (frames 420 down to 271 that 5 divides). REACHABLE: the walk runs every frame.
    if (nFrame % gBaalFx[nStage].nPeriod == 0 && gBaalFx[nStage].nParam)
    {
        pSeed = &pMissile->pSeed;
        SEED_RollRandomNumber(pSeed);
        memset(&tParams, 0, sizeof(tParams));
        tParams.dwFlags = MISSILESPAWN_TARGETOFFSET;
        tParams.pOwner = pOwner;
        tParams.pOrigin = pMissile;
        tParams.nTargetX = (int32_t)((uint32_t)SEED_RollRandomNumber(pSeed) % 40) - 20;
        tParams.nTargetY = (int32_t)((uint32_t)SEED_RollRandomNumber(pSeed) % 40) - 20;
        tParams.nMissile = MISSILE_BAALFX_SPIRIT_1;
        tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
        tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        pSpirit = MISSILE_ClientSpawn(&tParams);
        if (pSpirit)
        {
            //  BUG: a NULL test of pMissile after its seed was used. UNREACHABLE: the use above would have faulted first.
            D2COMMON_MISSILE_SetTargetX(pSpirit, pMissile ? (int32_t)pMissile->dwUnitId : -1);
        }
    }

    if (nFrame >= 210)
        D2COMMON_MISSILE_SetTargetX(pMissile, 1);
    else
        D2COMMON_MISSILE_SetTargetX(pMissile, 2);

    switch (nFrame)
    {
    case 325:
        nY = UNITS_GetYPosition(pMissile);
        nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
        nX = UNITS_GetXPosition(pMissile);
        MISSILE_ClientSpawnSimple(pOwner, MISSILE_BAALFX_BAAL_HEAD_3, nX, nY, nSkillId, nLevel);
        break;

    case 375:
        nY = UNITS_GetYPosition(pMissile);
        nX = UNITS_GetXPosition(pMissile);
        nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
        memset(&tParams, 0, sizeof(tParams));
        tParams.dwFlags = MISSILESPAWN_STARTPOS;
        tParams.pOwner = pOwner;
        tParams.nX = nX;
        tParams.nY = nY;
        tParams.nSkill = nSkillId;
        tParams.nSkillLevel = nLevel;
        tParams.nMissile = MISSILE_BAALFX_BAAL_HEAD_2;
        MISSILE_ClientSpawn(&tParams);
        break;

    case 425:
        nY = UNITS_GetYPosition(pMissile);
        nX = UNITS_GetXPosition(pMissile);
        nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
        memset(&tParams, 0, sizeof(tParams));
        tParams.dwFlags = MISSILESPAWN_STARTPOS;
        tParams.pOwner = pOwner;
        tParams.nX = nX;
        tParams.nY = nY;
        tParams.nSkill = nSkillId;
        tParams.nSkillLevel = nLevel;
        tParams.nMissile = MISSILE_BAALFX_BAAL_HEAD_1;
        MISSILE_ClientSpawn(&tParams);
        break;

    case 450:
        nY = UNITS_GetYPosition(pMissile);
        nX = UNITS_GetXPosition(pMissile);
        nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
        memset(&tParams, 0, sizeof(tParams));
        tParams.dwFlags = MISSILESPAWN_STARTPOS;
        tParams.pOwner = pOwner;
        tParams.nX = nX + 1;
        tParams.nY = nY + 1;
        tParams.nSkill = nSkillId;
        tParams.nSkillLevel = nLevel;
        tParams.nMissile = MISSILE_BAALFX_BAAL_HEAD_APPEAR;
        MISSILE_ClientSpawn(&tParams);
        break;
    }

    if (D2COMMON_MISSILE_GetTargetY(pMissile))
    {
        pTyrael = D2COMMON_UNITFINDS_FindUnitInNeighboredRooms(D2COMMON_UNITS_GetRoom(pMissile), UNIT_MONSTER, MONSTER_TYRAEL3);
        if (pTyrael)
        {
            nOffsetX = SEED_RollLimitedRandomNumber(&pMissile->pSeed, 10) - 5;
            nOffsetY = SEED_RollLimitedRandomNumber(&pMissile->pSeed, 10) - 5;
            nX = nOffsetX + UNITS_GetXPosition(pTyrael);
            nY = UNITS_GetYPosition(pTyrael) + nOffsetY;
            MISSILE_SpawnFallingSubMissile(pMissile, nX, nY);
        }
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB3C540
//  One falling piece of Tyrael's debris, for pCltDoFunc 64 and 66; nY is the one stack argument. Nothing when the point is blocked for a unit of size
//  3 (COLLISION_CheckMaskWithSize with COLLIDE_WALL, its answer tested as the word D2Common returns -- `test ax, ax`). Otherwise one block, flags
//  0x2001, for the missile's owner (asked, not tested), baalfx tyreal debris 1, 2 or 3 by a seed roll below 3, with the skill and the level, at (nX,
//  nY); the piece spawned gets gfx coords (0, 0, 250), gfx vector (0, 0, -10, scaled), 25 frames total and current, and animation speed 128.
//  The original inlines SEED_RollRandomNumber here.
void __fastcall MISSILE_SpawnFallingSubMissile(D2UnitStrc *pMissile, int32_t nX, int32_t nY)
{
    D2MissileStrc tParams;
    D2UnitStrc *pPiece;

    if (D2COMMON_COLLISION_CheckMaskWithSize(D2COMMON_UNITS_GetRoom(pMissile), nX, nY, 3, COLLIDE_WALL))
        return;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_RANDOMDIR;
    tParams.pOwner = CUNIT_GetOwnerUnit(pMissile);

    tParams.nMissile = (int32_t)((uint32_t)SEED_RollRandomNumber(&pMissile->pSeed) % 3) + MISSILE_BAALFX_TYREAL_DEBRIS_1;
    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
    tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    tParams.nX = nX;
    tParams.nY = nY;
    pPiece = MISSILE_ClientSpawn(&tParams);
    if (pPiece)
    {
        PARTICLE_SetUnitGfxCoords(pPiece, 0, 0, 250);
        PARTICLE_SetUnitGfxVector(pPiece, 0, 0, -10, 1);
        D2COMMON_MISSILE_SetTotalFrames(pPiece, 25);
        D2COMMON_MISSILE_SetCurrentFrame(pPiece, 25);
        D2COMMON_MISSILE_SetAnimSpeed(pPiece, 128);
    }
}


//1.10f: D2Client.0x6FB3C630
//  pCltDoFunc 65: baalfx spirit 1..5 (626..630, Range 250); pCltDoFunc 64 spawns the first of them with the control's unit id in TargetX. The
//  spirit's own STATE, its TargetY, first (0 at spawn, as pCltDoFunc 34's ball); no dynamic path, or no control by that id (the client-id table, as a
//  missile), and only MISSILE_CltDoFunc1. Then the control's PHASE, its TargetX -- 1, then 2 below frame 210 (pCltDoFunc 64).
//  A new phase of 2 or more (`cmp ebx, 2 / jl`), not the state: a seed roll, and on one in 25 the phase is stored as the state; at phase 2 the spirit
//  is sent at velocity 3840 to a fixed point, (15135, 5900), and started. Otherwise -- no new phase, or the roll missed -- by the state: at 0, a
//  roll, and on one in 10 the state becomes 1, the particle flag, velocity 3840, and on a CALLED roll's odd low bit a gfx vector (0, 0, 1, scaled);
//  at 1, it is steered at its own position plus its offset from the control turned a quarter, (X - dY, Y + dX), with (dX, dY) negated when bit 1 of
//  the spirit's unit id is set (`test al, 2`) -- spirits turn both ways -- unless it sits on the control, its X read by a CALL where the other reads
//  are inlined; in any other state, nothing. Every path ends in a tail jump to MISSILE_CltDoFunc1.
//  The original inlines SEED_RollRandomNumber, UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall MISSILE_CltDoFunc65(D2UnitStrc *pMissile)
{
    D2DynamicPathStrc *pPath;
    D2UnitStrc *pControl;
    D2SeedStrc *pSeed;
    int32_t nState;
    int32_t nPhase;
    int32_t nDeltaX;
    int32_t nDeltaY;
    int32_t nX;
    int32_t nY;

    pSeed = &pMissile->pSeed;
    nState = D2COMMON_MISSILE_GetTargetY(pMissile);
    pPath = pMissile->pDynamicPath;
    if (!pPath)
    {
        MISSILE_CltDoFunc1(pMissile);
        return;
    }

    pControl = CUNIT_FindUnitByClientIdAndType(D2COMMON_MISSILE_GetTargetX(pMissile), UNIT_MISSILE);
    if (!pControl)
    {
        MISSILE_CltDoFunc1(pMissile);
        return;
    }

    nPhase = D2COMMON_MISSILE_GetTargetX(pControl);

    if (nState != nPhase && nPhase >= 2 && (uint32_t)SEED_RollRandomNumber(pSeed) % 25 == 0)
    {
        D2COMMON_MISSILE_SetTargetY(pMissile, nPhase);
        if (nPhase == 2)
        {
            D2COMMON_PATH_SetVelocity(pPath, 3840, szMissileCpp, 6619);
            D2COMMON_10170_PathSetTargetPos(pPath, 15135, 5900);
            D2COMMON_PATH_StartUnitMove(pPath, pMissile, 0);
        }
        MISSILE_CltDoFunc1(pMissile);
        return;
    }

    if (nState == 0)
    {
        if ((uint32_t)SEED_RollRandomNumber(pSeed) % 10 == 0)
        {
            D2COMMON_MISSILE_SetTargetY(pMissile, 1);
            PARTICLE_SetUnitParticleFlag(pMissile);
            D2COMMON_PATH_SetVelocity(pPath, 3840, szMissileCpp, 6630);
            if ((uint32_t)SEED_RollRandomNumber(pSeed) & 1)
                PARTICLE_SetUnitGfxVector(pMissile, 0, 0, 1, 1);
        }
    }
    else if (nState == 1)
    {
        nX = UNITS_GetXPosition(pMissile);
        nDeltaX = nX - UNITS_GetXPosition(pControl);

        nY = UNITS_GetYPosition(pMissile);
        nDeltaY = nY - UNITS_GetYPosition(pControl);

        if ((uint8_t)pMissile->dwUnitId & 2)
        {
            nDeltaX = -nDeltaX;
            nDeltaY = -nDeltaY;
        }

        if (nDeltaX || nDeltaY)
        {
            nY = UNITS_GetYPosition(pMissile) + nDeltaX;
            nX = UNITS_GetXPosition(pMissile) - nDeltaY;
            D2COMMON_10170_PathSetTargetPos(pPath, nX, nY);
            D2COMMON_PATH_StartUnitMove(pPath, pMissile, 0);
        }
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB3C8A0
//  pCltDoFunc 66: worldstone shake alone (Range 200), which QUESTSFX_SpawnWorldstoneShake (and its copy inlined in QUESTSFX_OnTyrael3Created) spawns
//  at the player. It lives on its owner: without one, or when the owner's room is outside levels 131..132 (Act 5 - Throne Room and Act 5 - World
//  Stone; `cmp eax, 83h / jl`, `cmp eax, 84h / jg`), the missile is destroyed (MISSILE_DestroyClientMissile, a tail jump). Otherwise it rides the
//  owner (D2Common #10235, PATH_UpdateRiderPath, the rider first) and steps its animation (UNITS_UpdateFrame). A screen shake -- magnitude 6, 2000
//  up, 3000 held, 2000 down -- and TargetX 1, on the first frame (TargetX still 0) and after it on a frame whose roll has its low six bits clear, one
//  in 64. Then a roll below 10, and on 0 one falling piece of Tyrael's debris (MISSILE_SpawnFallingSubMissile) at the position moved by two rolls
//  below 60, less 30 (X read first; the Y's three arms each end in their own call and return).
//  It never calls MISSILE_CltDoFunc1: every exit is a `retn` or the destroy's tail jump, so nothing here steps the missile's frame counter; losing
//  the owner, or the owner leaving the two levels, is what ends it.
//  The original inlines SEED_RollRandomNumber, UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall MISSILE_CltDoFunc66(D2UnitStrc *pMissile)
{
    D2UnitStrc *pOwner;
    D2SeedStrc *pSeed;
    uint64_t lSeed;
    int32_t bThroneOrWorldstone;
    int32_t nLevelId;
    int32_t nRollX;
    int32_t nRollY;
    int32_t nX;

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    bThroneOrWorldstone = FALSE;
    if (pOwner)
    {
        nLevelId = D2COMMON_DUNGEON_GetLevelIdFromRoom(D2COMMON_UNITS_GetRoom(pOwner));
        if (nLevelId >= 131 && nLevelId <= 132)
            bThroneOrWorldstone = TRUE;
    }
    if (!bThroneOrWorldstone)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    D2COMMON_10235_PATH_UpdateRiderPath(pMissile, pOwner);
    D2COMMON_UNITS_UpdateFrame(pMissile);

    pSeed = &pMissile->pSeed;

    if (!D2COMMON_MISSILE_GetTargetX(pMissile) || !((uint32_t)SEED_RollRandomNumber(pSeed) & 0x3F))
    {
        VIEWEFFECTS_StartScreenShake(6, 2000, 3000, 2000);
        D2COMMON_MISSILE_SetTargetX(pMissile, 1);
    }

    if ((uint32_t)SEED_RollRandomNumber(pSeed) % 10)
        return;

    lSeed = (uint64_t)pMissile->pSeed.nLowSeed * 0x6AC690C5u + pMissile->pSeed.nHighSeed;
    pMissile->pSeed.nLowSeed = (uint32_t)lSeed;
    nRollX = (int32_t)((uint32_t)lSeed % 60);
    lSeed = (uint64_t)(uint32_t)lSeed * 0x6AC690C5u + (uint32_t)(lSeed >> 32);
    pMissile->pSeed.nHighSeed = (uint32_t)(lSeed >> 32);
    pMissile->pSeed.nLowSeed = (uint32_t)lSeed;
    nRollY = (int32_t)((uint32_t)lSeed % 60);

    nX = UNITS_GetXPosition(pMissile) + nRollX - 30;

    MISSILE_SpawnFallingSubMissile(pMissile, nX, UNITS_GetYPosition(pMissile) + nRollY - 30);
}


//  The missile ids carried as literals (0x58, 0x59, 0x1A1, 0x214, 0x255, 0x267, 0x268 and 0x21E in the bytes).
static_assert(MISSILE_FREEZINGARROWEXP1 == 88 && MISSILE_FREEZINGARROWEXP2 == 89
                  && MISSILE_CATAPULT_COLD_EXPLOSION == 417 && MISSILE_ANYA_ICEMAGIC == 532
                  && MISSILE_ANYASTEAM1 == 597 && MISSILE_ANYASTEAMVENT == 615 && MISSILE_ANYASTEAM == 616
                  && MISSILE_ANCIENT_DEATH_CLOUD == 542,
              "batch 22's missile ids");


//1.10f: D2Client.0x6FB3CA50
//  pCltDoFunc 59: world stone chip 1..4 (570..573, Range 500), which MONSTER_EnvTickWorldstoneChips spawns with TargetX 100 and TargetY 300. A chip
//  whose gfx height (PARTICLE_GetUnitGfxCoordZ) is below -TargetX or above TargetY -- SIGNED, `jl` and `jg` -- is destroyed
//  (MISSILE_DestroyClientMissile); any other goes on to MISSILE_CltDoFunc1, each by a tail jump. TargetX holds the lower bound NEGATED: those chips
//  live between heights -100 and 300.
void __fastcall MISSILE_CltDoFunc59(D2UnitStrc *pMissile)
{
    int32_t nLow;
    int32_t nHigh;
    int32_t nZ;

    nLow = -D2COMMON_MISSILE_GetTargetX(pMissile);
    nHigh = D2COMMON_MISSILE_GetTargetY(pMissile);
    nZ = PARTICLE_GetUnitGfxCoordZ(pMissile);
    if (nZ < nLow || nZ > nHigh)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB3CA90
//  pCltHitFunc 48: catapult cold ball alone (416, Range 25) -- the Overseer's cold shot landing (pCltDoFunc 61's). The unit hit is never read. Two
//  eight-entry tables on the stack, X {0, 1, 1, 1, 0, -1, -1, -1} and Y {1, 1, 0, -1, -1, -1, 0, 1}: the eight neighbours of a point, going round.
//  The ball's position (X read first); the level and the skill; for the ball itself were it a player or a monster, else its owner (none, no spawn):
//  one freezingarrowexp1, flags 0x20, the ball as origin, TargetX and TargetY written 0 over the clear. Then, for each neighbour: the level, the
//  skill and the owner asked again, and one freezingarrowexp2 the same way, turned to face the neighbour (PATH_FaceCoords); the level, the skill and
//  the owner once more, and one catapult cold explosion AIMED at the neighbour, its total frames set to the total less a roll below half of it, its
//  velocity to a roll below itself, and an arc (PARTICLE_StartUnitArc, peak 30) over that many frames. Answers 1.
//  Both rolls are D2Common's SEED_RollLimitedRandomNumber inlined as its header has it: a bound of 0 or less rolls nothing, a power of two masks a
//  CALLED SEED_RollRandomNumber, anything else takes an inlined step's remainder (SEED_RollLimitedRandomNumberInline). The half is a SIGNED division
//  (`cdq / sub eax, edx / sar ebx, 1`).
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall MISSILE_CltHitFunc48(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissileStrc tParams;
    D2MissileStrc tExplosion;
    D2UnitStrc *pOwner;
    D2UnitStrc *pSpawned;
    D2DynamicPathStrc *pPath;
    int32_t anOffsetX[8];
    int32_t anOffsetY[8];
    int32_t nX;
    int32_t nY;
    int32_t nPointX;
    int32_t nPointY;
    int32_t nLevel;
    int32_t nSkillId;
    int32_t nDir;
    int32_t nTotal;
    int32_t nFrames;

    UNREFERENCED_PARAMETER(pTarget);

    anOffsetX[5] = -1;
    anOffsetX[6] = -1;
    anOffsetX[7] = -1;
    anOffsetY[3] = -1;
    anOffsetY[4] = -1;
    anOffsetY[5] = -1;
    anOffsetX[0] = 0;
    anOffsetX[1] = 1;
    anOffsetX[2] = 1;
    anOffsetX[3] = 1;
    anOffsetX[4] = 0;
    anOffsetY[0] = 1;
    anOffsetY[1] = 1;
    anOffsetY[2] = 0;
    anOffsetY[6] = 0;
    anOffsetY[7] = 1;

    nX = UNITS_GetXPosition(pMissile);

    nY = UNITS_GetYPosition(pMissile);

    nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
    if (pMissile->dwUnitType == UNIT_PLAYER || pMissile->dwUnitType == UNIT_MONSTER)
        pOwner = pMissile;
    else
        pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (pOwner)
    {
        memset(&tParams, 0, sizeof(tParams));
        tParams.dwFlags = MISSILESPAWN_TARGETPOS;
        tParams.nSkill = nSkillId;
        tParams.pOwner = pOwner;
        tParams.pOrigin = pMissile;
        tParams.nTargetX = 0;
        tParams.nTargetY = 0;
        tParams.nSkillLevel = nLevel;
        tParams.nMissile = MISSILE_FREEZINGARROWEXP1;
        MISSILE_ClientSpawn(&tParams);
    }

    for (nDir = 0; nDir < 8; ++nDir)
    {
        nPointX = nX + anOffsetX[nDir];
        nPointY = nY + anOffsetY[nDir];

        nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
        if (pMissile->dwUnitType == UNIT_PLAYER || pMissile->dwUnitType == UNIT_MONSTER)
            pOwner = pMissile;
        else
            pOwner = CUNIT_GetOwnerUnit(pMissile);
        if (pOwner)
        {
            memset(&tParams, 0, sizeof(tParams));
            tParams.pOwner = pOwner;
            tParams.nTargetX = 0;
            tParams.nTargetY = 0;
            tParams.dwFlags = MISSILESPAWN_TARGETPOS;
            tParams.pOrigin = pMissile;
            tParams.nSkill = nSkillId;
            tParams.nSkillLevel = nLevel;
            tParams.nMissile = MISSILE_FREEZINGARROWEXP2;
            pSpawned = MISSILE_ClientSpawn(&tParams);
            if (pSpawned)
                D2COMMON_PATH_FaceCoords(pSpawned->pDynamicPath, nPointX, nPointY, 0);
        }

        nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
        if (pMissile->dwUnitType == UNIT_PLAYER || pMissile->dwUnitType == UNIT_MONSTER)
        {
            pOwner = pMissile;
        }
        else
        {
            pOwner = CUNIT_GetOwnerUnit(pMissile);
            if (!pOwner)
                continue;
        }

        memset(&tExplosion, 0, sizeof(tExplosion));
        tExplosion.pOwner = pOwner;
        tExplosion.dwFlags = MISSILESPAWN_TARGETPOS;
        tExplosion.pOrigin = pMissile;
        tExplosion.nTargetX = nPointX;
        tExplosion.nTargetY = nPointY;
        tExplosion.nSkill = nSkillId;
        tExplosion.nSkillLevel = nLevel;
        tExplosion.nMissile = MISSILE_CATAPULT_COLD_EXPLOSION;
        pSpawned = MISSILE_ClientSpawn(&tExplosion);
        if (!pSpawned)
            continue;

        nTotal = D2COMMON_MISSILE_GetTotalFrames(pSpawned);

        nFrames = nTotal - (int32_t)SEED_RollLimitedRandomNumberInline(&pMissile->pSeed, nTotal / 2);
        D2COMMON_MISSILE_SetTotalFrames(pSpawned, nFrames);

        pPath = pSpawned->pDynamicPath;
        D2COMMON_PATH_SetVelocity(pPath, (int32_t)SEED_RollLimitedRandomNumberInline(&pMissile->pSeed, D2COMMON_PATH_GetVelocity(pPath)), szMissileCpp, 6763);
        PARTICLE_StartUnitArc(pSpawned, 30, nFrames);
    }

    return 1;
}


//1.10f: D2Client.0x6FB3CE10
//  pCltDoFunc 54: anya center alone (528, Range 300, no server function), which QUESTSFX_TriggerAnya spawns for the player at object 460 dummy. The
//  owner, the frames the missile has LIVED (MISSILE_GetRemainingFrames) and its position (X read first), every frame. Below 110 frames lived, puffs
//  of anyasteam1: eight on the first frame -- after one anya icemagic at the position, for the owner -- and below 50; five below 80; one below 110
//  (`setl` into `lea [eax*4+1]`) -- the steam THINS as the missile lives. Each puff, with an owner: flags 1, at the position plus (2, 2), with the
//  level and the skill asked again; spawned, it takes gfx coords of 50 less a roll below 100 on each axis and a rising gfx vector of 2 plus a CALLED
//  roll's low three bits. Then: on frame 5 a screen shake (magnitude 3, 600 up, 4000 held, 3000 down); below 200 frames, on every seventh (a SIGNED
//  remainder) a steam vent near the position (MISSILE_SpawnSteamVentNearby), the shake's frame included -- 5 is not a multiple of 7, so that test
//  fails there. Then MISSILE_CltDoFunc1.
//  The original inlines UNITS_GetXPosition, UNITS_GetYPosition and SEED_RollRandomNumber here.
void __fastcall MISSILE_CltDoFunc54(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    D2UnitStrc *pOwner;
    D2UnitStrc *pPuff;
    int32_t nLived;
    int32_t nX;
    int32_t nPosY;
    int32_t nCount;
    int32_t nLevel;
    int32_t nSkillId;
    int32_t nGfxX;
    int32_t nGfxY;
    int32_t nRise;

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    nLived = D2COMMON_MISSILE_GetRemainingFrames(pMissile);

    nX = UNITS_GetXPosition(pMissile);

    nPosY = UNITS_GetYPosition(pMissile);

    if (nLived < 110)
    {
        if (nLived == 0)
        {
            nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
            nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
            if (pOwner)
            {
                memset(&tParams, 0, sizeof(tParams));
                tParams.nX = nX;
                tParams.dwFlags = MISSILESPAWN_STARTPOS;
                tParams.pOwner = pOwner;
                tParams.nY = nPosY;
                tParams.nSkill = nSkillId;
                tParams.nSkillLevel = nLevel;
                tParams.nMissile = MISSILE_ANYA_ICEMAGIC;
                MISSILE_ClientSpawn(&tParams);
            }
            nCount = 8;
        }
        else if (nLived < 50)
        {
            nCount = 8;
        }
        else
        {
            nCount = 4 * (nLived < 80) + 1;
        }

        if (nCount > 0)
        {
            do
            {
                nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
                nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
                if (pOwner)
                {
                    memset(&tParams, 0, sizeof(tParams));
                    tParams.nX = nX + 2;
                    tParams.dwFlags = MISSILESPAWN_STARTPOS;
                    tParams.pOwner = pOwner;
                    tParams.nY = nPosY + 2;
                    tParams.nSkill = nSkillId;
                    tParams.nSkillLevel = nLevel;
                    tParams.nMissile = MISSILE_ANYASTEAM1;
                    pPuff = MISSILE_ClientSpawn(&tParams);
                    if (pPuff)
                    {
                        nGfxX = 50 - (int32_t)((uint32_t)SEED_RollRandomNumber(&pMissile->pSeed) % 100);
                        nGfxY = 50 - (int32_t)((uint32_t)SEED_RollRandomNumber(&pMissile->pSeed) % 100);
                        nRise = (int32_t)((uint32_t)SEED_RollRandomNumber(&pMissile->pSeed) & 7);
                        PARTICLE_SetUnitGfxCoords(pPuff, nGfxX, nGfxY, 0);
                        PARTICLE_SetUnitGfxVector(pPuff, 0, 0, nRise + 2, 1);
                    }
                }
            }
            while (--nCount);
        }
    }

    if (nLived == 5)
    {
        VIEWEFFECTS_StartScreenShake(3, 600, 4000, 3000);
    }
    else if (nLived >= 200)
    {
        MISSILE_CltDoFunc1(pMissile);
        return;
    }

    if (nLived % 7 == 0)
        MISSILE_SpawnSteamVentNearby(pOwner, pMissile, nX, nPosY);

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB3D090
//  A steam vent near a point, for pCltDoFunc 54; nX and nY are the two stack arguments. Nothing without the missile, the owner or the missile's room.
//  Then up to four tries: two seed rolls, each 17 less a roll below 34 -- -16..17 -- as an offset; an offset with X in -1..1 and Y -1 or more is
//  passed over (`jl`, `jg`, `jge`, SIGNED), and any other is taken when the spot is clear of COLLIDE_MISSILE_BARRIER (COLLISION_CheckMask, its word
//  answer tested, `test ax, ax`). On the first clear spot: one anyasteamvent and one anyasteam from the same block, flags 1, for pOwner, with the
//  missile's skill and level, at the spot. Four failures spawn nothing.
//  The original inlines SEED_RollRandomNumber here.
void __fastcall MISSILE_SpawnSteamVentNearby(D2UnitStrc *pOwner, D2UnitStrc *pMissile, int32_t nX, int32_t nY)
{
    D2MissileStrc tParams;
    D2ActiveRoomStrc *pRoom;
    int32_t bNoSpot;
    int32_t nTries;
    int32_t nOffsetX;
    int32_t nOffsetY;
    int32_t nSpotX;
    int32_t nSpotY;

    bNoSpot = TRUE;
    nTries = 0;
    if (!pMissile || !pOwner)
        return;

    pRoom = D2COMMON_UNITS_GetRoom(pMissile);
    if (!pRoom)
        return;

    while (nTries < 4)
    {
        nOffsetX = 17 - (int32_t)((uint32_t)SEED_RollRandomNumber(&pMissile->pSeed) % 34);
        nOffsetY = 17 - (int32_t)((uint32_t)SEED_RollRandomNumber(&pMissile->pSeed) % 34);
        if (nOffsetX < -1 || nOffsetX > 1 || nOffsetY < -1)
        {
            nSpotX = nOffsetX + nX;
            nSpotY = nOffsetY + nY;
            if (!D2COMMON_COLLISION_CheckMask(pRoom, nSpotX, nSpotY, COLLIDE_MISSILE_BARRIER))
            {
                nX = nSpotX;
                nY = nSpotY;
                bNoSpot = FALSE;
            }
        }

        ++nTries;
        if (!bNoSpot)
            break;
    }

    if (bNoSpot)
        return;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS;
    tParams.pOwner = pOwner;
    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
    tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    tParams.nMissile = MISSILE_ANYASTEAMVENT;
    tParams.nX = nX;
    tParams.nY = nY;
    MISSILE_ClientSpawn(&tParams);
    tParams.nMissile = MISSILE_ANYASTEAM;
    MISSILE_ClientSpawn(&tParams);
}


//1.10f: D2Client.0x6FB3D200
//  pCltDoFunc 55: ancient death center alone (541, Range 400), which QUESTSFX_OnAncientDeath spawns for the dying Ancient, aimed at his statue. On
//  every third frame lived (a SIGNED remainder; the counter from MISSILE_GetRemainingFrames): two rolls below 20; the position moved by a roll's low
//  bit, less 1, on each axis (X, its roll, Y, its roll); one ancient death cloud, flags 0x21, for the owner (asked, not tested), there, aimed at that
//  point plus the first two rolls, less 10. Then MISSILE_CltDoFunc1. The first two rolls are FUSED -- the first step stores only the low dword and
//  hands its high half to the second in edi, which stores both -- and are written in place.
//  The original inlines UNITS_GetXPosition, UNITS_GetYPosition and SEED_RollRandomNumber here.
void __fastcall MISSILE_CltDoFunc55(D2UnitStrc *pMissile)
{
    D2MissileStrc tParams;
    uint64_t lSeed;
    uint32_t nRollX;
    uint32_t nRollY;
    int32_t nSpawnX;
    int32_t nSpawnY;

    if (D2COMMON_MISSILE_GetRemainingFrames(pMissile) % 3 == 0)
    {
        lSeed = (uint64_t)pMissile->pSeed.nLowSeed * 0x6AC690C5u + pMissile->pSeed.nHighSeed;
        pMissile->pSeed.nLowSeed = (uint32_t)lSeed;
        nRollX = (uint32_t)lSeed % 20;
        lSeed = (uint64_t)(uint32_t)lSeed * 0x6AC690C5u + (uint32_t)(lSeed >> 32);
        pMissile->pSeed.nHighSeed = (uint32_t)(lSeed >> 32);
        pMissile->pSeed.nLowSeed = (uint32_t)lSeed;
        nRollY = (uint32_t)lSeed % 20;

        nSpawnX = UNITS_GetXPosition(pMissile);

        nSpawnX = (int32_t)((uint32_t)SEED_RollRandomNumber(&pMissile->pSeed) & 1) + nSpawnX - 1;

        nSpawnY = UNITS_GetYPosition(pMissile);

        nSpawnY = (int32_t)((uint32_t)SEED_RollRandomNumber(&pMissile->pSeed) & 1) + nSpawnY - 1;

        memset(&tParams, 0, sizeof(tParams));
        tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETPOS;
        tParams.pOwner = CUNIT_GetOwnerUnit(pMissile);
        tParams.nX = nSpawnX;
        tParams.nY = nSpawnY;
        tParams.nTargetX = (int32_t)nRollX + nSpawnX - 10;
        tParams.nTargetY = nSpawnY + (int32_t)nRollY - 10;
        tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
        tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        tParams.nMissile = MISSILE_ANCIENT_DEATH_CLOUD;
        MISSILE_ClientSpawn(&tParams);
    }

    MISSILE_CltDoFunc1(pMissile);
}


//  Layout checks for the missiles.txt members and game types the per-frame step reads.
static_assert(offsetof(D2MissilesTxt, dwMissileFlags) == 0x04 && offsetof(D2MissilesTxt, wCltDoFunc) == 0x08
                  && offsetof(D2MissilesTxt, wTravelSound) == 0x12 && offsetof(D2MissilesTxt, nCltSrcTown) == 0x12C
                  && offsetof(D2MissilesTxt, nFlicker) == 0x131 && offsetof(D2MissilesTxt, nLoopAnim) == 0x137
                  && offsetof(D2MissilesTxt, nSubLoop) == 0x180 && offsetof(D2MissilesTxt, nClientCol) == 0x185,
              "batch 23's missiles.txt members");
static_assert(GAMETYPE_SINGLEPLAYER == 0 && GAMETYPE_SINGLEPLAYER_UNCAPPED == 1, "batch 23's game types");


//1.10f: D2Client.0x6FB3D380
//  pCltDoFunc 60: Hurricane's debris, hurricaneswoosh, -cart, -rock, -sack, -tree and -vase (581..586, server do function 1), which
//  SKILLS_StatesSpawnHurricaneCltMissile spawns with the druid's position as TargetX/Y. On every frame lived that 2 divides (a SIGNED remainder: `and
//  80000001h / jns / dec / or / inc`): the offset from that point, (X - TargetX, Y - TargetY), turned a quarter -- the path aimed at (X + dy, Y - dx)
//  and started again (PATH_StartUnitMove, bAllowInTown 0) -- so the piece circles the point it was given. Then MISSILE_CltDoFunc1, by a tail jump.
//  The path is read once, before the two coordinates, and kept in a slot for the two calls.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall MISSILE_CltDoFunc60(D2UnitStrc *pMissile)
{
    D2DynamicPathStrc *pPath;
    int32_t nX;
    int32_t nY;
    int32_t nDeltaX;
    int32_t nDeltaY;

    if (D2COMMON_MISSILE_GetRemainingFrames(pMissile) % 2 == 0)
    {
        pPath = pMissile->pDynamicPath;

        nX = UNITS_GetXPosition(pMissile);

        nY = UNITS_GetYPosition(pMissile);

        nDeltaX = nX - D2COMMON_MISSILE_GetTargetX(pMissile);
        nDeltaY = nY - D2COMMON_MISSILE_GetTargetY(pMissile);
        D2COMMON_10170_PathSetTargetPos(pPath, nX + nDeltaY, nY - nDeltaX);
        D2COMMON_PATH_StartUnitMove(pPath, pMissile, 0);
    }

    MISSILE_CltDoFunc1(pMissile);
}


//1.10f: D2Client.0x6FB3D440
//  pCltHitFunc 62: baal taunt lightning control alone (654, Range 1, its CltHitSubMissile1 baal taunt lightning). The unit hit is never read. Nothing
//  without the missile, its missiles.txt row, a sub-missile id of 0 or more (the word at +0x2C, `jl`) or an owner. Then the missile's seed is
//  RESEEDED from its X (D2Common's SEED_InitLowSeed), the spread is CltHitPar1 floored at 1 (`jl`, signed), and two rolls below 2 * spread + 1 place
//  one sub-missile's target at the position plus each roll less the spread -- flags 0x20, for the owner, origin the missile, with its skill and
//  level. Answers 1.
//  The original inlines the Missiles.txt record lookup, UNITS_GetXPosition, UNITS_GetYPosition and SEED_RollLimitedRandomNumber here.
int32_t __fastcall MISSILE_CltHitFunc62(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pOwner;
    int32_t nMissileId;
    int32_t nSpread;
    int32_t nRange;
    int32_t nX;
    int32_t nY;
    int32_t nTargetX;
    int32_t nRollY;

    UNREFERENCED_PARAMETER(pTarget);

    if (!pMissile)
        return 1;

    nMissileId = pMissile->dwClassId;
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 1;
    pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    if (!pMissilesTxt || (int16_t)pMissilesTxt->wCltHitSubMissile[0] < 0)
        return 1;

    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
        return 1;

    D2COMMON_SEED_InitLowSeed(&pMissile->pSeed, UNITS_GetXPosition(pMissile));

    nSpread = (int32_t)pMissilesTxt->dwCltHitPar[0];
    if (nSpread < 1)
        nSpread = 1;

    nX = UNITS_GetXPosition(pMissile);

    nY = UNITS_GetYPosition(pMissile);

    nRange = 2 * nSpread + 1;

    nTargetX = SEED_RollLimitedRandomNumber(&pMissile->pSeed, nRange) - nSpread + nX;

    nRollY = SEED_RollLimitedRandomNumber(&pMissile->pSeed, nRange);

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_TARGETPOS;
    tParams.pOwner = pOwner;
    tParams.pOrigin = pMissile;
    tParams.nSkill = D2COMMON_MISSILE_GetSkill(pMissile);
    tParams.nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    tParams.nTargetX = nTargetX;
    tParams.nMissile = (int16_t)pMissilesTxt->wCltHitSubMissile[0];
    tParams.nTargetY = nRollY - nSpread + nY;
    MISSILE_ClientSpawn(&tParams);
    return 1;
}


//1.10f: D2Client.0x6FB3D660
//  pCltHitFunc 63: baal taunt poison control alone (655, Range 1, its CltHitSubMissile1 baal taunt poison). The unit hit is never read. With the row
//  and a sub-missile id of 0 or more (`test dx, dx / jl`): MISSILE_SpawnFromMissile's two rings of it -- every second of the sixteen directions, and
//  every CltHitPar1-th of entries 1 to 15, the column floored at 1 (`jge`, signed); no loops. Answers 1. No owner is asked for:
//  MISSILE_SpawnFromMissile takes the missile's own.
//  The original inlines the Missiles.txt record lookup here.
int32_t __fastcall MISSILE_CltHitFunc63(D2UnitStrc *pMissile, D2UnitStrc *pTarget)
{
    D2MissilesTxt *pMissilesTxt;
    int32_t nMissileId;
    int32_t nSubMissile;
    int32_t nDirectionStep;

    UNREFERENCED_PARAMETER(pTarget);

    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
        {
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
            if (pMissilesTxt)
            {
                nSubMissile = (int16_t)pMissilesTxt->wCltHitSubMissile[0];
                if (nSubMissile >= 0)
                {
                    nDirectionStep = (int32_t)pMissilesTxt->dwCltHitPar[0];
                    if (nDirectionStep < 1)
                        nDirectionStep = 1;
                    MISSILE_SpawnFromMissile(pMissile, nSubMissile, nDirectionStep, 2, 0);
                }
            }
        }
    }

    return 1;
}


//1.10f: D2Client.0x6FB3D6D0
//  One client frame of a missile -- UNITMODE_UpdateUnit's, for a unit of type 3. Without the missile or its missiles.txt row it is destroyed
//  (MISSILE_DestroyClientMissile, a tail jump). Then:
//    * the flicker -- only for a row with a Flicker: on a frame whose 8.8 sequence frame has bits 8 and 9 clear (`test ah, 3`: every fourth whole
//      frame), a missile with a light whose radius is at least the row's Light (`jl`, signed) gets a target radius of Light plus a roll below
//      Flicker. The row is looked up a SECOND time for it, over the id and count the first lookup read, its NULL arm never tested -- the first has
//      passed;
//    * the town -- a row without the Town flag (MISSILESFLAG_TOWN, bit 10: byte 1 of the flags against gdwBitMasks[2], 4) in a town room is
//      destroyed, a tail jump. A missile with no room tests NULL, which D2Common answers "not in town";
//    * the dispatch -- the row's CltDoFunc, a SIGNED word, taken when it is above 0 and below the count (`test ax, ax / jle`, `movsx`, `cmp / jge`),
//      and called through the table by a tail jump with the missile in ecx. Slot 0 is never reached.
//  With no room, the missile's Y and then its X are read and thrown away -- UNITS_GetYPosition's and UNITS_GetXPosition's dynamic arms, the path
//  calls made and their answers discarded, the static arms' loads dropped -- and the room stays NULL (`xor eax, eax`). The likeliest source is a room
//  search from those coordinates whose NULL-room head the compiler folded, keeping only the arguments' calls (CLIENT_FindRoomContainingSubtile
//  answers NULL for a NULL room); nothing in the bytes proves which function it was, so the copies are written in place.
//  The original inlines the Missiles.txt record lookup, SEED_RollLimitedRandomNumber, UNITS_GetYPosition and UNITS_GetXPosition here.
void __fastcall MISSILE_ClientUpdate(D2UnitStrc *pMissile)
{
    D2MissilesTxt *pMissilesTxt;
    D2MissilesTxt *pFlickerTxt;
    D2ClientLightStrc *pLight;
    D2ActiveRoomStrc *pRoom;
    int32_t nMissileId;
    int32_t nFlickerOffset;
    int32_t nDoFunc;

    if (!pMissile)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }
    nMissileId = pMissile->dwClassId;
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }
    pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    if (!pMissilesTxt)
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    if (pMissilesTxt->nFlicker)
    {
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pFlickerTxt = &sgptDataTables->pMissilesTxt[nMissileId];
        else
            pFlickerTxt = NULL;

        if (!(pMissile->nSeqCurrentFramePrecise & 0x300))
        {
            pLight = pMissile->pLight;
            if (pLight && DLIGHTMAP_GetRadius(pLight) >= pFlickerTxt->nLight)
            {
                nFlickerOffset = (int32_t)SEED_RollLimitedRandomNumberInline(&pMissile->pSeed, pFlickerTxt->nFlicker);
                DLIGHTMAP_SetTargetRadius(pLight, pFlickerTxt->nLight + nFlickerOffset);
            }
        }
    }

    pRoom = D2COMMON_UNITS_GetRoom(pMissile);
    if (!pRoom)
    {
        if (!(pMissile->dwUnitType == UNIT_OBJECT || ((int32_t)pMissile->dwUnitType > (int32_t)UNIT_MISSILE && (int32_t)pMissile->dwUnitType <= (int32_t)UNIT_TILE)) && pMissile->pDynamicPath)
        {
            D2COMMON_PATH_GetYPosition(pMissile->pDynamicPath);
        }

        if (!(pMissile->dwUnitType == UNIT_OBJECT || ((int32_t)pMissile->dwUnitType > (int32_t)UNIT_MISSILE && (int32_t)pMissile->dwUnitType <= (int32_t)UNIT_TILE)) && pMissile->pDynamicPath)
        {
            D2COMMON_PATH_GetXPosition(pMissile->pDynamicPath);
        }

        pRoom = NULL;
    }

    if (!((uint8_t)(pMissilesTxt->dwMissileFlags >> 8) & (uint8_t)gdwBitMasks[2]) && D2COMMON_DUNGEON_IsRoomInTown(pRoom))
    {
        MISSILE_DestroyClientMissile(pMissile);
        return;
    }

    nDoFunc = (int16_t)pMissilesTxt->wCltDoFunc;
    if (nDoFunc > 0 && nDoFunc < gnMissileCltDoFuncCount)
        gpMissileCltDoFuncs[nDoFunc](pMissile);
}


//1.10f: D2Client.0x6FB3D880
//  pCltDoFunc 1: one frame of a missile's flight -- the do function of many missiles.txt rows, and the step every other do function here ends in.
//  Without the missile or its row, nothing.
//  In a single-player game or as the host of an open or LAN game (game types 0, 1, 6 and 8, the switch's byte table), a missile whose owner is the
//  local player is marked. Then, once or TWICE:
//    * Before activate -- the frames it has left (MISSILE_GetCurrentFrame) at most its activate frame (`setle`, signed). Then, a missile that has
//      lived more frames than the row's InitSteps is drawn: UNITFLAGEX_NODRAW cleared -- InitSteps is "frames before the missile becomes visible".
//    * When PARTICLE_IsUnitMotionActive answers 0: MISSILE_ClientOnHit(missile, NULL, 1), and nothing more.
//    * Before activate, the animation: without LoopAnim, a frame stepped unless the cycle is at its end; with LoopAnim and SubLoop, past frame
//      AnimLen - SubStop + 1 (`jle`, signed) the sub-loop restarts (D2Common's UNITS_SetFrameNonRate, SubStart and SubStop); any other looping row
//      steps a frame.
//    * A missile with a velocity takes its path step (D2Common #10226, fed a constant 0); the frames left drop by one, and at 0 or below it ends:
//      MISSILE_ClientOnHit(missile, NULL, 0).
//    * After activate, nothing more.
//    * CltSrcTown: with no owner, or an owner in a town room, the frames left are cut to CltSrcTown unless already fewer (`jl`), and a row without
//      LoopAnim has its sequence frame moved to AnimLen - CltSrcTown when that is above 0 (`shl eax, 8` into +0x44): it is a count of frames.
//    * Mode 0 stops here. A path whose collision word (D2Common #10201) has bit 0 or 2 set ends in MISSILE_ClientOnHit(missile, NULL, 1); mode 6
//      stops here.
//    * Client collision -- for a row with ClientCol and a non-zero word: each point of the path's last step (D2Common #10198) is searched (#10407)
//      through the mode's record of gaMissileTargetTests, with the missile, its owner and its row as the test's context; the first unit found that is
//      not dead is hit: MISSILE_ClientOnHit(missile, unit, 0).
//    * The second pass -- a marked missile on its first frame (lived 1) that still has a velocity is unmarked and stepped again, from the top.
//  gaMissileTargetTests (0x6FB91218) has nine records of {test, collision mask}, the mask column the one MISSILE_InitClientUnit reads.
//  The original inlines the Missiles.txt record lookup here.
void __fastcall MISSILE_CltDoFunc1(D2UnitStrc *pMissile)
{
    D2MissilesTxt *pMissilesTxt;
    D2DynamicPathStrc *pPath;
    D2PathPointStrc *pPathPoints;
    D2PathPointStrc *pPoint;
    D2ActiveRoomStrc *pRoom;
    D2UnitStrc *pOwner;
    D2UnitStrc *pHit;
    D2ClientMissileTargetCtxStrc tTargetCtx;
    int32_t nMissileId;
    int32_t bOwnedByLocalPlayer;
    int32_t nActivateFrame;
    int32_t bBeforeActivate;
    int32_t nFramesAfterHold;
    uint32_t nMode;
    uint16_t nPathFlags;
    int32_t nSteps;
    int32_t nSize;
    int32_t i;

    if (!pMissile)
        return;

    nMissileId = pMissile->dwClassId;
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return;
    pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    if (!pMissilesTxt)
        return;

    bOwnedByLocalPlayer = FALSE;
    switch (gnGameType)
    {
    case GAMETYPE_SINGLEPLAYER:
    case GAMETYPE_SINGLEPLAYER_UNCAPPED:
    case GAMETYPE_OBNET_HOST:
    case GAMETYPE_LAN_HOST:
        pOwner = CUNIT_GetOwnerUnit(pMissile);
        if (pOwner == CUNIT_GetPlayer())
            bOwnedByLocalPlayer = TRUE;
        break;
    default:
        break;
    }

    for (;;)
    {
        nActivateFrame = D2COMMON_MISSILE_GetActivateFrame(pMissile);
        bBeforeActivate = D2COMMON_MISSILE_GetCurrentFrame(pMissile) <= nActivateFrame;
        if (bBeforeActivate && D2COMMON_MISSILE_GetRemainingFrames(pMissile) > pMissilesTxt->nInitSteps)
            pMissile->dwFlagEx &= ~UNITFLAGEX_NODRAW;

        if (!PARTICLE_IsUnitMotionActive(pMissile))
        {
            MISSILE_ClientOnHit(pMissile, NULL, 1);
            return;
        }

        if (bBeforeActivate)
        {
            if (!pMissilesTxt->nLoopAnim)
            {
                if (!D2COMMON_UNITS_IsAtEndOfFrameCycle(pMissile))
                    D2COMMON_UNITS_UpdateFrame(pMissile);
            }
            else if (pMissilesTxt->nSubLoop && D2COMMON_MISSILE_GetCurrentFrame(pMissile) > pMissilesTxt->nAnimLen - pMissilesTxt->nSubStop + 1)
            {
                D2COMMON_10375_UNITS_SetFrameNonRate(pMissile, pMissilesTxt->nSubStart, pMissilesTxt->nSubStop);
            }
            else
            {
                D2COMMON_UNITS_UpdateFrame(pMissile);
            }
        }

        pPath = pMissile->pDynamicPath;
        if (D2COMMON_PATH_GetVelocity(pPath))
            D2COMMON_10226(pMissile, UNRESOLVED_sub_6FAABFE0());

        D2COMMON_MISSILE_SetCurrentFrame(pMissile, D2COMMON_MISSILE_GetCurrentFrame(pMissile) - 1);
        if (D2COMMON_MISSILE_GetCurrentFrame(pMissile) <= 0)
        {
            MISSILE_ClientOnHit(pMissile, NULL, 0);
            return;
        }

        if (!bBeforeActivate)
            return;

        if (pMissilesTxt->nCltSrcTown)
        {
            pOwner = CUNIT_GetOwnerUnit(pMissile);
            if (!pOwner || ((pRoom = D2COMMON_UNITS_GetRoom(pOwner)) && D2COMMON_DUNGEON_IsRoomInTown(pRoom)))
            {
                if (D2COMMON_MISSILE_GetCurrentFrame(pMissile) >= pMissilesTxt->nCltSrcTown)
                {
                    D2COMMON_MISSILE_SetCurrentFrame(pMissile, pMissilesTxt->nCltSrcTown);
                    if (!pMissilesTxt->nLoopAnim)
                    {
                        nFramesAfterHold = pMissilesTxt->nAnimLen - pMissilesTxt->nCltSrcTown;
                        if (nFramesAfterHold > 0)
                            pMissile->nSeqCurrentFramePrecise = nFramesAfterHold << 8;
                    }
                }
            }
        }

        nMode = pMissile->dwAnimMode;
        if (!nMode)
            return;

        nPathFlags = D2COMMON_10201(pPath);
        if (nPathFlags & 5)
        {
            MISSILE_ClientOnHit(pMissile, NULL, 1);
            return;
        }

        if (nMode == 6)
            return;

        tTargetCtx.pMissile = pMissile;
        tTargetCtx.pOwner = CUNIT_GetOwnerUnit(pMissile);
        tTargetCtx.pMissilesTxt = pMissilesTxt;
        if (pMissilesTxt->nClientCol && nPathFlags)
        {
            nSteps = D2COMMON_10198_PathGetSaveStep(pPath, &pPathPoints);
            pRoom = D2COMMON_UNITS_GetRoom(pMissile);
            for (i = 0; i < nSteps; ++i)
            {
                pPoint = &pPathPoints[i];
                nSize = D2COMMON_UNITS_GetUnitSizeX(pMissile);
                pHit = D2COMMON_10407(pRoom, pPoint->X, pPoint->Y, gaMissileTargetTests[nMode].pfUnitFindCallback, &tTargetCtx, nSize);
                if (pHit && !CUNIT_IsUnitDead(pHit))
                {
                    MISSILE_ClientOnHit(pMissile, pHit, 0);
                    return;
                }
            }
        }

        if (!bOwnedByLocalPlayer || D2COMMON_MISSILE_GetRemainingFrames(pMissile) != 1 || !D2COMMON_PATH_GetVelocity(pPath))
        {
            return;
        }
        bOwnedByLocalPlayer = FALSE;
    }
}


//1.10f: D2Client.0x6FB3DBB0
//  A client missile's end. Nothing for no missile. With its row and a TravelSound of 0 or more (a signed word, `jl`): the sound, if still playing on
//  the missile, is detached (SOUNDINST_DetachFromUnit, not forced). Then the unit's light is taken from it (D2Common's UNITS_ResetLightMap) and
//  released to fade (DLIGHTMAP_ReleaseLight), and the unit removed -- by its client id when dwFlags bit 21 (UNITFLAG_ISASYNC) is set, else by its id:
//  `shr eax, 15h / and eax, 1 / cmp eax, 1`, each a tail jump. A missing row skips only the sound.
//  The original inlines the Missiles.txt record lookup here.
void __fastcall MISSILE_DestroyClientMissile(D2UnitStrc *pMissile)
{
    D2MissilesTxt *pMissilesTxt;
    D2ClientLightStrc *pLight;
    int32_t hSound;
    int32_t nMissileId;

    if (!pMissile)
        return;

    nMissileId = pMissile->dwClassId;
    if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
    {
        pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
        if (pMissilesTxt && (int16_t)pMissilesTxt->wTravelSound >= 0)
        {
            hSound = UNITSND_FindPlayingSoundInGroup(pMissile, (D2C_Sounds)(int16_t)pMissilesTxt->wTravelSound);
            if (hSound)
                SOUNDINST_DetachFromUnit(hSound, pMissile, 0);
        }
    }

    pLight = (D2ClientLightStrc *)D2COMMON_UNITS_ResetLightMap(pMissile);
    if (pLight)
        DLIGHTMAP_ReleaseLight(pLight);

    if (((pMissile->dwFlags >> 21) & 1) == 1)
        CUNIT_RemoveUnitByClientId(pMissile->dwUnitId, pMissile->dwUnitType);
    else
        CUNIT_RemoveUnitById(pMissile->dwUnitId, pMissile->dwUnitType);
}


//  Layout checks for the missiles.txt members MISSILE_ClientOnHit reads.
static_assert(offsetof(D2MissilesTxt, wCltHitFunc) == 0x0A && offsetof(D2MissilesTxt, wHitSound) == 0x14
                  && offsetof(D2MissilesTxt, wExplosionMissile) == 0x16
                  && offsetof(D2MissilesTxt, nCollideKill) == 0x186 && offsetof(D2MissilesTxt, nNextDelay) == 0x189
                  && offsetof(D2MissilesTxt, nAlwaysExplode) == 0x18C,
              "batch 24's missiles.txt members");
//  The states, stats and the class carried as literals (0x56, 0x45, 0x9C, 0x148, 0x92) and the target's flag bit tested (`shr ecx, 2`).
static_assert(STATE_JUSTHIT == 0x56 && STATE_PIERCE == 0x45 && STAT_ITEM_PIERCE == 0x9C && STAT_PIERCE_IDX == 0x148
                  && MISSILE_SPIDERGOO == 0x92 && UNITFLAG_CANBEATTACKED == (1 << 2),
              "batch 24's literals");


//1.10f: D2Client.0x6FB3DC50
//  A client missile's hit -- of pTarget, or of nothing (the end of its frames or of its path). bForceExplode is the one stack argument. Answers the
//  explosion missile it made, or NULL.
//    * The row is looked up with its NULL kept. The owner is asked for next (CUNIT_GetOwnerUnit, which asserts on a NULL unit and exits), and only
//      then is the missile tested -- so that NULL test can never fire.
//    * A target must not carry STATE_JUSTHIT when the row has NextHit, must not be the unit the missile last collided with (D2Common's
//      MISSILE_CheckUnitIfOwner and MISSILE_SetOwner: for a row with LastCollide they keep the last unit collided with) -- which it then becomes --
//      and must carry UNITFLAG_CANBEATTACKED (`shr ecx, 2 / and ecx, 1`, stored to a slot nothing reads); with an owner, it must be a selectable
//      target of the owner's (CUNIT_IsUnitSelectableTarget) unless the row has CollideFriend. Any refusal answers NULL and leaves the missile be.
//    * Pierce -- with an owner and a target, a row with the Pierce flag (MISSILESFLAG_PIERCE: byte 0 of the flags against gdwBitMasks[2], 4), an
//      owner of type 0 or 1 (`jl`/`jg`, signed) that has STATE_PIERCE or STAT_ITEM_PIERCE: the row is looked up AGAIN by MISSILE_GetClassId, its NULL
//      kept and read, and with the flag there too a missile with STAT_PIERCE_IDX left has it taken down by one and is not destroyed (the destroy
//      flags 3 -> 2).
//    * No target and neither bForceExplode nor AlwaysExplode: straight to the end. And the destroy flags' bit 1 is tested (`test byte ptr
//      [esp+84h+var_6C], 2`) -- they are 3 or 2, so it always passes.
//    * The hit: with NextHit and a target, a statlist of state JUSTHIT on the target -- NextDelay as its timeout, removed through
//      ITEM_OnUninterruptableStateRemoved -- posted, and the state set; the unit event 0 fired (CUNITEVENT_FireEvent); the row's CltHitFunc, a SIGNED
//      word taken above 0 and below the count, CALLED through the table with (missile, target) -- its 0 answers NULL, the missile left be; the
//      HitSound played; and an ExplosionMissile of 0 or more (read again, and the first read against -1) made for the missile's owner, or the missile
//      itself when it is a player or a monster: flags 0x20, origin the missile, TargetX/Y written 0, its skill and level; made, it takes the
//      missile's gfx coordinates, its motion stopped and its particle flag set, and the missile's direction -- spidergoo (146) a random one of 64
//      instead.
//    * The end: a target and a row without CollideKill, NULL. Otherwise the travel sound detached, the light released, and the missile removed when
//      the destroy flags keep bit 0 -- as MISSILE_DestroyClientMissile does, but gated, with this row and no NULL test: not a copy of it.
//  The original inlines the Missiles.txt record lookup and SEED_RollRandomNumber here.
D2UnitStrc *__fastcall MISSILE_ClientOnHit(D2UnitStrc *pMissile, D2UnitStrc *pTarget, int32_t bForceExplode)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2MissilesTxt *pPierceTxt;
    D2UnitStrc *pOwner;
    D2UnitStrc *pExplosionOwner;
    D2UnitStrc *pExplosion;
    D2StatListStrc *pStatList;
    D2ClientLightStrc *pLight;
    int32_t hSound;
    int32_t (__fastcall *pfnHit)(D2UnitStrc *, D2UnitStrc *);
    int32_t nMissileId;
    int32_t nExplosionMissile;
    int32_t nDestroyFlags;
    int32_t nPierceFlags;
    int32_t nPierceIdx;
    int32_t nHitFunc;
    int32_t nLevel;
    int32_t nSkillId;
    int32_t nGfxX;
    int32_t nGfxY;
    int32_t nGfxZ;
    uint8_t nDirection;

    pMissilesTxt = NULL;
    if (pMissile)
    {
        nMissileId = pMissile->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    }

    //  BUG: the row is not tested before ExplosionMissile is read, so no missile, or one with a class out of range, reads address 0x16.
    nExplosionMissile = (int16_t)pMissilesTxt->wExplosionMissile;
    nDestroyFlags = 3;
    pOwner = CUNIT_GetOwnerUnit(pMissile);
    pExplosion = NULL;
    if (!pMissile)
        return NULL;

    if (pTarget)
    {
        if (pMissilesTxt->nNextHit && D2COMMON_STATES_CheckState(pTarget, STATE_JUSTHIT))
            return NULL;
        if (D2COMMON_MISSILE_CheckUnitIfOwner(pMissile, pTarget))
            return NULL;
        D2COMMON_MISSILE_SetOwner(pMissile, pTarget);
        if (!((pTarget->dwFlags >> 2) & 1))
            return NULL;
        if (pOwner && !CUNIT_IsUnitSelectableTarget(pOwner, pTarget) && !pMissilesTxt->nCollideFriend)
            return NULL;
    }

    if (pOwner && pTarget && ((uint8_t)pMissilesTxt->dwMissileFlags & (uint8_t)gdwBitMasks[2]) && (int32_t)pOwner->dwUnitType >= 0 && (int32_t)pOwner->dwUnitType <= 1 && (D2COMMON_STATES_CheckState(pOwner, STATE_PIERCE)
            || D2COMMON_STATLIST_UnitGetItemStatOrSkillStatValue(pOwner, STAT_ITEM_PIERCE, 0)))
    {
        nPierceFlags = 3;

        nMissileId = D2COMMON_MISSILE_GetClassId(pMissile);
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
            pPierceTxt = &sgptDataTables->pMissilesTxt[nMissileId];
        else
            pPierceTxt = NULL;

        if (!((uint8_t)pPierceTxt->dwMissileFlags & (uint8_t)gdwBitMasks[2]))
        {
            nDestroyFlags = 3;
        }
        else
        {
            nPierceIdx = D2COMMON_STATLIST_UnitGetStatValue(pMissile, STAT_PIERCE_IDX, 0);
            if (nPierceIdx)
            {
                nPierceFlags = 2;
                D2COMMON_STATLIST_SetUnitStat(pMissile, STAT_PIERCE_IDX, nPierceIdx - 1, 0);
            }
            nDestroyFlags = nPierceFlags;
        }
    }

    if (!pTarget && !bForceExplode && !pMissilesTxt->nAlwaysExplode)
        goto TheEnd;

    //  The destroy flags are 3 or 2, both keep bit 1: never taken.
    if (!(nDestroyFlags & 2))
        goto TheEnd;

    if (pMissilesTxt->nNextHit && pTarget)
    {
        pStatList = D2COMMON_STATLIST_AllocStatList(NULL, 2, pMissilesTxt->nNextDelay, pTarget->dwUnitType, pTarget->dwUnitId);
        if (pStatList)
        {
            D2COMMON_STATLIST_SetState(pStatList, STATE_JUSTHIT);
            D2COMMON_STATLIST_SetStatRemoveCallback(pStatList, ITEM_OnUninterruptableStateRemoved);
            D2COMMON_10475_PostStatToStatList(pTarget, pStatList, 1);
            D2COMMON_STATES_ToggleState(pTarget, STATE_JUSTHIT, 1);
        }
    }

    CUNITEVENT_FireEvent(0, pTarget, pMissile, 0);

    nHitFunc = (int16_t)pMissilesTxt->wCltHitFunc;
    if (nHitFunc > 0 && nHitFunc < gnMissileCltHitFuncCount)
    {
        pfnHit = gpMissileCltHitFuncs[nHitFunc];
        if (pfnHit && !pfnHit(pMissile, pTarget))
            return NULL;
    }

    if ((int16_t)pMissilesTxt->wHitSound >= 0)
        SOUNDINST_PlaySound((D2C_Sounds)(int16_t)pMissilesTxt->wHitSound, pMissile, 0, 0, 0);

    if ((int16_t)pMissilesTxt->wExplosionMissile >= 0 && nExplosionMissile != -1)
    {
        nLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
        nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
        if (pMissile->dwUnitType == UNIT_PLAYER || pMissile->dwUnitType == UNIT_MONSTER)
            pExplosionOwner = pMissile;
        else
            pExplosionOwner = CUNIT_GetOwnerUnit(pMissile);

        if (!pExplosionOwner)
        {
            pExplosion = NULL;
        }
        else
        {
            memset(&tParams, 0, sizeof(tParams));
            tParams.pOwner = pExplosionOwner;
            tParams.dwFlags = MISSILESPAWN_TARGETPOS;
            tParams.pOrigin = pMissile;
            tParams.nTargetX = 0;
            tParams.nTargetY = 0;
            tParams.nSkill = nSkillId;
            tParams.nSkillLevel = nLevel;
            tParams.nMissile = nExplosionMissile;
            pExplosion = MISSILE_ClientSpawn(&tParams);
            if (pExplosion)
            {
                nGfxZ = PARTICLE_GetUnitGfxCoordZ(pMissile);
                nGfxY = PARTICLE_GetUnitGfxCoordY(pMissile);
                nGfxX = PARTICLE_GetUnitGfxCoordX(pMissile);
                PARTICLE_SetUnitGfxCoords(pExplosion, nGfxX, nGfxY, nGfxZ);
                PARTICLE_StopUnitMotion(pExplosion);
                PARTICLE_SetUnitParticleFlag(pExplosion);
                if (pExplosion->dwClassId == MISSILE_SPIDERGOO)
                {
                    nDirection = (uint8_t)((uint32_t)SEED_RollRandomNumber(&pExplosion->pSeed) & 0x3F);
                }
                else
                {
                    nDirection = D2COMMON_PATH_GetDirection(pMissile->pDynamicPath);
                }
                D2COMMON_10160_PathUpdateDirection(pExplosion->pDynamicPath, nDirection);
            }
        }
    }

TheEnd:
    if (pTarget && !pMissilesTxt->nCollideKill)
        return NULL;

    if ((int16_t)pMissilesTxt->wTravelSound >= 0)
    {
        hSound = UNITSND_FindPlayingSoundInGroup(pMissile, (D2C_Sounds)(int16_t)pMissilesTxt->wTravelSound);
        if (hSound)
            SOUNDINST_DetachFromUnit(hSound, pMissile, 0);
    }

    pLight = (D2ClientLightStrc *)D2COMMON_UNITS_ResetLightMap(pMissile);
    if (pLight)
        DLIGHTMAP_ReleaseLight(pLight);

    if (nDestroyFlags & 1)
    {
        if (((pMissile->dwFlags >> 21) & 1) == 1)
            CUNIT_RemoveUnitByClientId(pMissile->dwUnitId, pMissile->dwUnitType);
        else
            CUNIT_RemoveUnitById(pMissile->dwUnitId, pMissile->dwUnitType);
    }

    return pExplosion;
}


//  Layout checks for the missiles.txt row and spawn-block members MISSILE_ClientSpawn reads.
static_assert(offsetof(D2MissilesTxt, nVelLev) == 0x9B && offsetof(D2MissilesTxt, nMaxVel) == 0x9C
                  && offsetof(D2MissilesTxt, wAccel) == 0x9E && offsetof(D2MissilesTxt, nRGB) == 0x132
                  && offsetof(D2MissilesTxt, nActivate) == 0x136 && offsetof(D2MissilesTxt, nAnimSpeed) == 0x1A1
                  && offsetof(D2MissileStrc, nGfxArg) == 0x24 && offsetof(D2MissileStrc, nLightRadius) == 0x50,
              "batch 25's members");
//  The two missiles.txt flag indexes MISSILE_ClientSpawn spells by name; the bytes read gdwBitMasks[3] and [2] against byte 0 of the row's flags.
static_assert(MISSILESFLAGINDEX_CANSLOW == 3 && MISSILESFLAGINDEX_PIERCE == 2,
              "MISSILE_ClientSpawn's flag indexes");
//  The state and stats carried as literals (0x57, 0xA1, 0xA6), the flag cleared (`and ecx, 0FFFFFFF7h`) and the sound marked (0x13A, sounds.txt 314,
//  weapon_bow_draw_1).
static_assert(STATE_SLOWMISSILES == 0x57 && STAT_SKILL_HANDOFATHENA == 0xA1 && STAT_SKILL_PIERCE == 0xA6
                  && UNITFLAG_ISVALIDTARGET == 8,
              "batch 25's literals");
//  The names of the spawn flag word, D2C_MissileSpawnFlags: the first dword of D2MissileStrc, which every caller sets and this function tests bit by
//  bit. One assertion a value, each against the bit this function's test reads (or, for 0x1000 and 0x40000000, which it never reads, the store that
//  sets it).
static_assert(MISSILESPAWN_STARTPOS == 0x1, "`test byte ptr [ebx], 1`, 0x6FB3E0BD");
static_assert(MISSILESPAWN_TARGETOFFSET == 0x2, "the target point added to the start");
static_assert(MISSILESPAWN_VELOCITY == 0x4, "nVelocity given");
static_assert(MISSILESPAWN_SUBLOOPS == 0x8, "SubLoop's range times nLoops");
static_assert(MISSILESPAWN_VELOCITYSCALED == 0x10, "nVelocity already 8.8");
static_assert(MISSILESPAWN_TARGETPOS == 0x20, "the target point as given");
static_assert(MISSILESPAWN_ARC == 0x100, "an arc of nGfxArg over the frames");
static_assert(MISSILESPAWN_STARTFRAME == 0x200, "nFrame in");
static_assert(MISSILESPAWN_FRAMESTOTARGET == 0x400, "frames set to the travel time");
static_assert(MISSILESPAWN_ACTIVATEFRAME == 0x800, "nActivateFrame");
static_assert(MISSILESPAWN_ATTACKBONUS == 0x1000, "D2MOO's server only: STAT_TOHIT from nAttBonus");
static_assert(MISSILESPAWN_RANDOMDIR == 0x2000, "a random direction of 64");
static_assert(MISSILESPAWN_NOLIGHT == 0x4000, "`test ah, 40h`, 0x6FB3EA26");
static_assert(MISSILESPAWN_RANGE == 0x8000, "nRange is the range");
static_assert(MISSILESPAWN_FROMSERVER == 0x40000000, "SCMD_HandleSrv73_SpawnMissile's `mov [esp+80h+pParams], 40000001h`, 0x6FAB3FD2");


//1.10f: D2Client.0x6FB3E070
//  Makes a client missile from a D2MissileStrc -- the one spawner every other goes through. Answers the missile, or NULL. The flag word
//  (D2C_MissileSpawnFlags) decides which fields are read; each bit is named where it is tested below.
//    * The row of nMissile, or NULL.
//    * The start -- nX/nY with flag 1, else pOrigin's client position (NULL without an origin).
//    * A room for it -- the owner's room, or with no owner the local player's (neither: no room is asked for at all), searched for the start and then
//      its neighbours (CLIENT_FindRoomContainingSubtile), and failing that the act's room at that subtile (D2Common's
//      DUNGEON_FindRoomBySubtileCoordinates over gpAct); none, NULL. The room found is not used again: it only admits the spawn.
//    * The target point -- nTargetX/Y added to the start with flag 2, taken as they are with flag 0x20, else the start itself.
//    * The velocity -- nVelocity with flag 4, shifted up by 8 unless flag 0x10 says it already is; else the row's VelLev times nSkillLevel over 8
//      (SIGNED: `cdq / and edx, 7`) plus Vel, shifted up by 8. An owner in STATE_SLOWMISSILES, for a row with the CanSlow flag (byte 0 of the flags
//      against gdwBitMasks[3], 8), scales it by the state's statlist value of STAT_SKILL_HANDOFATHENA over 100. And a velocity that is not 0 is then
//      ALWAYS taken to 75%. Both through DATATBLS_ApplyRatio.
//    * Too far -- with a velocity: an owner with no target unit and a target point on the start gets the point in front of it
//      (SKILLS_GetPosInFrontOfUnit); a target unit on the owner's own position is dropped for that point too. Then a target (the unit's position, or
//      the point) 100 subtiles or more from the start on either axis makes nothing: NULL.
//    * The unit -- CUNIT_CreateClientOnlyUnit(nMissile, start, UNIT_MISSILE, 0), or NULL; its animation start frame, a statlist, a frame count of
//      AnimLen in 8.8, and an animation speed of AnimSpeed << 4.
//    * Its frames -- nRange with flag 0x8000, else LevRange times nSkillLevel plus Range (signed words), plus (SubStop - SubStart) times nLoops with
//      flag 8 and a SubLoop, plus the range scaled by 100 over the slow factor when that factor is neither 0 nor 100 (ADDED, not replaced). Flag
//      0x200 starts it nFrame in (the sequence frame too). Total and current set; the activate frame is that less nActivateFrame with flag 0x800,
//      else less the row's Activate.
//    * Its path -- none: the unit removed by its client id, NULL. Velocity 0, aimed at the target unit or the point, and with a velocity that
//      velocity and a start (PATH_StartUnitMove, not in town). The row's InitSteps hides it (UNITFLAGEX_NODRAW); the owner becomes its last collided
//      unit (D2Common's MISSILE_SetOwner); Accel and MaxVel << 8; flag 0x2000 a random direction of 64 from its own seed; particles. Flag 0x400 sets
//      both frame counts to the distance << 16 over the velocity << 4 (unsigned, `div ebp`), the distance at least 1 -- it lives exactly until it
//      arrives. Flag 0x100 arcs it (PARTICLE_StartUnitArc, nGfxArg high, over its total frames) and turns it to (nTargetX + nTargetY) & 0x3F -- a
//      byte sum.
//    * Skill and level; with an owner, the owner stored, and pierce: for an owner that is a player or a monster and a row with the Pierce flag, a
//      chance of STAT_SKILL_PIERCE plus STAT_ITEM_PIERCE, rolled below 100 up to four times on a LOCAL seed made from the owner's base
//      STAT_PIERCE_IDX -- or from 0 against a chance of 100 when the owner is a player other than the local one -- and the count of passes stored as
//      the missile's STAT_PIERCE_IDX.
//    * UNITFLAG_ISVALIDTARGET cleared. The light: none when gnLightQuality is 0 or with flag 0x4000; else, when the row has a Light or the block a
//      nLightRadius (its low byte, `mov al, [ebx+50h]`), the row's Light radius and colour at full intensity -- the block's radius only gates it.
//    * pInitFunc(missile, pInitArgs); a TravelSound that is not 0 played -- tested `!= 0` twice, where MISSILE_DestroyClientMissile and
//      MISSILE_ClientOnHit test `>= 0` -- and then the owner's playing weapon_bow_draw_1, if any, marked started; and with an owner, the monster
//      modifiers' spawn functions (MONUNIQUE_MonUModRunCltFuncsOnMissileSpawn).
//  The original inlines the Missiles.txt record lookup, UNITS_GetXPosition, UNITS_GetYPosition, CLIENT_FindRoomContainingSubtile, DATATBLS_ApplyRatio
//  and SEED_RollRandomNumber here.
D2UnitStrc *__fastcall MISSILE_ClientSpawn(D2MissileStrc *pParams)
{
    D2SeedStrc tPierceSeed;
    D2MissilesTxt *pMissilesTxt;
    D2MissilesTxt *pPierceTxt;
    D2UnitStrc *pPlayer;
    D2UnitStrc *pOwner;
    D2UnitStrc *pTarget;
    D2UnitStrc *pMissile;
    D2ActiveRoomStrc *pRoom;
    D2DynamicPathStrc *pPath;
    D2StatListStrc *pStatList;
    int32_t hSound;
    uint32_t nDistance;
    int32_t nMissileId;
    int32_t nX;
    int32_t nY;
    int32_t nTargetX;
    int32_t nTargetY;
    int32_t nUnitX;
    int32_t nUnitY;
    int32_t nDeltaX;
    int32_t nDeltaY;
    int32_t nVelocity;
    int32_t nSlowFactor;
    int32_t nRange;
    int32_t nFrames;
    int32_t nPierceChance;
    int32_t nPierceCount;
    int32_t nLowSeed;
    int32_t nSoundId;
    uint8_t nDirection;

    nMissileId = pParams->nMissile;
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return NULL;
    pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    if (!pMissilesTxt)
        return NULL;

    if (pParams->dwFlags & MISSILESPAWN_STARTPOS)
    {
        nX = pParams->nX;
        nY = pParams->nY;
    }
    else
    {
        if (!pParams->pOrigin)
            return NULL;

        nX = UNITS_GetXPosition(pParams->pOrigin);

        nY = UNITS_GetYPosition(pParams->pOrigin);
    }

    if (pParams->pOwner)
    {
        pRoom = CLIENT_FindRoomContainingSubtile(D2COMMON_UNITS_GetRoom(pParams->pOwner), nX, nY);
    }
    else
    {
        pPlayer = CUNIT_GetPlayer();
        if (!pPlayer)
            goto NoRoomAsked;

        pRoom = CLIENT_FindRoomContainingSubtile(D2COMMON_UNITS_GetRoom(pPlayer), nX, nY);
    }

    if (!pRoom && !D2COMMON_DUNGEON_FindRoomBySubtileCoordinates(gpAct, nX, nY))
        return NULL;

NoRoomAsked:
    if (pParams->dwFlags & MISSILESPAWN_TARGETOFFSET)
    {
        nTargetX = pParams->nTargetX + nX;
        nTargetY = pParams->nTargetY + nY;
    }
    else if (pParams->dwFlags & MISSILESPAWN_TARGETPOS)
    {
        nTargetX = pParams->nTargetX;
        nTargetY = pParams->nTargetY;
    }
    else
    {
        nTargetX = nX;
        nTargetY = nY;
    }

    if (pParams->dwFlags & MISSILESPAWN_VELOCITY)
    {
        nVelocity = pParams->nVelocity;
        if (!(pParams->dwFlags & MISSILESPAWN_VELOCITYSCALED))
            nVelocity <<= 8;
    }
    else
    {
        nVelocity = (pMissilesTxt->nVelLev * pParams->nSkillLevel / 8 + pMissilesTxt->nVel) << 8;
    }

    nSlowFactor = 100;
    if (pParams->pOwner && ((uint8_t)pMissilesTxt->dwMissileFlags & (uint8_t)gdwBitMasks[MISSILESFLAGINDEX_CANSLOW]) && D2COMMON_STATES_CheckState(pParams->pOwner, STATE_SLOWMISSILES))
    {
        pStatList = D2COMMON_STATLIST_GetStatListFromUnitAndState(pParams->pOwner, STATE_SLOWMISSILES);
        if (pStatList)
        {
            nSlowFactor = D2COMMON_STATLIST_GetStatValue(pStatList, STAT_SKILL_HANDOFATHENA, 0);

            nVelocity = DATATBLS_ApplyRatio(nVelocity, nSlowFactor, 100);
        }
    }

    if (nVelocity)
    {
        nVelocity = DATATBLS_ApplyRatio(nVelocity, 75, 100);
    }

    pTarget = pParams->pTarget;
    if (nVelocity)
    {
        pOwner = pParams->pOwner;
        if (pOwner && !pTarget)
        {
            if (nTargetX == nX && nTargetY == nY)
                SKILLS_GetPosInFrontOfUnit(pOwner, &nTargetX, &nTargetY);
        }
        else if (pTarget && pOwner && pTarget != pOwner)
        {
            nUnitX = UNITS_GetXPosition(pTarget);
            if (nUnitX == UNITS_GetXPosition(pParams->pOwner))
            {
                nUnitY = UNITS_GetYPosition(pTarget);
                if (nUnitY == UNITS_GetYPosition(pParams->pOwner))
                {
                    SKILLS_GetPosInFrontOfUnit(pParams->pOwner, &nTargetX, &nTargetY);
                    pTarget = NULL;
                }
            }
        }

        nDeltaX = nTargetX;
        nDeltaY = nTargetY;
        if (pTarget)
        {
            nDeltaX = UNITS_GetXPosition(pTarget);

            nDeltaY = UNITS_GetYPosition(pTarget);
        }

        nDeltaX -= nX;
        if (nDeltaX < 0)
            nDeltaX = -nDeltaX;
        if (nDeltaX >= 100)
            return NULL;
        nDeltaY -= nY;
        if (nDeltaY < 0)
            nDeltaY = -nDeltaY;
        if (nDeltaY >= 100)
            return NULL;
    }

    pMissile = CUNIT_CreateClientOnlyUnit(pParams->nMissile, nX, nY, UNIT_MISSILE, 0);
    if (!pMissile)
        return NULL;

    D2COMMON_UNITS_SetAnimStartFrame(pMissile);
    D2COMMON_STATLIST_AllocStatListEx(pMissile, 0, NULL, NULL);
    pMissile->dwFrameCountPrecise = pMissilesTxt->nAnimLen << 8;
    D2COMMON_MISSILE_SetAnimSpeed(pMissile, pMissilesTxt->nAnimSpeed << 4);

    if (pParams->dwFlags & MISSILESPAWN_RANGE)
    {
        nRange = pParams->nRange;
    }
    else
    {
        nRange = (int16_t)pMissilesTxt->wLevRange * pParams->nSkillLevel + (int16_t)pMissilesTxt->wRange;
        if ((pParams->dwFlags & MISSILESPAWN_SUBLOOPS) && pMissilesTxt->nSubLoop)
            nRange += (pMissilesTxt->nSubStop - pMissilesTxt->nSubStart) * pParams->nLoops;

        if (nSlowFactor && nSlowFactor != 100)
        {
            nRange += DATATBLS_ApplyRatio(nRange, 100, nSlowFactor);
        }
    }

    if (pParams->dwFlags & MISSILESPAWN_STARTFRAME)
    {
        nRange -= pParams->nFrame;
        pMissile->nSeqCurrentFramePrecise = pParams->nFrame << 8;
    }
    D2COMMON_MISSILE_SetTotalFrames(pMissile, nRange);
    D2COMMON_MISSILE_SetCurrentFrame(pMissile, nRange);
    if (pParams->dwFlags & MISSILESPAWN_ACTIVATEFRAME)
        nRange -= pParams->nActivateFrame;
    else
        nRange -= pMissilesTxt->nActivate;
    D2COMMON_MISSILE_SetActivateFrame(pMissile, nRange);

    pPath = pMissile->pDynamicPath;
    if (!pPath)
    {
        CUNIT_RemoveUnitByClientId(pMissile->dwUnitId, pMissile->dwUnitType);
        return NULL;
    }

    D2COMMON_PATH_SetVelocity(pPath, 0, szMissileCpp, 7924);
    if (pTarget)
        D2COMMON_PATH_SetTargetUnit(pPath, pTarget);
    else
        D2COMMON_10170_PathSetTargetPos(pPath, nTargetX, nTargetY);
    if (nVelocity)
    {
        D2COMMON_PATH_SetVelocity(pPath, nVelocity, szMissileCpp, 7935);
        D2COMMON_PATH_StartUnitMove(pPath, pMissile, 0);
    }

    if (pMissilesTxt->nInitSteps)
        pMissile->dwFlagEx |= UNITFLAGEX_NODRAW;

    D2COMMON_MISSILE_SetOwner(pMissile, pParams->pOwner);
    D2COMMON_PATH_SetAcceleration(pPath, (int16_t)pMissilesTxt->wAccel);
    D2COMMON_PATH_SetMaxVelocity(pPath, pMissilesTxt->nMaxVel << 8);

    if (pParams->dwFlags & MISSILESPAWN_RANDOMDIR)
    {
        nDirection = (uint8_t)((uint32_t)SEED_RollRandomNumber(&pMissile->pSeed) & 0x3F);
        D2COMMON_PATH_SetDirection(pMissile->pDynamicPath, nDirection);
    }

    PARTICLE_AllocUnitParticles(pMissile);
    PARTICLE_SetUnitParticleFlag(pMissile);

    if (pParams->dwFlags & MISSILESPAWN_FRAMESTOTARGET)
    {
        nDistance = D2COMMON_UNITS_GetDistanceToCoordinates(pMissile, nTargetX, nTargetY);
        if (!nDistance)
            nDistance = 1;
        nVelocity <<= 4;
        if (nVelocity)
        {
            nFrames = (int32_t)((nDistance << 16) / (uint32_t)nVelocity);
            D2COMMON_MISSILE_SetCurrentFrame(pMissile, nFrames);
            D2COMMON_MISSILE_SetTotalFrames(pMissile, nFrames);
        }
    }

    if (pParams->dwFlags & MISSILESPAWN_ARC)
    {
        PARTICLE_StartUnitArc(pMissile, pParams->nGfxArg, D2COMMON_MISSILE_GetTotalFrames(pMissile));
        PARTICLE_UpdateUnitParticle(pMissile);
        D2COMMON_PATH_SetDirection(pPath, (uint8_t)((nTargetY + nTargetX) & 0x3F));
        D2COMMON_10203_PATH_SetRotateFlag(pPath, 0);
    }

    D2COMMON_MISSILE_SetSkill(pMissile, (D2C_Skills)pParams->nSkill);
    D2COMMON_MISSILE_SetLevel(pMissile, (uint16_t)pParams->nSkillLevel);

    if (pParams->pOwner)
    {
        D2COMMON_UNITS_StoreOwner(pMissile, pParams->pOwner);
        pOwner = pParams->pOwner;
        nPierceCount = 0;
        if (!pOwner || pOwner->dwUnitType == UNIT_PLAYER || pOwner->dwUnitType == UNIT_MONSTER)
        {
            nMissileId = D2COMMON_MISSILE_GetClassId(pMissile);
            if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
                pPierceTxt = &sgptDataTables->pMissilesTxt[nMissileId];
            else
                pPierceTxt = NULL;

            if ((uint8_t)pPierceTxt->dwMissileFlags & (uint8_t)gdwBitMasks[MISSILESFLAGINDEX_PIERCE])
            {
                nPierceChance = D2COMMON_STATLIST_UnitGetStatValue(pOwner, STAT_SKILL_PIERCE, 0);
                nPierceChance += D2COMMON_STATLIST_UnitGetItemStatOrSkillStatValue(pOwner, STAT_ITEM_PIERCE, 0);
                if (nPierceChance)
                {
                    if (pOwner && pOwner->dwUnitType == UNIT_PLAYER && CUNIT_GetPlayer() != pOwner)
                    {
                        nPierceChance = 100;
                        nLowSeed = 0;
                    }
                    else
                    {
                        nLowSeed = D2COMMON_STATLIST_GetUnitBaseStat(pOwner, STAT_PIERCE_IDX, 0);
                    }

                    D2COMMON_SEED_InitLowSeed(&tPierceSeed, nLowSeed);
                    while (nPierceCount < 4)
                    {
                        if ((int32_t)((uint32_t)SEED_RollRandomNumber(&tPierceSeed) % 100) >= nPierceChance)
                            break;
                        ++nPierceCount;
                    }
                    D2COMMON_STATLIST_SetUnitStat(pMissile, STAT_PIERCE_IDX, nPierceCount, 0);
                }
            }
        }
    }

    pMissile->dwFlags &= ~UNITFLAG_ISVALIDTARGET;

    if (!gnLightQuality || (pParams->dwFlags & MISSILESPAWN_NOLIGHT))
    {
        CUNIT_GfxFreeUnitLightMap(pMissile);
    }
    else if (pMissilesTxt->nLight || (uint8_t)pParams->nLightRadius)
    {
        CUNIT_GfxFreeUnitLightMap(pMissile);
        pMissile->pLight = DLIGHTMAP_AddLight(pMissile, 1, pMissilesTxt->nLight, 0xFF, pMissilesTxt->nRGB[0], pMissilesTxt->nRGB[1], pMissilesTxt->nRGB[2]);
    }

    if (pParams->pInitFunc)
        pParams->pInitFunc(pMissile, pParams->pInitArgs);

    if (pMissilesTxt->wTravelSound)
    {
        nSoundId = (int16_t)pMissilesTxt->wTravelSound;
        if (nSoundId)
        {
            SOUNDINST_PlaySound((D2C_Sounds)nSoundId, pMissile, 0, 0, 0);
            if (pParams->pOwner)
            {
                hSound = UNITSND_FindPlayingSoundInGroup(pParams->pOwner, SOUND_WEAPON_BOW_DRAW_1);
                if (hSound)
                    SOUNDINST_MarkStarted(hSound);
            }
        }
    }

    if (pParams->pOwner)
        MONUNIQUE_MonUModRunCltFuncsOnMissileSpawn(pParams->pOwner, pMissile);

    return pMissile;
}


//  The three wrappers and a skill effect's block: four functions build a D2MissileStrc on the stack -- each clears it whole (`mov ecx, 17h / rep
//  stosd`, 23 dwords) AFTER its guards -- fill a few fields and hand it to MISSILE_ClientSpawn, whose answer each returns as it is. They differ in
//  the flag word and in who the owner is.


//1.10f: D2Client.0x6FB3EAF0
//  Spawns a client missile at a point -- the wrapper with the most callers. Flag 1: the start is nSpawnX/nSpawnY as given. No other flag is set, so
//  MISSILE_ClientSpawn takes the start for the target point too, and a missile its row gives a velocity is aimed at the point in front of the owner
//  instead. The first argument is the OWNER -- the block's pOwner, and nothing else. No owner: NULL (eax is zeroed first and the test comes before
//  the clear).
D2UnitStrc *__fastcall MISSILE_ClientSpawnSimple(D2UnitStrc *pOwner, int32_t nMissileId, int32_t nSpawnX, int32_t nSpawnY, int32_t nSkillId, int32_t nSkillLevel)
{
    D2MissileStrc tParams;

    if (!pOwner)
        return NULL;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS;
    tParams.pOwner = pOwner;
    tParams.nMissile = nMissileId;
    tParams.nX = nSpawnX;
    tParams.nY = nSpawnY;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;
    return MISSILE_ClientSpawn(&tParams);
}


//1.10f: D2Client.0x6FB3EB50
//  Spawns a client missile from a unit, for its owner. Flag 0x20: the target point is nTargetX/nTargetY as given. No flag 1, so the start is
//  pOrigin's client position, and pOrigin is the unit.
//  The owner is the unit itself when it is a player or a monster -- two equality tests on dwUnitType, `test eax, eax` and `cmp eax, 1` -- and any
//  other unit's CUNIT_GetOwnerUnit; with none, NULL before anything is cleared. So a monster, a hireling or a summon among them, spawns for itself
//  and not for its master: the lookup is for the other types.
D2UnitStrc *__fastcall MISSILE_ClientSpawnForOwner(D2UnitStrc *pUnit, int32_t nMissileId, int32_t nTargetX, int32_t nTargetY, int32_t nSkillId, int32_t nSkillLevel)
{
    D2MissileStrc tParams;
    D2UnitStrc *pOwner;

    if (!pUnit)
        return NULL;

    pOwner = pUnit;
    if (pUnit->dwUnitType != UNIT_PLAYER && pUnit->dwUnitType != UNIT_MONSTER)
    {
        pOwner = CUNIT_GetOwnerUnit(pUnit);
        if (!pOwner)
            return NULL;
    }

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_TARGETPOS;
    tParams.pOwner = pOwner;
    tParams.pOrigin = pUnit;
    tParams.nMissile = nMissileId;
    tParams.nTargetX = nTargetX;
    tParams.nTargetY = nTargetY;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;
    return MISSILE_ClientSpawn(&tParams);
}


//1.10f: D2Client.0x6FB3EBE0
//  Spawns a skill's client effect: it fills the block and spawns the missile, answering it, as the three wrappers do. Flags 0x521:
//    0x1    the start as given -- here the unit's own client position plus nSpawnOffsetX/nSpawnOffsetY; no pOrigin is set
//    0x20   the target point as given
//    0x400  the missile's frames set to its travel time to the point
//    0x100  an arc of nGfxArg over those frames, which MISSILE_ClientSpawn reads after the 0x400 step -- nGfxArg, or 30 when it is 0 (`mov ebp, 1Eh`)
//  The owner as in MISSILE_ClientSpawnForOwner. A target point with either coordinate 0 is replaced by the path target of the UNIT (`mov ecx, esi`)
//  -- where MISSILE_ClientSpawnAtPathTarget asks its owner's -- written into the argument slots themselves (`lea edx`). The answer is tested and
//  cannot be 0: SKILLS_GetPathTargetPos answers 1 on every path. Then a coordinate still 0 makes nothing, which after that call only a step in front
//  of the unit landing on 0 could give.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
D2UnitStrc *__fastcall MISSILE_ClientSpawnSkillEffect(D2UnitStrc *pUnit, int32_t nMissileId, int32_t nSpawnOffsetX, int32_t nSpawnOffsetY, int32_t nTargetX, int32_t nTargetY, int32_t nSkillId, int32_t nSkillLevel, int32_t nGfxArg)
{
    D2MissileStrc tParams;
    D2UnitStrc *pOwner;
    int32_t nGfxArgOrDefault;

    if (!pUnit)
        return NULL;

    pOwner = pUnit;
    if (pUnit->dwUnitType != UNIT_PLAYER && pUnit->dwUnitType != UNIT_MONSTER)
    {
        pOwner = CUNIT_GetOwnerUnit(pUnit);
        if (!pOwner)
            return NULL;
    }

    if ((!nTargetX || !nTargetY) && !SKILLS_GetPathTargetPos(pUnit, &nTargetX, &nTargetY))
        return NULL;

    if (!nTargetX || !nTargetY)
        return NULL;

    nGfxArgOrDefault = nGfxArg;
    if (!nGfxArgOrDefault)
        nGfxArgOrDefault = 30;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETPOS | MISSILESPAWN_ARC | MISSILESPAWN_FRAMESTOTARGET;
    tParams.pOwner = pOwner;

    tParams.nX = UNITS_GetXPosition(pUnit) + nSpawnOffsetX;

    tParams.nY = UNITS_GetYPosition(pUnit) + nSpawnOffsetY;

    tParams.nMissile = nMissileId;
    tParams.nTargetX = nTargetX;
    tParams.nTargetY = nTargetY;
    tParams.nGfxArg = nGfxArgOrDefault;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;
    return MISSILE_ClientSpawn(&tParams);
}


//1.10f: D2Client.0x6FB3ED40
//  Spawns a client missile at where the owner is heading; one caller, SKILLS_CltDoFunc72. Flags 0x420: the target point as given -- the OWNER's path
//  target (`mov ecx, ebx`), which is the unit itself only for a player or a monster -- and the frames set to the travel time to it. No flag 1: the
//  start is pOrigin's client position, and pOrigin is the unit. The owner as in MISSILE_ClientSpawnForOwner. The path target's answer is tested and
//  cannot be 0, as above.
D2UnitStrc *__fastcall MISSILE_ClientSpawnAtPathTarget(D2UnitStrc *pUnit, int32_t nMissileId, int32_t nSkillId, int32_t nSkillLevel)
{
    D2MissileStrc tParams;
    D2UnitStrc *pOwner;
    int32_t nTargetX;
    int32_t nTargetY;

    if (!pUnit)
        return NULL;

    pOwner = pUnit;
    if (pUnit->dwUnitType != UNIT_PLAYER && pUnit->dwUnitType != UNIT_MONSTER)
    {
        pOwner = CUNIT_GetOwnerUnit(pUnit);
        if (!pOwner)
            return NULL;
    }

    if (!SKILLS_GetPathTargetPos(pOwner, &nTargetX, &nTargetY))
        return NULL;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_TARGETPOS | MISSILESPAWN_FRAMESTOTARGET;
    tParams.pOwner = pOwner;
    tParams.pOrigin = pUnit;
    tParams.nMissile = nMissileId;
    tParams.nTargetX = nTargetX;
    tParams.nTargetY = nTargetY;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;
    return MISSILE_ClientSpawn(&tParams);
}

//  mods.h -- the data of mods.cpp that is ours: the period-of-day tbl ids, the set-bonus states, the six-bit population count, the client stat
//  descriptors and the elixir stat labels. Each is a C++17 inline variable, so every file that includes this header shares one object.
//  Address range 0x6FB6FA30..0x6FB7A4A0. Include it after D2Client.h.
#pragma once

//  D2Client 0x6FB6FA30
//  The four .tbl string ids of the period of day (0x52F3, 0x52F5, 0x52F2, 0x52F4), read by MODS_FormatPropertyLine.
inline const D2C_StringIndices gaPeriodOfDayStrIndices[4] =
{ STR_IDX_21235_ModStre9e, STR_IDX_21237_ModStre9g, STR_IDX_21234_ModStre9d, STR_IDX_21236_ModStre9f };
static_assert(sizeof(gaPeriodOfDayStrIndices) == 8, "gaPeriodOfDayStrIndices: the item's 8 bytes in the .i64");

//  D2Client 0x6FB6FA38
//  The six set-bonus state ids, 165..170. Read by MODS_AppendSetBonusText and MODS_AppendStateModsText.
inline const D2C_States gnSetBonusStates[6] = { STATE_ITEMSET1, STATE_ITEMSET2, STATE_ITEMSET3, STATE_ITEMSET4, STATE_ITEMSET5, STATE_ITEMSET6 };
static_assert(sizeof(gnSetBonusStates) == 24, "gnSetBonusStates: the item's 24 bytes in the .i64");

//  D2Client 0x6FB6FA50
//  The population count of 0..63, read by MODS_AppendSetBonusText.
inline const int32_t gnSixBitPopCount[64] =
{
    0,   // [0]
    1,   // [1]
    1,   // [2]
    2,   // [3]
    1,   // [4]
    2,   // [5]
    2,   // [6]
    3,   // [7]
    1,   // [8]
    2,   // [9]
    2,   // [10]
    3,   // [11]
    2,   // [12]
    3,   // [13]
    3,   // [14]
    4,   // [15]
    1,   // [16]
    2,   // [17]
    2,   // [18]
    3,   // [19]
    2,   // [20]
    3,   // [21]
    3,   // [22]
    4,   // [23]
    2,   // [24]
    3,   // [25]
    3,   // [26]
    4,   // [27]
    3,   // [28]
    4,   // [29]
    4,   // [30]
    5,   // [31]
    1,   // [32]
    2,   // [33]
    2,   // [34]
    3,   // [35]
    2,   // [36]
    3,   // [37]
    3,   // [38]
    4,   // [39]
    2,   // [40]
    3,   // [41]
    3,   // [42]
    4,   // [43]
    3,   // [44]
    4,   // [45]
    4,   // [46]
    5,   // [47]
    2,   // [48]
    3,   // [49]
    3,   // [50]
    4,   // [51]
    3,   // [52]
    4,   // [53]
    4,   // [54]
    5,   // [55]
    3,   // [56]
    4,   // [57]
    4,   // [58]
    5,   // [59]
    4,   // [60]
    5,   // [61]
    5,   // [62]
    6,   // [63]
};
static_assert(sizeof(gnSixBitPopCount) == 256, "gnSixBitPopCount: the item's 256 bytes in the .i64");

//  D2Client 0x6FB79B48
//  A copy inside D2Client of itemstatcost.txt's description columns: 143 records of 16 bytes, the first with id -1. Only the stat id is read.
inline D2ClientStatDescStrc gptClientStatDescs[143] =
{
    { .nStatId = STAT_INVALID, .nDescFunc = 3 },   // [0]
    {   // [1]
        .nStatId = STAT_ITEM_REQ_PERCENT,
        .nDescFunc = 4,
        .wDescStrPos = STR_IDX_3532_ModStr3h,
        .wDescStrNeg = STR_IDX_3532_ModStr3h,
        .nDescVal = 2,
    },
    {   // [2]
        .nStatId = STAT_ITEM_REPLENISH_DURABILITY,
        .nDescFunc = 11,
        .wDescStrPos = STR_IDX_21241_ModStre9t,
        .wDescStrNeg = STR_IDX_21241_ModStre9t,
    },
    {   // [3]
        .nStatId = STAT_ITEM_REPLENISH_QUANTITY,
        .nDescFunc = 3,
        .wDescStrPos = STR_IDX_21243_ModStre9v,
        .wDescStrNeg = STR_IDX_21243_ModStre9v,
    },
    {   // [4]
        .nStatId = STAT_ITEM_MAXDURABILITY_PERCENT,
        .nDescFunc = 2,
        .wDescStrPos = STR_IDX_3507_ModStr2i,
        .wDescStrNeg = STR_IDX_3507_ModStr2i,
        .nDescVal = 2,
    },
    { .nStatId = STAT_ITEM_EXTRA_STACK, .nDescFunc = 3, .wDescStrPos = STR_IDX_21239_ModStre9i, .wDescStrNeg = STR_IDX_21239_ModStre9i },   // [5]
    { .nStatId = STAT_ITEM_THROWABLE, .nDescFunc = 3, .wDescStrPos = STR_IDX_3575_ModStr5a, .wDescStrNeg = STR_IDX_3575_ModStr5a },   // [6]
    {   // [7]
        .nStatId = STAT_ITEM_LIGHTRADIUS,
        .nDescFunc = 1,
        .wDescStrPos = STR_IDX_3530_ModStr3f,
        .wDescStrNeg = STR_IDX_3530_ModStr3f,
        .nDescVal = 1,
    },
    {   // [8]
        .nStatId = STAT_ITEM_FIND_MAGIC_PERLEVEL,
        .nDescFunc = 7,
        .wDescStrPos = STR_IDX_11033_ModStr1x,
        .wDescStrNeg = STR_IDX_11033_ModStr1x,
        .nDescVal = 1,
    },
    {   // [9]
        .nStatId = STAT_ITEM_MAGICBONUS,
        .nDescFunc = 2,
        .wDescStrPos = STR_IDX_11033_ModStr1x,
        .wDescStrNeg = STR_IDX_11033_ModStr1x,
        .nDescVal = 1,
    },
    {   // [10]
        .nStatId = STAT_ITEM_FIND_GOLD_PERLEVEL,
        .nDescFunc = 7,
        .wDescStrPos = STR_IDX_11032_ModStr1w,
        .wDescStrNeg = STR_IDX_11032_ModStr1w,
        .nDescVal = 1,
    },
    {   // [11]
        .nStatId = STAT_ITEM_GOLDBONUS,
        .nDescFunc = 2,
        .wDescStrPos = STR_IDX_11032_ModStr1w,
        .wDescStrNeg = STR_IDX_11032_ModStr1w,
        .nDescVal = 1,
    },
    {   // [12]
        .nStatId = STAT_ITEM_DAMAGETOMANA,
        .nDescFunc = 2,
        .wDescStrPos = STR_IDX_3545_ModStr3w,
        .wDescStrNeg = STR_IDX_3545_ModStr3w,
        .nDescVal = 1,
    },
    {   // [13]
        .nStatId = STAT_ITEM_THORNS_PERLEVEL,
        .nDescFunc = 9,
        .wDescStrPos = STR_IDX_3494_ModStr1v,
        .wDescStrNeg = STR_IDX_3494_ModStr1v,
        .nDescVal = 2,
    },
    {   // [14]
        .nStatId = STAT_ITEM_ATTACKERTAKESLIGHTDAMAGE,
        .nDescFunc = 3,
        .wDescStrPos = STR_IDX_3534_ModStr3j,
        .wDescStrNeg = STR_IDX_3534_ModStr3j,
        .nDescVal = 2,
    },
    {   // [15]
        .nStatId = STAT_ITEM_ATTACKERTAKESDAMAGE,
        .nDescFunc = 3,
        .wDescStrPos = STR_IDX_3494_ModStr1v,
        .wDescStrNeg = STR_IDX_3494_ModStr1v,
        .nDescVal = 2,
    },
    {   // [16]
        .nStatId = STAT_ITEM_HEALAFTERDEMONKILL,
        .nDescFunc = 1,
        .wDescStrPos = STR_IDX_3603_ModStr6c,
        .wDescStrNeg = STR_IDX_3603_ModStr6c,
        .nDescVal = 1,
    },
    {   // [17]
        .nStatId = STAT_ITEM_MANAAFTERKILL,
        .nDescFunc = 1,
        .wDescStrPos = STR_IDX_3580_ModStr5f,
        .wDescStrNeg = STR_IDX_3580_ModStr5f,
        .nDescVal = 1,
    },
    {   // [18]
        .nStatId = STAT_ITEM_REANIMATE,
        .nDescFunc = 2,
        .wDescStrPos = STR_IDX_11037_ModStr6f,
        .wDescStrNeg = STR_IDX_11037_ModStr6f,
        .nDescVal = 1,
    },
    {   // [19]
        .nStatId = STAT_ITEM_POISONLENGTHRESIST,
        .nDescFunc = 2,
        .wDescStrPos = STR_IDX_3542_ModStr3r,
        .wDescStrNeg = STR_IDX_3542_ModStr3r,
        .nDescVal = 2,
    },
    { .nStatId = STAT_ITEM_HALFFREEZEDURATION, .nDescFunc = 3, .wDescStrPos = STR_IDX_3550_ModStr4b, .wDescStrNeg = STR_IDX_3550_ModStr4b },   // [20]
    { .nStatId = STAT_ITEM_CANNOTBEFROZEN, .nDescFunc = 3, .wDescStrPos = STR_IDX_3600_ModStr5z, .wDescStrNeg = STR_IDX_3600_ModStr5z },   // [21]
    {   // [22]
        .nStatId = STAT_MAGIC_DAMAGE_REDUCTION,
        .nDescFunc = 3,
        .wDescStrPos = STR_IDX_3518_ModStr2t,
        .wDescStrNeg = STR_IDX_3518_ModStr2t,
        .nDescVal = 2,
    },
    {   // [23]
        .nStatId = STAT_NORMAL_DAMAGE_REDUCTION,
        .nDescFunc = 3,
        .wDescStrPos = STR_IDX_3519_ModStr2u,
        .wDescStrNeg = STR_IDX_3519_ModStr2u,
        .nDescVal = 2,
    },
    {   // [24]
        .nStatId = STAT_ITEM_ABSORBCOLD_PERCENT,
        .nDescFunc = 2,
        .wDescStrPos = STR_IDX_3587_ModStr5m,
        .wDescStrNeg = STR_IDX_3587_ModStr5m,
        .nDescVal = 2,
    },
    {   // [25]
        .nStatId = STAT_ITEM_ABSORBLIGHT_PERCENT,
        .nDescFunc = 2,
        .wDescStrPos = STR_IDX_3583_ModStr5i,
        .wDescStrNeg = STR_IDX_3583_ModStr5i,
        .nDescVal = 2,
    },
    {   // [26]
        .nStatId = STAT_ITEM_ABSORBFIRE_PERCENT,
        .nDescFunc = 2,
        .wDescStrPos = STR_IDX_3581_ModStr5g,
        .wDescStrNeg = STR_IDX_3581_ModStr5g,
        .nDescVal = 2,
    },
    {   // [27]
        .nStatId = STAT_ITEM_ABSORBMAGIC_PERCENT,
        .nDescFunc = 2,
        .wDescStrPos = STR_IDX_3585_ModStr5k,
        .wDescStrNeg = STR_IDX_3585_ModStr5k,
        .nDescVal = 2,
    },
    {   // [28]
        .nStatId = STAT_ITEM_ABSORB_LTNG_PERLEVEL,
        .nDescFunc = 6,
        .wDescStrPos = STR_IDX_10046_ModStre9q,
        .wDescStrNeg = STR_IDX_10046_ModStre9q,
        .nDescVal = 1,
    },
    {   // [29]
        .nStatId = STAT_ITEM_ABSORBLIGHT,
        .nDescFunc = 1,
        .wDescStrPos = STR_IDX_3584_ModStr5j,
        .wDescStrNeg = STR_IDX_3584_ModStr5j,
        .nDescVal = 1,
    },
    {   // [30]
        .nStatId = STAT_ITEM_ABSORB_COLD_PERLEVEL,
        .nDescFunc = 6,
        .wDescStrPos = STR_IDX_3588_ModStr5n,
        .wDescStrNeg = STR_IDX_3588_ModStr5n,
        .nDescVal = 1,
    },
    {   // [31]
        .nStatId = STAT_ITEM_ABSORBCOLD,
        .nDescFunc = 1,
        .wDescStrPos = STR_IDX_3588_ModStr5n,
        .wDescStrNeg = STR_IDX_3588_ModStr5n,
        .nDescVal = 1,
    },
    {   // [32]
        .nStatId = STAT_ITEM_ABSORB_FIRE_PERLEVEL,
        .nDescFunc = 6,
        .wDescStrPos = STR_IDX_3582_ModStr5h,
        .wDescStrNeg = STR_IDX_3582_ModStr5h,
        .nDescVal = 1,
    },
    {   // [33]
        .nStatId = STAT_ITEM_ABSORBFIRE,
        .nDescFunc = 1,
        .wDescStrPos = STR_IDX_3582_ModStr5h,
        .wDescStrNeg = STR_IDX_3582_ModStr5h,
        .nDescVal = 1,
    },
    {   // [34]
        .nStatId = STAT_ITEM_ABSORBMAGIC,
        .nDescFunc = 1,
        .wDescStrPos = STR_IDX_3586_ModStr5l,
        .wDescStrNeg = STR_IDX_3586_ModStr5l,
        .nDescVal = 1,
    },
    {   // [35]
        .nStatId = STAT_ITEM_RESIST_POIS_PERLEVEL,
        .nDescFunc = 7,
        .wDescStrPos = STR_IDX_3486_ModStr1n,
        .wDescStrNeg = STR_IDX_3486_ModStr1n,
        .nDescVal = 2,
    },
    {   // [36]
        .nStatId = STAT_POISONRESIST,
        .nDescFunc = 4,
        .wDescStrPos = STR_IDX_3486_ModStr1n,
        .wDescStrNeg = STR_IDX_3486_ModStr1n,
        .nDescVal = 2,
    },
    {   // [37]
        .nStatId = STAT_ITEM_RESIST_LTNG_PERLEVEL,
        .nDescFunc = 7,
        .wDescStrPos = STR_IDX_3484_ModStr1l,
        .wDescStrNeg = STR_IDX_3484_ModStr1l,
        .nDescVal = 2,
    },
    {   // [38]
        .nStatId = STAT_LIGHTRESIST,
        .nDescFunc = 4,
        .wDescStrPos = STR_IDX_3484_ModStr1l,
        .wDescStrNeg = STR_IDX_3484_ModStr1l,
        .nDescVal = 2,
    },
    {   // [39]
        .nStatId = STAT_ITEM_RESIST_COLD_PERLEVEL,
        .nDescFunc = 7,
        .wDescStrPos = STR_IDX_3483_ModStr1k,
        .wDescStrNeg = STR_IDX_3483_ModStr1k,
        .nDescVal = 2,
    },
    {   // [40]
        .nStatId = STAT_COLDRESIST,
        .nDescFunc = 4,
        .wDescStrPos = STR_IDX_3483_ModStr1k,
        .wDescStrNeg = STR_IDX_3483_ModStr1k,
        .nDescVal = 2,
    },
    {   // [41]
        .nStatId = STAT_ITEM_RESIST_FIRE_PERLEVEL,
        .nDescFunc = 7,
        .wDescStrPos = STR_IDX_3482_ModStr1j,
        .wDescStrNeg = STR_IDX_3482_ModStr1j,
        .nDescVal = 2,
    },
    {   // [42]
        .nStatId = STAT_FIRERESIST,
        .nDescFunc = 4,
        .wDescStrPos = STR_IDX_3482_ModStr1j,
        .wDescStrNeg = STR_IDX_3482_ModStr1j,
        .nDescVal = 2,
    },
    {   // [43]
        .nStatId = STAT_MAGICRESIST,
        .nDescFunc = 4,
        .wDescStrPos = STR_IDX_3485_ModStr1m,
        .wDescStrNeg = STR_IDX_3485_ModStr1m,
        .nDescVal = 2,
    },
    {   // [44]
        .nStatId = STAT_DAMAGERESIST,
        .nDescFunc = 2,
        .wDescStrPos = STR_IDX_3519_ModStr2u,
        .wDescStrNeg = STR_IDX_3519_ModStr2u,
        .nDescVal = 2,
    },
    {   // [45]
        .nStatId = STAT_MAXPOISONRESIST,
        .nDescFunc = 4,
        .wDescStrPos = STR_IDX_3599_ModStr5y,
        .wDescStrNeg = STR_IDX_3599_ModStr5y,
        .nDescVal = 1,
    },
    {   // [46]
        .nStatId = STAT_MAXLIGHTRESIST,
        .nDescFunc = 4,
        .wDescStrPos = STR_IDX_3597_ModStr5w,
        .wDescStrNeg = STR_IDX_3597_ModStr5w,
        .nDescVal = 1,
    },
    {   // [47]
        .nStatId = STAT_MAXCOLDRESIST,
        .nDescFunc = 4,
        .wDescStrPos = STR_IDX_3596_ModStr5v,
        .wDescStrNeg = STR_IDX_3596_ModStr5v,
        .nDescVal = 1,
    },
    {   // [48]
        .nStatId = STAT_MAXFIRERESIST,
        .nDescFunc = 4,
        .wDescStrPos = STR_IDX_3595_ModStr5u,
        .wDescStrNeg = STR_IDX_3595_ModStr5u,
        .nDescVal = 1,
    },
    {   // [49]
        .nStatId = STAT_MAXMAGICRESIST,
        .nDescFunc = 4,
        .wDescStrPos = STR_IDX_3598_ModStr5x,
        .wDescStrNeg = STR_IDX_3598_ModStr5x,
        .nDescVal = 1,
    },
    {   // [50]
        .nStatId = STAT_ITEM_REGENSTAMINA_PERLEVEL,
        .nDescFunc = 7,
        .wDescStrPos = STR_IDX_3544_ModStr3v,
        .wDescStrNeg = STR_IDX_3544_ModStr3v,
        .nDescVal = 2,
    },
    {   // [51]
        .nStatId = STAT_STAMINARECOVERYBONUS,
        .nDescFunc = 2,
        .wDescStrPos = STR_IDX_3544_ModStr3v,
        .wDescStrNeg = STR_IDX_3544_ModStr3v,
        .nDescVal = 2,
    },
    {   // [52]
        .nStatId = STAT_ITEM_STAMINADRAINPCT,
        .nDescFunc = 2,
        .wDescStrPos = STR_IDX_10020_ModStr6e_SLOWERSTAMINADRAIN,
        .wDescStrNeg = STR_IDX_10020_ModStr6e_SLOWERSTAMINADRAIN,
        .nDescVal = 1,
    },
    {   // [53]
        .nStatId = STAT_ITEM_STAMINA_PERLEVEL,
        .nDescFunc = 6,
        .wDescStrPos = STR_IDX_10001_ModStr5d,
        .wDescStrNeg = STR_IDX_3578_ModStr5d,
        .nDescVal = 1,
    },
    {   // [54]
        .nStatId = STAT_MAXSTAMINA,
        .nDescFunc = 1,
        .wDescStrPos = STR_IDX_10001_ModStr5d,
        .wDescStrNeg = STR_IDX_3578_ModStr5d,
        .nDescVal = 1,
    },
    {   // [55]
        .nStatId = STAT_MANARECOVERYBONUS,
        .nDescFunc = 2,
        .wDescStrPos = STR_IDX_3555_ModStr4g,
        .wDescStrNeg = STR_IDX_3555_ModStr4g,
        .nDescVal = 2,
    },
    {   // [56]
        .nStatId = STAT_ITEM_MANA_PERLEVEL,
        .nDescFunc = 6,
        .wDescStrPos = STR_IDX_3477_ModStr1e,
        .wDescStrNeg = STR_IDX_3477_ModStr1e,
        .nDescVal = 1,
    },
    {   // [57]
        .nStatId = STAT_ITEM_MAXMANA_PERCENT,
        .nDescFunc = 2,
        .wDescStrPos = STR_IDX_3506_ModStr2h,
        .wDescStrNeg = STR_IDX_3506_ModStr2h,
        .nDescVal = 2,
    },
    { .nStatId = STAT_MAXMANA, .nDescFunc = 1, .wDescStrPos = STR_IDX_3477_ModStr1e, .wDescStrNeg = STR_IDX_3477_ModStr1e, .nDescVal = 1 },   // [58]
    { .nStatId = STAT_HPREGEN, .nDescFunc = 1, .wDescStrPos = STR_IDX_3510_ModStr2l, .wDescStrNeg = STR_IDX_3521_ModStr2w, .nDescVal = 2 },   // [59]
    {   // [60]
        .nStatId = STAT_ITEM_HP_PERLEVEL,
        .nDescFunc = 6,
        .wDescStrPos = STR_IDX_3493_ModStr1u,
        .wDescStrNeg = STR_IDX_3493_ModStr1u,
        .nDescVal = 1,
    },
    {   // [61]
        .nStatId = STAT_ITEM_MAXHP_PERCENT,
        .nDescFunc = 2,
        .wDescStrPos = STR_IDX_3505_ModStr2g,
        .wDescStrNeg = STR_IDX_3505_ModStr2g,
        .nDescVal = 2,
    },
    { .nStatId = STAT_MAXHP, .nDescFunc = 1, .wDescStrPos = STR_IDX_3493_ModStr1u, .wDescStrNeg = STR_IDX_3493_ModStr1u, .nDescVal = 1 },   // [62]
    {   // [63]
        .nStatId = STAT_ITEM_ENERGY_PERLEVEL,
        .nDescFunc = 6,
        .wDescStrPos = STR_IDX_3476_ModStr1d,
        .wDescStrNeg = STR_IDX_3476_ModStr1d,
        .nDescVal = 1,
    },
    { .nStatId = STAT_ENERGY, .nDescFunc = 1, .wDescStrPos = STR_IDX_3476_ModStr1d, .wDescStrNeg = STR_IDX_3476_ModStr1d, .nDescVal = 1 },   // [64]
    {   // [65]
        .nStatId = STAT_ITEM_VITALITY_PERLEVEL,
        .nDescFunc = 6,
        .wDescStrPos = STR_IDX_3475_ModStr1c,
        .wDescStrNeg = STR_IDX_3475_ModStr1c,
        .nDescVal = 1,
    },
    { .nStatId = STAT_VITALITY, .nDescFunc = 1, .wDescStrPos = STR_IDX_3475_ModStr1c, .wDescStrNeg = STR_IDX_3475_ModStr1c, .nDescVal = 1 },   // [66]
    {   // [67]
        .nStatId = STAT_ITEM_DEXTERITY_PERLEVEL,
        .nDescFunc = 6,
        .wDescStrPos = STR_IDX_3474_ModStr1b,
        .wDescStrNeg = STR_IDX_3474_ModStr1b,
        .nDescVal = 1,
    },
    {   // [68]
        .nStatId = STAT_DEXTERITY,
        .nDescFunc = 1,
        .wDescStrPos = STR_IDX_3474_ModStr1b,
        .wDescStrNeg = STR_IDX_3474_ModStr1b,
        .nDescVal = 1,
    },
    {   // [69]
        .nStatId = STAT_ITEM_STRENGTH_PERLEVEL,
        .nDescFunc = 6,
        .wDescStrPos = STR_IDX_3473_ModStr1a,
        .wDescStrNeg = STR_IDX_3473_ModStr1a,
        .nDescVal = 1,
    },
    { .nDescFunc = 1, .wDescStrPos = STR_IDX_3473_ModStr1a, .wDescStrNeg = STR_IDX_3473_ModStr1a, .nDescVal = 1 },   // [70]
    {   // [71]
        .nStatId = STAT_ARMORCLASS_VS_MISSILE,
        .nDescFunc = 1,
        .wDescStrPos = STR_IDX_3601_ModStr6a,
        .wDescStrNeg = STR_IDX_3601_ModStr6a,
        .nDescVal = 1,
    },
    {   // [72]
        .nStatId = STAT_ARMORCLASS_VS_HTH,
        .nDescFunc = 1,
        .wDescStrPos = STR_IDX_3602_ModStr6b,
        .wDescStrNeg = STR_IDX_3602_ModStr6b,
        .nDescVal = 1,
    },
    {   // [73]
        .nStatId = STAT_ARMORCLASS,
        .nDescFunc = 1,
        .wDescStrPos = STR_IDX_3481_ModStr1i,
        .wDescStrNeg = STR_IDX_3481_ModStr1i,
        .nDescVal = 1,
    },
    {   // [74]
        .nStatId = STAT_ITEM_ARMOR_PERLEVEL,
        .nDescFunc = 6,
        .wDescStrPos = STR_IDX_3481_ModStr1i,
        .wDescStrNeg = STR_IDX_3481_ModStr1i,
        .nDescVal = 1,
    },
    {   // [75]
        .nStatId = STAT_ITEM_ARMORPERCENT_PERLEVEL,
        .nDescFunc = 8,
        .wDescStrPos = STR_IDX_3520_Modstr2v,
        .wDescStrNeg = STR_IDX_3520_Modstr2v,
        .nDescVal = 1,
    },
    {   // [76]
        .nStatId = STAT_ITEM_ARMOR_PERCENT,
        .nDescFunc = 4,
        .wDescStrPos = STR_IDX_3520_Modstr2v,
        .wDescStrNeg = STR_IDX_3520_Modstr2v,
        .nDescVal = 1,
    },
    {   // [77]
        .nStatId = STAT_ITEM_DAMAGETARGETAC,
        .nDescFunc = 1,
        .wDescStrPos = STR_IDX_3552_ModStr4d,
        .wDescStrNeg = STR_IDX_3552_ModStr4d,
        .nDescVal = 1,
    },
    { .nStatId = STAT_ITEM_KNOCKBACK, .nDescFunc = 3, .wDescStrPos = STR_IDX_3497_ModStr1y, .wDescStrNeg = STR_IDX_3497_ModStr1y },   // [78]
    {   // [79]
        .nStatId = STAT_ITEM_SLOW,
        .nDescFunc = 2,
        .wDescStrPos = STR_IDX_11036_ModStr5r,
        .wDescStrNeg = STR_IDX_11036_ModStr5r,
        .nDescVal = 2,
    },
    {   // [80]
        .nStatId = STAT_ITEM_FREEZE,
        .nDescFunc = 12,
        .wDescStrPos = STR_IDX_3536_ModStr3l,
        .wDescStrNeg = STR_IDX_3536_ModStr3l,
        .nDescVal = 2,
    },
    {   // [81]
        .nStatId = STAT_ITEM_HOWL,
        .nDescFunc = 5,
        .wDescStrPos = STR_IDX_11034_ModStr3u,
        .wDescStrNeg = STR_IDX_11034_ModStr3u,
        .nDescVal = 2,
    },
    {   // [82]
        .nStatId = STAT_ITEM_STUPIDITY,
        .nDescFunc = 12,
        .wDescStrPos = STR_IDX_11038_ModStr6d,
        .wDescStrNeg = STR_IDX_11038_ModStr6d,
        .nDescVal = 2,
    },
    { .nStatId = STAT_ITEM_PREVENTHEAL, .nDescFunc = 3, .wDescStrPos = STR_IDX_3549_ModStr4a, .wDescStrNeg = STR_IDX_3549_ModStr4a },   // [83]
    {   // [84]
        .nStatId = STAT_ITEM_OPENWOUNDS_PERLEVEL,
        .nDescFunc = 6,
        .wDescStrPos = STR_IDX_3577_ModStr5c,
        .wDescStrNeg = STR_IDX_3577_ModStr5c,
        .nDescVal = 1,
    },
    {   // [85]
        .nStatId = STAT_ITEM_OPENWOUNDS,
        .nDescFunc = 2,
        .wDescStrPos = STR_IDX_3537_ModStr3m,
        .wDescStrNeg = STR_IDX_3537_ModStr3m,
        .nDescVal = 1,
    },
    {   // [86]
        .nStatId = STAT_ITEM_DEADLYSTRIKE_PERLEVEL,
        .nDescFunc = 7,
        .wDescStrPos = STR_IDX_3591_ModStr5q,
        .wDescStrNeg = STR_IDX_3591_ModStr5q,
        .nDescVal = 1,
    },
    {   // [87]
        .nStatId = STAT_ITEM_DEADLYSTRIKE,
        .nDescFunc = 2,
        .wDescStrPos = STR_IDX_3591_ModStr5q,
        .wDescStrNeg = STR_IDX_3591_ModStr5q,
        .nDescVal = 1,
    },
    {   // [88]
        .nStatId = STAT_ITEM_CRUSHINGBLOW_PERLEVEL,
        .nDescFunc = 6,
        .wDescStrPos = STR_IDX_3537_ModStr3m,
        .wDescStrNeg = STR_IDX_3537_ModStr3m,
        .nDescVal = 1,
    },
    {   // [89]
        .nStatId = STAT_ITEM_CRUSHINGBLOW,
        .nDescFunc = 2,
        .wDescStrPos = STR_IDX_3577_ModStr5c,
        .wDescStrNeg = STR_IDX_3577_ModStr5c,
        .nDescVal = 1,
    },
    {   // [90]
        .nStatId = STAT_LIFEDRAINMINDAM,
        .nDescFunc = 2,
        .wDescStrPos = STR_IDX_3524_ModStr2z,
        .wDescStrNeg = STR_IDX_3524_ModStr2z,
        .nDescVal = 1,
    },
    {   // [91]
        .nStatId = STAT_MANADRAINMINDAM,
        .nDescFunc = 2,
        .wDescStrPos = STR_IDX_3523_ModStr2y,
        .wDescStrNeg = STR_IDX_3523_ModStr2y,
        .nDescVal = 1,
    },
    {   // [92]
        .nStatId = STAT_ITEM_POIS_DAMAGEMAX_PERLEVEL,
        .nDescFunc = 6,
        .wDescStrPos = STR_IDX_3556_ModStr4h,
        .wDescStrNeg = STR_IDX_3556_ModStr4h,
        .nDescVal = 1,
    },
    {   // [93]
        .nStatId = STAT_POISONMAXDAM,
        .nDescFunc = 1,
        .wDescStrPos = STR_IDX_3556_ModStr4h,
        .wDescStrNeg = STR_IDX_3556_ModStr4h,
        .nDescVal = 1,
    },
    {   // [94]
        .nStatId = STAT_POISONMINDAM,
        .nDescFunc = 1,
        .wDescStrPos = STR_IDX_3557_ModStr4i,
        .wDescStrNeg = STR_IDX_3557_ModStr4i,
        .nDescVal = 1,
    },
    {   // [95]
        .nStatId = STAT_POISONLENGTH,
        .nDescFunc = 1,
        .wDescStrPos = STR_IDX_3557_ModStr4i,
        .wDescStrNeg = STR_IDX_3557_ModStr4i,
        .nDescVal = 1,
    },
    {   // [96]
        .nStatId = STAT_ITEM_COLD_DAMAGEMAX_PERLEVEL,
        .nDescFunc = 6,
        .wDescStrPos = STR_IDX_3491_ModStr1s,
        .wDescStrNeg = STR_IDX_3491_ModStr1s,
        .nDescVal = 1,
    },
    {   // [97]
        .nStatId = STAT_COLDMAXDAM,
        .nDescFunc = 1,
        .wDescStrPos = STR_IDX_3491_ModStr1s,
        .wDescStrNeg = STR_IDX_3491_ModStr1s,
        .nDescVal = 1,
    },
    {   // [98]
        .nStatId = STAT_COLDMINDAM,
        .nDescFunc = 1,
        .wDescStrPos = STR_IDX_3492_ModStr1t,
        .wDescStrNeg = STR_IDX_3492_ModStr1t,
        .nDescVal = 1,
    },
    {   // [99]
        .nStatId = STAT_ITEM_LTNG_DAMAGEMAX_PERLEVEL,
        .nDescFunc = 6,
        .wDescStrPos = STR_IDX_3489_ModStr1q,
        .wDescStrNeg = STR_IDX_3489_ModStr1q,
        .nDescVal = 1,
    },
    {   // [100]
        .nStatId = STAT_LIGHTMAXDAM,
        .nDescFunc = 1,
        .wDescStrPos = STR_IDX_3489_ModStr1q,
        .wDescStrNeg = STR_IDX_3489_ModStr1q,
        .nDescVal = 1,
    },
    {   // [101]
        .nStatId = STAT_LIGHTMINDAM,
        .nDescFunc = 1,
        .wDescStrPos = STR_IDX_3490_ModStr1r,
        .wDescStrNeg = STR_IDX_3490_ModStr1r,
        .nDescVal = 1,
    },
    {   // [102]
        .nStatId = STAT_ITEM_FIRE_DAMAGEMAX_PERLEVEL,
        .nDescFunc = 6,
        .wDescStrPos = STR_IDX_3487_ModStr1o,
        .wDescStrNeg = STR_IDX_3487_ModStr1o,
        .nDescVal = 1,
    },
    {   // [103]
        .nStatId = STAT_FIREMAXDAM,
        .nDescFunc = 1,
        .wDescStrPos = STR_IDX_3487_ModStr1o,
        .wDescStrNeg = STR_IDX_3487_ModStr1o,
        .nDescVal = 1,
    },
    {   // [104]
        .nStatId = STAT_FIREMINDAM,
        .nDescFunc = 1,
        .wDescStrPos = STR_IDX_3488_ModStr1p,
        .wDescStrNeg = STR_IDX_3488_ModStr1p,
        .nDescVal = 1,
    },
    {   // [105]
        .nStatId = STAT_MAGICMINDAM,
        .nDescFunc = 1,
        .wDescStrPos = STR_IDX_3618_strModMagicDamage,
        .wDescStrNeg = STR_IDX_3618_strModMagicDamage,
        .nDescVal = 1,
    },
    {   // [106]
        .nStatId = STAT_MAGICMAXDAM,
        .nDescFunc = 1,
        .wDescStrPos = STR_IDX_3618_strModMagicDamage,
        .wDescStrNeg = STR_IDX_3618_strModMagicDamage,
        .nDescVal = 1,
    },
    {   // [107]
        .nStatId = STAT_ITEM_TOHIT_UNDEAD_PERLEVEL,
        .nDescFunc = 6,
        .wDescStrPos = STR_IDX_3559_ModStr4k,
        .wDescStrNeg = STR_IDX_3559_ModStr4k,
        .nDescVal = 1,
    },
    {   // [108]
        .nStatId = STAT_ITEM_UNDEAD_TOHIT,
        .nDescFunc = 1,
        .wDescStrPos = STR_IDX_3559_ModStr4k,
        .wDescStrNeg = STR_IDX_3559_ModStr4k,
        .nDescVal = 1,
    },
    {   // [109]
        .nStatId = STAT_ITEM_DAMAGE_UNDEAD_PERLEVEL,
        .nDescFunc = 8,
        .wDescStrPos = STR_IDX_3554_ModStr4f,
        .wDescStrNeg = STR_IDX_3554_ModStr4f,
        .nDescVal = 1,
    },
    {   // [110]
        .nStatId = STAT_ITEM_UNDEADDAMAGE_PERCENT,
        .nDescFunc = 4,
        .wDescStrPos = STR_IDX_3554_ModStr4f,
        .wDescStrNeg = STR_IDX_3554_ModStr4f,
        .nDescVal = 1,
    },
    {   // [111]
        .nStatId = STAT_ITEM_TOHIT_DEMON_PERLEVEL,
        .nDescFunc = 6,
        .wDescStrPos = STR_IDX_3558_ModStr4j,
        .wDescStrNeg = STR_IDX_3558_ModStr4j,
        .nDescVal = 1,
    },
    {   // [112]
        .nStatId = STAT_ITEM_DEMON_TOHIT,
        .nDescFunc = 1,
        .wDescStrPos = STR_IDX_3558_ModStr4j,
        .wDescStrNeg = STR_IDX_3558_ModStr4j,
        .nDescVal = 1,
    },
    {   // [113]
        .nStatId = STAT_ITEM_DAMAGE_DEMON_PERLEVEL,
        .nDescFunc = 8,
        .wDescStrPos = STR_IDX_3553_ModStr4e,
        .wDescStrNeg = STR_IDX_3553_ModStr4e,
        .nDescVal = 1,
    },
    {   // [114]
        .nStatId = STAT_ITEM_DEMONDAMAGE_PERCENT,
        .nDescFunc = 4,
        .wDescStrPos = STR_IDX_3553_ModStr4e,
        .wDescStrNeg = STR_IDX_3553_ModStr4e,
        .nDescVal = 1,
    },
    {   // [115]
        .nStatId = STAT_ITEM_TOHIT_PERLEVEL,
        .nDescFunc = 6,
        .wDescStrPos = STR_IDX_3480_ModStr1h,
        .wDescStrNeg = STR_IDX_3480_ModStr1h,
        .nDescVal = 1,
    },
    { .nStatId = STAT_TOHIT, .nDescFunc = 1, .wDescStrPos = STR_IDX_3480_ModStr1h, .wDescStrNeg = STR_IDX_3480_ModStr1h, .nDescVal = 1 },   // [116]
    {   // [117]
        .nStatId = STAT_ITEM_TOHITPERCENT_PERLEVEL,
        .nDescFunc = 7,
        .wDescStrPos = STR_IDX_11035_ModStr4c,
        .wDescStrNeg = STR_IDX_11035_ModStr4c,
        .nDescVal = 1,
    },
    {   // [118]
        .nStatId = STAT_ITEM_TOHIT_PERCENT,
        .nDescFunc = 2,
        .wDescStrPos = STR_IDX_11035_ModStr4c,
        .wDescStrNeg = STR_IDX_11035_ModStr4c,
        .nDescVal = 1,
    },
    {   // [119]
        .nStatId = STAT_ITEM_FRACTIONALTARGETAC,
        .nDescFunc = 2,
        .wDescStrPos = STR_IDX_3589_ModStr5o,
        .wDescStrNeg = STR_IDX_3589_ModStr5o,
        .nDescVal = 1,
    },
    { .nStatId = STAT_ITEM_IGNORETARGETAC, .nDescFunc = 3, .wDescStrPos = STR_IDX_3547_ModStr3y, .wDescStrNeg = STR_IDX_3547_ModStr3y },   // [120]
    {   // [121]
        .nStatId = STAT_ITEM_KICK_DAMAGE_PERLEVEL,
        .nDescFunc = 6,
        .wDescStrPos = STR_IDX_3579_ModStr5e,
        .wDescStrNeg = STR_IDX_3579_ModStr5e,
        .nDescVal = 1,
    },
    {   // [122]
        .nStatId = STAT_ITEM_KICKDAMAGE,
        .nDescFunc = 1,
        .wDescStrPos = STR_IDX_3579_ModStr5e,
        .wDescStrNeg = STR_IDX_3579_ModStr5e,
        .nDescVal = 1,
    },
    {   // [123]
        .nStatId = STAT_ITEM_NORMALDAMAGE,
        .nDescFunc = 1,
        .wDescStrPos = STR_IDX_3576_ModStr5b,
        .wDescStrNeg = STR_IDX_3576_ModStr5b,
        .nDescVal = 2,
    },
    {   // [124]
        .nStatId = STAT_SECONDARY_MAXDAMAGE,
        .nDescFunc = 1,
        .wDescStrPos = STR_IDX_3478_ModStr1f,
        .wDescStrNeg = STR_IDX_3478_ModStr1f,
        .nDescVal = 1,
    },
    {   // [125]
        .nStatId = STAT_SECONDARY_MINDAMAGE,
        .nDescFunc = 1,
        .wDescStrPos = STR_IDX_3479_ModStr1g,
        .wDescStrNeg = STR_IDX_3479_ModStr1g,
        .nDescVal = 1,
    },
    {   // [126]
        .nStatId = STAT_ITEM_MAXDAMAGE_PERLEVEL,
        .nDescFunc = 6,
        .wDescStrPos = STR_IDX_3478_ModStr1f,
        .wDescStrNeg = STR_IDX_3478_ModStr1f,
        .nDescVal = 1,
    },
    {   // [127]
        .nStatId = STAT_MAXDAMAGE,
        .nDescFunc = 1,
        .wDescStrPos = STR_IDX_3478_ModStr1f,
        .wDescStrNeg = STR_IDX_3478_ModStr1f,
        .nDescVal = 1,
    },
    {   // [128]
        .nStatId = STAT_MINDAMAGE,
        .nDescFunc = 1,
        .wDescStrPos = STR_IDX_3479_ModStr1g,
        .wDescStrNeg = STR_IDX_3479_ModStr1g,
        .nDescVal = 1,
    },
    {   // [129]
        .nStatId = STAT_ITEM_MAXDAMAGE_PERCENT_PERLEVEL,
        .nDescFunc = 8,
        .wDescStrPos = STR_IDX_3508_ModStr2j,
        .wDescStrNeg = STR_IDX_3508_ModStr2j,
        .nDescVal = 1,
    },
    { .nStatId = STAT_ITEM_MAXDAMAGE_PERCENT, .nDescFunc = 3, .wDescStrPos = STR_IDX_3508_ModStr2j, .wDescStrNeg = STR_IDX_3508_ModStr2j },   // [130]
    { .nStatId = STAT_ITEM_MINDAMAGE_PERCENT, .nDescFunc = 3, .wDescStrPos = STR_IDX_3509_ModStr2k, .wDescStrNeg = STR_IDX_3509_ModStr2k },   // [131]
    { .nStatId = STAT_ITEM_MAGICARROW, .nDescFunc = 3, .wDescStrPos = STR_IDX_3608_ModStr6h, .wDescStrNeg = STR_IDX_3608_ModStr6h },   // [132]
    { .nStatId = STAT_ITEM_PIERCE, .nDescFunc = 3, .wDescStrPos = STR_IDX_3607_ModStr6g, .wDescStrNeg = STR_IDX_3607_ModStr6g },   // [133]
    { .nStatId = STAT_ITEM_EXPLOSIVEARROW, .nDescFunc = 3, .wDescStrPos = STR_IDX_3609_ModStr6i, .wDescStrNeg = STR_IDX_3609_ModStr6i },   // [134]
    { .nStatId = STAT_TOBLOCK, .nDescFunc = 2, .wDescStrPos = STR_IDX_3531_ModStr3g, .wDescStrNeg = STR_IDX_3531_ModStr3g, .nDescVal = 1 },   // [135]
    {   // [136]
        .nStatId = STAT_ITEM_FASTERBLOCKRATE,
        .nDescFunc = 2,
        .wDescStrPos = STR_IDX_3573_ModStr4y,
        .wDescStrNeg = STR_IDX_3573_ModStr4y,
        .nDescVal = 1,
    },
    {   // [137]
        .nStatId = STAT_ITEM_FASTERGETHITRATE,
        .nDescFunc = 2,
        .wDescStrPos = STR_IDX_3564_ModStr4p,
        .wDescStrNeg = STR_IDX_3564_ModStr4p,
        .nDescVal = 1,
    },
    {   // [138]
        .nStatId = STAT_ITEM_FASTERCASTRATE,
        .nDescFunc = 2,
        .wDescStrPos = STR_IDX_3570_ModStr4v,
        .wDescStrNeg = STR_IDX_3570_ModStr4v,
        .nDescVal = 1,
    },
    {   // [139]
        .nStatId = STAT_ITEM_FASTERATTACKRATE,
        .nDescFunc = 2,
        .wDescStrPos = STR_IDX_3561_ModStr4m,
        .wDescStrNeg = STR_IDX_3561_ModStr4m,
        .nDescVal = 1,
    },
    {   // [140]
        .nStatId = STAT_ITEM_FASTERMOVEVELOCITY,
        .nDescFunc = 2,
        .wDescStrPos = STR_IDX_3567_ModStr4s,
        .wDescStrNeg = STR_IDX_3567_ModStr4s,
        .nDescVal = 1,
    },
    {   // [141]
        .nStatId = STAT_ITEM_ELEMSKILL,
        .nDescFunc = 1,
        .wDescStrPos = STR_IDX_3533_ModStr3i,
        .wDescStrNeg = STR_IDX_3533_ModStr3i,
        .nDescVal = 1,
    },
    {   // [142]
        .nStatId = STAT_ITEM_ALLSKILLS,
        .nDescFunc = 1,
        .wDescStrPos = STR_IDX_3535_ModStr3k,
        .wDescStrNeg = STR_IDX_3535_ModStr3k,
        .nDescVal = 1,
    },
};
static_assert(sizeof(gptClientStatDescs) == 2288, "gptClientStatDescs: the item's 2288 bytes in the .i64");

//  D2Client 0x6FB7A438
//  The record count of gptClientStatDescs, read by MODS_BuildStatsText.
inline int32_t gnClientStatDescCount = 143;
static_assert(sizeof(gnClientStatDescCount) == 4, "gnClientStatDescCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FB7A440
//  The six elixir stat labels MODS_BuildStatsText walks, 16 bytes each; +4 is always 1 and +12 always 2. Count in gnElixirStatLabelCount.
inline D2ElixirStatLabelStrc gptElixirStatLabels[6] =
{
    { .nAlwaysOne = 1, .wStrPositive = 3498, .wStrNegative = 3498, .nAlwaysTwo = 2 },   // [0]
    { .nFileIndex = 1, .nAlwaysOne = 1, .wStrPositive = 3500, .wStrNegative = 3500, .nAlwaysTwo = 2 },   // [1]
    { .nFileIndex = 2, .nAlwaysOne = 1, .wStrPositive = 3499, .wStrNegative = 3499, .nAlwaysTwo = 2 },   // [2]
    { .nFileIndex = 3, .nAlwaysOne = 1, .wStrPositive = 3501, .wStrNegative = 3501, .nAlwaysTwo = 2 },   // [3]
    { .nFileIndex = 9, .nAlwaysOne = 1, .wStrPositive = 3502, .wStrNegative = 3502, .nAlwaysTwo = 2 },   // [4]
    { .nFileIndex = 7, .nAlwaysOne = 1, .wStrPositive = 3503, .wStrNegative = 3503, .nAlwaysTwo = 2 },   // [5]
};
static_assert(sizeof(gptElixirStatLabels) == 96, "gptElixirStatLabels: the item's 96 bytes in the .i64");

//  D2Client 0x6FB7A4A0
//  The record count of gptElixirStatLabels.
inline uint32_t gnElixirStatLabelCount = 6;
static_assert(sizeof(gnElixirStatLabelCount) == 4, "gnElixirStatLabelCount: the item's 4 bytes in the .i64");

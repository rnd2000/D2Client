//  MonUnique.h -- the globals of MonUnique.cpp that are ours: the boss-modifier handler table, the client hook table (43 rows of five columns),
//  the champion names, the order the fixed modifiers are applied in, the modifiers' label strings and the name separator.
//  Each is a C++17 inline variable, one object for every file that includes this header. The comment line "D2Client 0x6F..." above a global
//  gives its address in the original. gpMonUModHandlers, gpMonUModCltFuncs and gMonUModLabelStrings are writable (.data) and nothing writes
//  them; gaChampionUModNames and gnMonUModOrder are const (.rdata). A boss's modifier id indexes the handler, hook and label tables by an index
//  nothing bounds; past the end of the table our code finds no handler, no hook and no label, where the original reads the bytes after it (a
//  declared difference, noted in MonUnique.cpp). gwszNameSeparator lies past .data's raw data (it ends at 0x6FB9B000), so the loader
//  zero-fills it; the original constructs it twice with a static initialiser, ours is constructed by its declaration.
//  Include it after D2Client.h.
#pragma once

//  D2Client 0x6FB917A8
//  Unique-monster modifier handlers, indexed by the monumod.txt `id` column: 43, exactly the row count of stock monumod.txt, 18 of them with a
//  handler, every one a function of MonUnique.cpp. Called as f(pMonster, nModId, bUnique) by MONUNIQUE_ApplyModifiers; the third argument is the
//  monster's type flag 8. Entry 26 is MonTeleport, entry 28 doubles armour class (stoneskin), entries 1, 12, 16 and 36..39 build the monster
//  name. The dword after the 43, 0x6FB91854, is 0 and unreferenced.
inline void (__fastcall *gpMonUModHandlers[43])(D2UnitStrc *pMonster, D2C_MonUMods nModId, int32_t bUnique) =
{
    NULL,                               //   0  none
    MONUNIQUE_BuildMonsterNameText,     //   1  rndname
    NULL,                               //   2  hpmultiply
    MONUNIQUE_ModLight,                 //   3  light
    NULL,                               //   4  leveladd
    NULL,                               //   5
    NULL,                               //   6
    NULL,                               //   7
    MONUNIQUE_ModResists,               //   8  resist
    MONUNIQUE_ModResists,               //   9  fire
    NULL,                               //  10
    MONUNIQUE_ModDurielDead,            //  11  durieldead
    MONUNIQUE_BuildMonsterNameText,     //  12  bloodraven
    NULL,                               //  13
    NULL,                               //  14
    NULL,                               //  15
    MONUNIQUE_BuildMonsterNameText,     //  16  champion
    MONUNIQUE_ModResists,               //  17  lightning
    MONUNIQUE_ModResists,               //  18  cold
    NULL,                               //  19
    NULL,                               //  20
    NULL,                               //  21
    NULL,                               //  22
    MONUNIQUE_ModResists,               //  23  poisonhit
    NULL,                               //  24
    MONUNIQUE_ModResists,               //  25  manahit
    MONUNIQUE_ModTeleport,              //  26  teleport
    MONUNIQUE_ModResists,               //  27  spectralhit
    MONUNIQUE_ModResists,               //  28  stoneskin
    NULL,                               //  29
    NULL,                               //  30
    NULL,                               //  31
    NULL,                               //  32
    NULL,                               //  33
    NULL,                               //  34
    NULL,                               //  35
    MONUNIQUE_BuildMonsterNameText,     //  36  ghostly
    MONUNIQUE_BuildMonsterNameText,     //  37  fanatic
    MONUNIQUE_BuildMonsterNameText,     //  38  possessed
    MONUNIQUE_BuildMonsterNameText,     //  39  berserk
    NULL,                               //  40
    NULL,                               //  41
    NULL,                               //  42
};
static_assert(sizeof(gpMonUModHandlers) == 172, "gpMonUModHandlers: 43 slots, the item's 172 bytes");

//  D2Client 0x6FB91858
//  The client-side monumod.txt hooks: 43 rows x 5 columns of `void __fastcall(D2UnitStrc *, D2C_MonUMods nModId, int32_t bUnique)`, indexed
//  `[5 * nModId + nColumn]`; fifteen slots in fourteen rows are live, every one a function of MonUnique.cpp. Columns 0..3 get the monster in
//  ecx; column 4 gets a MISSILE and is reached from MISSILE_ClientSpawn. The dispatcher indexes it `[5 * id + column]` and, for column 4,
//  `+10h` past `[5 * id]`, and is the only reader. A 0 dword lies on each side of the table, 0x6FB91854 and 0x6FB91BB4, unreferenced.
inline void (__fastcall *gpMonUModCltFuncs[43][5])(D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique) =
{
    //  each row: column 0, 1, 2, 3, 4
    { NULL, NULL, NULL, NULL, NULL },                                           //   0  none
    { NULL, NULL, NULL, NULL, NULL },                                           //   1  rndname
    { NULL, NULL, NULL, NULL, NULL },                                           //   2  hpmultiply
    { NULL, NULL, NULL, NULL, NULL },                                           //   3  light
    { NULL, NULL, NULL, NULL, NULL },                                           //   4  leveladd
    { NULL, NULL, NULL, NULL, NULL },                                           //   5  strong
    { NULL, NULL, NULL, NULL, NULL },                                           //   6  fast
    { NULL, NULL, NULL, NULL, NULL },                                           //   7  curse
    { NULL, NULL, NULL, NULL, NULL },                                           //   8  resist
    { NULL, NULL, MONUNIQUE_MonUModCltFire, NULL, NULL },                                //   9  fire
    { NULL, NULL, MONUNIQUE_MonUModCltElementalDeath, NULL, NULL },                      //  10  poisondead
    { NULL, NULL, MONUNIQUE_MonUModCltDurielDead, NULL, NULL },                          //  11  durieldead
    { NULL, NULL, NULL, NULL, NULL },                                           //  12  bloodraven
    { NULL, NULL, NULL, NULL, NULL },                                           //  13  rage
    { NULL, NULL, NULL, NULL, NULL },                                           //  14  spcdamage
    { NULL, NULL, NULL, NULL, NULL },                                           //  15  partydead
    { NULL, NULL, NULL, NULL, NULL },                                           //  16  champion
    { NULL, NULL, MONUNIQUE_MonUModCltGetHitOverlay, MONUNIQUE_MonUModCltLightning, NULL },       //  17  lightning
    { NULL, NULL, MONUNIQUE_MonUModCltElementalDeath, NULL, NULL },                      //  18  cold
    { NULL, NULL, NULL, NULL, NULL },                                           //  19  hireable
    { NULL, NULL, MONUNIQUE_MonUModCltScarab, NULL, NULL },                              //  20  scarab
    { NULL, NULL, NULL, NULL, NULL },                                           //  21  killself
    { NULL, MONUNIQUE_MonUModCltQuestComplete, NULL, NULL, NULL },                       //  22  questcomplete
    { MONUNIQUE_MonUModCltGetHitOverlay, NULL, NULL, NULL, NULL },                       //  23  poisonhit
    { NULL, NULL, NULL, NULL, NULL },                                           //  24  thief
    { NULL, NULL, NULL, NULL, NULL },                                           //  25  manahit
    { NULL, NULL, NULL, NULL, NULL },                                           //  26  teleport
    { NULL, NULL, NULL, NULL, NULL },                                           //  27  spectralhit
    { NULL, NULL, NULL, NULL, NULL },                                           //  28  stoneskin
    { NULL, NULL, NULL, NULL, MONUNIQUE_MonUModCltMultishotSpawn },                      //  29  multishot
    { NULL, NULL, NULL, NULL, NULL },                                           //  30  aura
    { NULL, NULL, MONUNIQUE_MonUModCltGoBoom, NULL, NULL },                              //  31  goboom
    { NULL, NULL, MONUNIQUE_MonUModCltFireSpikeExplode, NULL, NULL },                    //  32  firespike_explode
    { NULL, NULL, MONUNIQUE_MonUModCltSuicideMinionExplode, NULL, NULL },                //  33  suicideminion_explode
    { NULL, NULL, NULL, NULL, NULL },                                           //  34  ai_after_death
    { NULL, NULL, NULL, NULL, NULL },                                           //  35  shatter_on_death
    { NULL, NULL, NULL, NULL, NULL },                                           //  36  ghostly
    { NULL, NULL, NULL, NULL, NULL },                                           //  37  fanatic
    { NULL, NULL, NULL, NULL, NULL },                                           //  38  possessed
    { NULL, NULL, NULL, NULL, NULL },                                           //  39  berserk
    { NULL, NULL, MONUNIQUE_MonUModCltWormsOnDeath, NULL, NULL },                        //  40  worms_on_death
    { NULL, NULL, NULL, NULL, NULL },                                           //  41  always_run_ai
    { NULL, NULL, MONUNIQUE_MonUModCltElementalDeath, NULL, NULL },                      //  42  lightningdeath
};
static_assert(sizeof(gpMonUModCltFuncs) == 860, "gpMonUModCltFuncs: 43 rows of five slots, the item's 860 bytes");

//  D2Client 0x6FB70CF0
//  A champion modifier's monumod.txt id and the tbl string its name begins with: five pairs, 16 champion and 36..39 ghostly, fanatic,
//  possessed, berserk, read by MONUNIQUE_BuildMonsterNameText (+0 and +4 of a pair). The dword after them, 0x6FB70D18, holds 5 and nothing
//  references it. The words above each tbl index are 0, the record's padding.
inline const D2ChampionUModNameStrc gaChampionUModNames[5] =
{
    { MONUMOD_CHAMPMODS,        STR_IDX_3220_Champion,          0 },     //  16 champion   "Champion"
    { MONUMOD_CHAMPSTONED,      STR_IDX_11084_champghostlyX,    0 },     //  36 ghostly    "Ghostly"
    { MONUMOD_CHAMPSTATS,       STR_IDX_11085_champfanaticX,    0 },     //  37 fanatic    "Fanatic"
    { MONUMOD_CHAMPCURSEIMMUNE, STR_IDX_11086_champpossessedX,  0 },     //  38 possessed  "Possessed"
    { MONUMOD_CHAMPSTATS2,      STR_IDX_11087_champberserkX,    0 },     //  39 berserk    "Berserker"
};
static_assert(sizeof(gaChampionUModNames) == 0x6FB70D18 - 0x6FB70CF0,
              "gaChampionUModNames: five 8-byte pairs, 0x6FB70CF0 up to the unreferenced 5");

//  D2Client 0x6FB70D1C
//  The ids MONUNIQUE_ApplyModifiers runs on every boss with a modifier before its own: four dwords, 1, 2, 3, 4 (rndname, hpmultiply, light,
//  leveladd), the walk's bound the address after them (0x6FB70D2C). Not a bitmask -- one dword per id. Slots 2 and 4, hpmultiply and leveladd,
//  are empty on the client, so the walk names the boss (1) and lights it (3).
inline const D2C_MonUMods gnMonUModOrder[4] =
{
    MONUMOD_RNDNAME, MONUMOD_HPMULTIPLY, MONUMOD_LIGHT, MONUMOD_LEVELADD,
};
static_assert(sizeof(gnMonUModOrder) == 0x6FB70D2C - 0x6FB70D1C,
              "gnMonUModOrder: four dwords, 0x6FB70D1C up to the walk's bound");

//  D2Client 0x6FB91BB8
//  Which tbl string names each monumod modifier on a monster's hover label -- D2C_StringIndices[44], indexed by the monumod.txt id, 0 for a
//  modifier with no visible name. Read by MONUNIQUE_MonUModBuildMonsterModifierLabel alone. Fourteen entries are populated, from monumod 5
//  `strong` -> 'Extra Strong' to 30 `aura` -> 'Aura Enchanted'. The tbl ids are not in row order: 17 `lightning` takes 3211 and 18 `cold`
//  takes 3210. 44 slots where monumod.txt has 43 rows; the 44th word, id 43, is 0, and whether the source said 44 the image cannot tell (43
//  words and two bytes of padding before the next string are the same bytes). Past the end ours finds no label (`if (nUMod >= 44) continue;`),
//  a declared difference.
inline D2C_StringIndices gMonUModLabelStrings[44] =
{
    {}, {}, {}, {}, {},                     //   0..4   none
    STR_IDX_3205_uniquextrastrong,          //   5  strong       MONUMOD_EXTRASTRONG     "Extra Strong"
    STR_IDX_3206_uniqueextrafast,           //   6  fast         MONUMOD_EXTRAFAST       "Extra Fast"
    STR_IDX_3207_uniquecursed,              //   7  curse        MONUMOD_CURSED          "Cursed"
    STR_IDX_3208_uniquemagicresistance,     //   8  resist       MONUMOD_MAGICRESISTANT  "Magic Resistant"
    STR_IDX_3209_uniquefireenchanted,       //   9  fire         MONUMOD_FIRECHANT       "Fire Enchanted"
    {}, {}, {}, {}, {}, {}, {},             //  10..16  none
    STR_IDX_3211_monsteruniqueprop2,        //  17  lightning    MONUMOD_LIGHTCHANT      "Lightning Enchanted"
    STR_IDX_3210_monsteruniqueprop1,        //  18  cold         MONUMOD_COLDCHANT       "Cold Enchanted"
    {}, {}, {}, {}, {},                     //  19..23  none
    STR_IDX_3217_monsteruniqueprop8,        //  24  thief        MONUMOD_THIEF           "Thief"
    STR_IDX_3212_monsteruniqueprop3,        //  25  manahit      MONUMOD_MANABURN        "Mana Burn"
    STR_IDX_3214_monsteruniqueprop5,        //  26  teleport     MONUMOD_TELEHEAL        "Teleportation"
    STR_IDX_3213_monsteruniqueprop4,        //  27  spectralhit  MONUMOD_SPECTRALHIT     "Spectral Hit"
    STR_IDX_3215_monsteruniqueprop6,        //  28  stoneskin    MONUMOD_STONESKIN       "Stone Skin"
    STR_IDX_3216_monsteruniqueprop7,        //  29  multishot    MONUMOD_MULTISHOT       "Multiple Shots"
    STR_IDX_3218_monsteruniqueprop9,        //  30  aura         MONUMOD_AURACHANT       "Aura Enchanted"
    {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {},     //  31..43  none
};
static_assert(sizeof(gMonUModLabelStrings) == 0x6FB91C10 - 0x6FB91BB8,
              "gMonUModLabelStrings: 44 words, 0x6FB91BB8 up to the __FILE__ string at 0x6FB91C10");

//  D2Client 0x6FBBC230
//  The single space that joins the parts of a monster's hover label -- a 2-character Unicode buffer, not a literal. The original constructs it
//  before DllMain returns, and MONUNIQUE_BuildMonsterNameText fills it by running Unicode::toUnicode over the ASCII " " (gszSpace), so it is
//  empty until that function has run once. It lies past .data's raw data (which ends at 0x6FB9B000), so the loader zero-fills it and our
//  declaration starts zeroed too.
inline Unicode gwszNameSeparator[2];
static_assert(sizeof(gwszNameSeparator) == 4, "gwszNameSeparator: the item's 4 bytes in the .i64");

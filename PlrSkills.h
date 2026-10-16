//  PlrSkills.h -- the globals of PlrSkills.cpp that are ours: the dispatch tables of a state's set and remove functions. They are writable in the
//  original's .data, so they are not const. Include it after D2Client.h, which declares every function named here.
#pragma once

//  D2Client 0x6FB93EE0
//  States.txt setfunc dispatch table: 32 slots, slot 0 null, 1..19 used; called as f(pUnit, nState) by PLRSKILLS_StatesRunStateSetFunc, which
//  reaches slots 0..30 (slot 31 is never read). Nothing writes it.
inline void (__fastcall *gpStateSetFuncs[32])(D2UnitStrc *, D2C_States) =
{
    NULL,                                       //   0
    STATES_SetFunc1_Progressive,                //   1
    STATES_SetFunc2_CloakOfShadows,             //   2
    STATES_SetFunc3_Blaze,                      //   3
    STATES_SetFunc4_SourceUnit,                 //   4
    STATES_SetFunc5_ChangeClass,                //   5
    STATES_SetFunc6_BoneArmor,                  //   6
    STATES_SetFunc7_Hurricane,                  //   7
    STATES_SetFunc8_CycloneArmor,               //   8
    STATES_SetFunc9_RemFunc6_ShrineSkill,       //   9
    STATES_SetFunc10_Redeemed,                  //  10
    STATES_SetFunc11_VineBeast,                 //  11
    STATES_SetFunc12_Invis,                     //  12
    STATES_SetFunc13_Attached,                  //  13
    STATES_SetFunc14_Shapeshift,                //  14
    STATES_SetFunc15_Revive,                    //  15
    STATES_SetFunc16_SkillCharges,              //  16
    STATES_SetFunc17_RestInPeace,               //  17
    STATES_SetFunc18_CorpseExp,                 //  18
    STATES_SetFunc19_RemFunc12_SkillDelay,      //  19
    NULL, NULL, NULL, NULL, NULL, NULL,         //  20..25
    NULL, NULL, NULL, NULL, NULL, NULL,         //  26..31
};

//  D2Client 0x6FB93F60
//  States.txt remfunc dispatch table: 30 slots, slot 0 null, 1..12 used (6 and 12 are shared with gpStateSetFuncs); called as f(pUnit, nState) by
//  PLRSKILLS_StatesRunStateRemFunc, which reaches all 30. Nothing writes it.
inline void (__fastcall *gpStateRemFuncs[30])(D2UnitStrc *, D2C_States) =
{
    NULL,                                       //   0
    STATES_RemFunc1_Progressive,                //   1
    STATES_RemFunc2_SourceUnit,                 //   2
    STATES_RemFunc3_BoneArmor,                  //   3
    STATES_RemFunc4_Hurricane,                  //   4
    STATES_RemFunc5_CycloneArmor,               //   5
    STATES_SetFunc9_RemFunc6_ShrineSkill,       //   6
    STATES_RemFunc7_Uninterruptable,            //   7
    STATES_RemFunc8_Invis,                      //   8
    STATES_RemFunc9_Attached,                   //   9
    STATES_RemFunc10_Pregnant,                  //  10
    STATES_RemFunc11_SkillCharges,              //  11
    STATES_SetFunc19_RemFunc12_SkillDelay,      //  12
    NULL, NULL, NULL, NULL, NULL, NULL,         //  13..18
    NULL, NULL, NULL, NULL, NULL, NULL,         //  19..24
    NULL, NULL, NULL, NULL, NULL,               //  25..29
};

static_assert(sizeof(gpStateSetFuncs) == 0x6FB93F60 - 0x6FB93EE0
                  && sizeof(gpStateRemFuncs) == 0x6FB93FD8 - 0x6FB93F60,
              "gpStateSetFuncs: 32 slots up to gpStateRemFuncs; gpStateRemFuncs: 30 slots up to the path string");

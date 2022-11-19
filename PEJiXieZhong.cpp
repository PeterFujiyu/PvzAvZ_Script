/*
 * @Author: Lucky_Ferry
 * @Date: 2022-11-08 18:47:23
 * @LastEditTime: 2022-11-09 18:19:18
 * @LastEditors: Lucky_Ferry
 * @Description:PVZ_[PE]机械钟无炮_Endless
 * @FilePath: \AVZscript\PEJiXieZhong.cpp
 */

#include "avz.h"

using namespace AvZ;
PlantFixer pumpkin_fixer;
PlantFixer fume_shroom_fixer;
PlantFixer lily_pad_fixer;
PlantFixer C_a_user;
PlantFixer gloom_shroom_user;
// 监测红眼
bool IsGGExist(int row, int abs, int hp)
{
    bool result = false;
    auto zombie = GetMainObject()->zombieArray();
    for (int index = 0; index < GetMainObject()->zombieTotal(); ++index) {
        if ((zombie[index].type() == GIGA_GARGANTUAR && zombie[index].isExist()) && !zombie[index].isDead() && zombie[index].row() == row - 1 && zombie[index].abscissa() < abs && zombie[index].hp() >= hp && zombie[index].state() != 70) {
            result = true;
        }
    }
    return result;
}
// 种植植物
void SmartCardNotInQueue(int type, int row, int col)
{
    auto seed = GetMainObject()->seedArray();
    // auto IceCoordinate = GetMainObject()->IceCoordinate();
    if (seed[type - 1].isUsable() && GetPlantIndex(row, col) == -1) {
        CardNotInQueue(type, row, col);
    }
}
// 用冰
void UseI(int wave)
{
    InsertTimeOperation(0, wave, [=]() {
        auto coffee_bean = GetMainObject()->seedArray() + GetSeedIndex(COFFEE_BEAN);
        if (GetPlantIndex(3, 4) >= 0 || GetPlantIndex(3, 3) >= 0 || GetPlantIndex(3, 2) >= 0) {
            if (coffee_bean->isUsable()) {
                SetTime(0, wave);
                ice_filler.coffee();
            } else if (coffee_bean->cd() >= 609) {
                SetTime(751 - coffee_bean->cd(), wave);
                ice_filler.coffee();
            }
        }
    });
}
// 智能核弹
void SmartN(int wave)
{
    InsertTimeOperation(-200, wave, [=]() {
        auto seed = GetMainObject()->seedArray();
        if ((seed[2].isUsable() || 5001 - seed[2].cd() < 401) && (seed[3].isUsable() || 751 - seed[3].cd() < 401)) {
            SetTime(-200, wave);
            fume_shroom_fixer.pause();
            SetTime(200, wave);
            Card(DOOM_SHROOM, {{2, 8}, {5, 7}, {2, 7}});
            Card(COFFEE_BEAN, {{2, 8}, {5, 7}, {2, 7}});
            fume_shroom_fixer.goOn();
        } else {
            UseI(wave);
        }
    });
}
// 用孪生灰烬
void AA(int time, int wave)
{
    InsertTimeOperation(0, wave, [=]() {
        auto seed = GetMainObject()->seedArray();
        if (seed[5].isUsable() || 5001 - seed[5].cd() <= time) {
            SetTime(time, wave);
            Card(CHERRY_BOMB, {{5, 9}, {5, 8}});
            Shovel(2, 7);
            Card(JALAPENO, {{2, 9}, {2, 8}, {2, 7}, {2, 3}});
            InsertTimeOperation(time + 1, wave, [=]() {
                auto jaiapeno = GetMainObject()->seedArray() + GetSeedIndex(JALAPENO);
                if (jaiapeno->isUsable()) {
                    ShovelNotInQueue(2, 3);
                    CardNotInQueue(6, 2, 3);
                }
            });
        } else {
            UseI(wave);
        }
    });
}
// 智能寒冰菇
void SmartI(int wave)
{
    InsertTimeOperation(0, wave, [=]() {
        if (IsZombieExist(32) || IsZombieExist(12, 2) || IsZombieExist(15, 5) || IsZombieExist(7, 2) || IsZombieExist(7, 5)) {
            UseI(wave);
        } else if (IsZombieExist(16) && (wave == 13 || wave == 15 || wave == 17 || wave == 19)) {
            UseI(wave);
        }
    });
}
// 铲除曾哥
void RemoveGloomShroom(int wave, int row, int col)
{
    InsertTimeOperation(1000, wave, [=]() {
        auto plant = GetMainObject()->plantArray();
        auto gloom_shroom = GetMainObject()->seedArray() + GetSeedIndex(GLOOM_SHROOM);
        if (gloom_shroom->isUsable()) {
            for (int index = 0; index < GetMainObject()->plantTotal(); ++index) {
                ShovelNotInQueue(row, col);
                break;
            }
        }
    });
}
// 垫材
void SmartC_a()
{
    if (IsGGExist(1, 520, 0)) {
        SmartCardNotInQueue(9, 1, 6);
    }
    if (IsGGExist(6, 560, 0)) {
        SmartCardNotInQueue(9, 6, 6);
        SmartCardNotInQueue(9, 6, 7);
    }
    if (IsGGExist(1, 520, 1000)) {
        SmartCardNotInQueue(7, 1, 6);
    }
    if (IsGGExist(6, 520, 1000)) {
        SmartCardNotInQueue(7, 6, 6);
    }
    if (IsGGExist(2, 560, 1000)) {
        SmartCardNotInQueue(10, 2, 7);
    }
    if (IsGGExist(6, 500, 500)) {
        SmartCardNotInQueue(10, 6, 6);
    }
}
void PlantGloomShroom(int row, int col)
{
    auto gloom_shroom = GetMainObject()->seedArray() + GetSeedIndex(GLOOM_SHROOM);
    if (gloom_shroom->isUsable() && GetPlantIndex(row, col, 10) > -1) {
        CardNotInQueue(7, row, col);
    }
}
// 补曾哥（全局）
void FixGloomShroom()
{
    PlantGloomShroom(3, 9);
    PlantGloomShroom(4, 9);
    PlantGloomShroom(3, 8);
    PlantGloomShroom(4, 8);
    PlantGloomShroom(3, 7);
    PlantGloomShroom(4, 7);
    PlantGloomShroom(2, 6);
    PlantGloomShroom(5, 6);
}
// 主要代码
void Script()
{
    // 倍速
    SetGameSpeed(2.5);
    // 准备结束脚本
    OpenMultipleEffective('X', MAIN_UI_OR_FIGHT_UI);
    // 获得出怪
    auto zombie_type = GetMainObject()->zombieTypeList();
    // 准备工作
    pumpkin_fixer.start(PUMPKIN, {{3, 9}, {3, 8}, {4, 9}, {4, 8}, {1, 1}, {1, 4}, {2, 1}, {2, 2}, {2, 4}, {3, 6}, {3, 7}, {4, 6}, {4, 7}, {5, 1}, {5, 4}, {6, 1}, {6, 4}, {2, 3}}, 2000);
    ice_filler.start({{3, 2}, {3, 3}, {3, 4}, {4, 3}, {2, 3}});
    // 开局核弹
    SetTime(0, 1);
    Card({{DOOM_SHROOM, 2, 9}, {COFFEE_BEAN, 2, 9}});
    // 补大喷
    SetTime(1, 3);
    fume_shroom_fixer.start(FUME_SHROOM, {{3, 9}, {4, 9}, {3, 8}, {4, 8}, {3, 7}, {4, 7}, {2, 6}, {5, 6}, {1, 5}, {6, 5}, {1, 4}, {6, 4}}, 0);
    fume_shroom_fixer.isUseCoffee(true);
    // 中场点核
    SmartN(11);
    // 旗帜波次冰杀小偷（控丑）
    for (auto wave : {10, 20}) {
        SetTime(-654, wave);
        fume_shroom_fixer.pause();
        SetTime(97);
        ice_filler.coffee();
        fume_shroom_fixer.goOn();
    }
    // 针对特定僵尸
    if (zombie_type[GIGA_GARGANTUAR]) {
        SelectCards({ICE_SHROOM, M_ICE_SHROOM, COFFEE_BEAN, DOOM_SHROOM, CHERRY_BOMB, JALAPENO, SQUASH, PUMPKIN, PUFF_SHROOM, FUME_SHROOM});
        C_a_user.pushFunc(SmartC_a);
        for (auto wave : {2, 6, 12, 18}) {
            AA(400, wave);
        }
        AA(2300, 9);
        AA(2300, 20);
        if (zombie_type[GARGANTUAR]) {
            for (auto wave : {5, 17}) {
                SmartN(wave);
            }
            for (auto wave : {3, 4, 8, 9, 13, 14, 15, 16, 19}) {
                UseI(wave);
            }
        } else {
            for (auto wave : {3, 4, 8, 9, 13, 14, 15, 16, 17, 19}) {
                SmartI(wave);
            }
        }
    } else {
        SelectCards({ICE_SHROOM, M_ICE_SHROOM, COFFEE_BEAN, DOOM_SHROOM, CHERRY_BOMB, JALAPENO, GLOOM_SHROOM, PUMPKIN, LILY_PAD, FUME_SHROOM});
        gloom_shroom_user.pushFunc(FixGloomShroom);
        SetTime(-599, 1);
        lily_pad_fixer.start(LILY_PAD, {{3, 9}, {4, 9}, {3, 8}, {3, 9}, {3, 7}, {4, 7}}, 0);
        for (auto wave : {2, 8, 12, 19}) {
            AA(400, wave);
        }
        //
        // for (auto wave : {9, 19}) {
        //     RemoveGloomShroom(wave, 2, 6);
        //     RemoveGloomShroom(wave, 5, 6);
        // }
    }
    if (zombie_type[GIGA_GARGANTUAR] || zombie_type[JACK_IN_THE_BOX_ZOMBIE] || zombie_type[FOOTBALL_ZOMBIE]) {

        if (zombie_type[GARGANTUAR]) {
            ;
        } else {
            for (auto wave : {7, 18}) {
                SmartN(wave);
            }
        }
        for (auto wave : {3, 4, 6, 9, 13, 14, 15, 16}) {
            SmartI(wave);
        }
    } else if (zombie_type[GARGANTUAR] || zombie_type[BALLOON_ZOMBIE]) {
        for (auto wave : {3, 7, 9, 13, 15, 18}) {
            UseI(wave);
        }
    }
}

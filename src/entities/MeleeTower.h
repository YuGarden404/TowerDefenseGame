//
// Created by Lenovo on 26-1-12.
//

#ifndef MELEETOWER_H
#define MELEETOWER_H

#include "Tower.h"
#include "../components/Affix.h"
#include "../components/BurnAffix.h"
#include "../components/SlowAffix.h"

class MeleeTower : public Tower {
    //成员变量
    int blockLimit; //最大阻挡人数
    std::vector<Enemy *>blockedEnemies; //当前正在被此塔阻挡的敌人列表
public:
    //成员函数
    //构造函数
    MeleeTower(float x, float y, int hp, int maxHp, int attackPower, float attackCooldown, float attackRange, float speed, int blockLimit);
    //实现具体的近战伤害逻辑
    void attack(Enemy* target) override;
    //实现具体的近战塔更新逻辑
    void update(float deltaTime, std::vector<Entity*>& entities) override;
    //析构函数
    ~MeleeTower() override;
};



#endif //MELEETOWER_H

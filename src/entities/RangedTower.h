//
// Created by Lenovo on 26-1-12.
//

#ifndef RANGEDTOWER_H
#define RANGEDTOWER_H

#include "Tower.h"

class RangedTower : public Tower {
public:
    //公共成员函数
    RangedTower();
    //含参构造函数
    RangedTower(int attackPower, float attackRange, float attackCooldown, float x, float y, int hp, int maxHp, float speed = 0.0);
    //纯虚函数 远程塔的攻击表现不同，留给子类实现
    void attack(Enemy* target) override;
    void update(float deltaTime, std::vector<Entity*>& entities) override;
};



#endif //RANGEDTOWER_H

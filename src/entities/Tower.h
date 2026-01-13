//
// Created by Lenovo on 26-1-12.
//

#ifndef TOWER_H
#define TOWER_H

#include "Enemy.h"
#include "Entity.h"

class Tower : public Entity {
protected:
    //成员变量
    int attackPower;        //攻击力
    float attackRange;      //攻击范围
    float attackCooldown;   //攻击冷却时间（秒）
    float lastAttackTimer;  //距离上次攻击已过去的时间
public:
    //公共成员函数
    //含参构造函数
    Tower(int attackPower, float attackRange, float attackCooldown, float x, float y, int hp, int maxHp, float speed = 0.0);
    //纯虚函数 近战塔和远程塔的攻击表现不同，留给子类实现
    virtual void attack(Enemy* target) = 0;
    //纯虚函数 每帧都要执行逻辑
    void update(float deltaTime, std::vector<Entity*>& entities) override;
};



#endif //TOWER_H

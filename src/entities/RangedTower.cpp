//
// Created by Lenovo on 26-1-12.
//

#include "RangedTower.h"

RangedTower::RangedTower()
:Tower(10,10,0.5f,0,0,10,10){}

RangedTower::RangedTower(const int attackPower, const float attackRange, const float attackCooldown, const float x, float y, int hp, int maxHp, float speed)
: Tower(attackPower,attackRange,attackCooldown,x,y,hp,maxHp,speed){}

void RangedTower::attack(Enemy* target)
{
    if(!target||target->isDead()||isDead()||lastAttackTimer<attackCooldown)return;
    if(euclideanDistance(getX(),getY(),target->getX(),target->getY())<=attackRange)
    {
        std::cout << "远程塔攻击(" << target->getX() << "," << target->getY() << ")成功，造成" << attackPower << "点伤害" << std::endl;
        target->takeDamage(attackPower);
    }
}

void RangedTower::update(const float deltaTime, std::vector<Entity*>& entities)
{
    Tower::update(deltaTime,entities);
    if(lastAttackTimer>=attackCooldown)
    {
        Entity* target = nullptr;
        float minDistance = attackRange;
        for(const auto& entity:entities)
        {
            if(!entity||entity->isDead())continue;
            auto* enemy = dynamic_cast<Enemy*>(entity);
            if(!enemy)continue;
            if(const float distance = euclideanDistance(getX(),getY(),enemy->getX(),enemy->getY()); distance<=minDistance)
            {
                target = enemy;
                minDistance = distance;
            }
        }
        if(target)
        {
            attack(dynamic_cast<Enemy*>(target));
            lastAttackTimer = 0.0f;
        }
    }
}
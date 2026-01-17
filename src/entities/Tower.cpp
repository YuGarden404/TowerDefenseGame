//
// Created by Lenovo on 26-1-12.
//

#include "Tower.h"

Tower::Tower(const int attackPower, const float attackRange, const float attackCooldown, const float x, const float y, const int hp, const int maxHp, const float speed)
:Entity(x,y,hp,maxHp,speed),attackPower(attackPower),attackRange(attackRange),attackCooldown(attackCooldown),lastAttackTimer(0)
{
    onHitEffects.clear();
}

void Tower::update(const float deltaTime, std::vector<Entity*>& entities)
{
    if(isDead())return;
    lastAttackTimer += deltaTime;
}
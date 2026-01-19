//
// Created by Lenovo on 26-1-12.
//

#include "MeleeTower.h"

#include <algorithm>

MeleeTower::MeleeTower(const float x, const float y, const int hp, const int maxHp, const int attackPower, const float attackCooldown, const float attackRange, float speed, const int blockLimit)
:Tower(attackPower, attackRange, attackCooldown, x, y, hp, maxHp),blockLimit(blockLimit)
{
    blockedEnemies.clear();
}

void MeleeTower::attack(Enemy* target)
{
    if (!target || target->isDead()) return;
    std::cout << "近战塔在(" << getX() << "," << getY() << ")攻击敌人，造成" << attackPower << "点伤害" << std::endl;
    target->takeDamage(attackPower);
    for(auto& effect : onHitEffects)
    {
        if(effect == "Slow")
        {
            target->addAffix(std::make_unique<SlowAffix>(0.5f, "Slow", 0.5f));
        }else if(effect == "Burn")
        {
            target->addAffix(std::make_unique<BurnAffix>(0.5f, "Burn", 5));
        }
    }
}

void MeleeTower::update(float deltaTime, std::vector<Entity*>& entities)
{
    if(isDead())return;
    lastAttackTimer += deltaTime;
    std::erase_if(blockedEnemies,[](Enemy* enemy)
    {
        if(!enemy||enemy->isDead())
        {
            if(enemy) enemy->setBlocked(false);
            return true;
        }
        return false;
    });
    for(const auto& entity : entities)
    {
        if(!entity||entity->isDead())continue;
        if(auto* enemy = dynamic_cast<Enemy*>(entity))
        {
            if(!enemy->getBlocked()&&euclideanDistance(getX(), getY(), enemy->getX(), enemy->getY()) <= attackRange && blockedEnemies.size() < blockLimit)
            {
                if(std::ranges::find(blockedEnemies,enemy) == blockedEnemies.end())
                {
                    blockedEnemies.push_back(enemy);
                    enemy->setBlocked(true);
                }
            }
        }
    }
    if(lastAttackTimer>=attackCooldown&&!blockedEnemies.empty())
    {
        Enemy* target = nullptr;
        int minHp = INT_MAX;
        for(const auto& enemy : blockedEnemies)
        {
            if(!enemy||enemy->isDead())continue;
            if(enemy->getHp() < minHp)
            {
                target = enemy;
                minHp = enemy->getHp();
            }
        }
        if(target)
        {
            attack(target);
            lastAttackTimer = 0.0f;
        }
    }
}

MeleeTower::~MeleeTower()
{
    for(const auto& enemy : blockedEnemies)
    {
        if(enemy)
        {
            enemy->setBlocked(false);
        }
    }
}

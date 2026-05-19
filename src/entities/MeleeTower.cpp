//
// Created by Lenovo on 26-1-12.
//

#include "MeleeTower.h"

#include <algorithm>
#include <climits>

MeleeTower::MeleeTower(const float x, const float y, const int hp, const int maxHp, const int attackPower, const float attackCooldown, const float attackRange, float speed, const int blockLimit)
    : Tower(attackPower, attackRange, attackCooldown, x, y, hp, maxHp, speed), blockLimit(blockLimit)
{
    blockedEnemies.clear();
}

MeleeTower::~MeleeTower()
{
    for (auto &weakEnemy : blockedEnemies)
    {
        if (auto enemy = weakEnemy.lock())
        {
            enemy->setBlocked(false);
            enemy->clearBlockedBy();
        }
    }
}

void MeleeTower::attack(Enemy *target)
{
    if (!target || target->isDead() || isDead())
        return;
    int finalAttackPower = attackPower;
    if (hasEquippedAffix(AffixId::Berserk))
    {
        finalAttackPower *= 2;
    }
    target->takeDamage(finalAttackPower);
}

void MeleeTower::update(const float deltaTime, std::vector<std::shared_ptr<Entity>> &entities)
{
    if (isDead())
        return;

    updateAffixes(deltaTime);
    lastAttackTimer += deltaTime;

    std::shared_ptr<Entity> self = nullptr;
    for (const auto &entity : entities)
    {
        if (entity.get() == this)
        {
            self = entity;
            break;
        }
    }
    if (!self)
        return;
    blockedEnemies.erase(
        std::remove_if(blockedEnemies.begin(), blockedEnemies.end(),
                       [this](const std::weak_ptr<Enemy> &weakEnemy)
                       {
                           auto enemy = weakEnemy.lock();

                           if (!enemy)
                           {
                               return true;
                           }

                           if (enemy->isDead() ||
                               euclideanDistance(getX(), getY(), enemy->getX(), enemy->getY()) > attackRange)
                           {
                               enemy->setBlocked(false);
                               enemy->clearBlockedBy();
                               return true;
                           }

                           return false;
                       }),
        blockedEnemies.end());
    for (const auto &entity : entities)
    {
        if (auto enemy = std::dynamic_pointer_cast<Enemy>(entity))
        {
            if (!enemy->isDead() && !enemy->getBlocked() &&
                euclideanDistance(getX(), getY(), enemy->getX(), enemy->getY()) <= attackRange &&
                blockedEnemies.size() < blockLimit)
            {
                if (enemy->canBlink())
                {
                    enemy->triggerBlink();
                    continue;
                }

                blockedEnemies.push_back(enemy);
                enemy->setBlocked(true);
                enemy->setBlockedBy(self);
            }
        }
    }
    float finalCooldown = attackCooldown;
    if (hasEquippedAffix(AffixId::Berserk))
    {
        finalCooldown *= 0.5f;
    }

    if (lastAttackTimer >= finalCooldown && !blockedEnemies.empty())
    {
        std::shared_ptr<Enemy> target = nullptr;
        int minHp = INT_MAX;

        for (auto &weakEnemy : blockedEnemies)
        {
            if (auto enemy = weakEnemy.lock())
            { // 必须先 lock 才能访问数据
                if (enemy->getHp() < minHp)
                {
                    target = enemy;
                    minHp = enemy->getHp();
                }
            }
        }

        if (target)
        {
            attack(target.get());
            lastAttackTimer = 0.0f;
        }
    }
}

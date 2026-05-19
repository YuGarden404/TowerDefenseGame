//
// Created by Lenovo on 26-1-12.
//

#include "RangedTower.h"
#include "../components/SlowAffix.h"
#include "../components/BurnAffix.h"

RangedTower::RangedTower()
    : Tower(10, 10, 0.5f, 0, 0, 10, 10) {}

RangedTower::RangedTower(const int attackPower, const float attackRange, const float attackCooldown, const float x, float y, int hp, int maxHp, float speed)
    : Tower(attackPower, attackRange, attackCooldown, x, y, hp, maxHp, speed) {}

void RangedTower::attack(Enemy *target)
{
    if (!target || target->isDead() || isDead() || lastAttackTimer < attackCooldown)
        return;
    if (euclideanDistance(getX(), getY(), target->getX(), target->getY()) > attackRange)
    {
        return;
    }
    target->takeDamage(attackPower);

    if (hasEquippedAffix(AffixId::Slow))
    {
        target->addAffix(std::make_unique<SlowAffix>(1.5f, "Slow", 0.5f));
    }

    if (hasEquippedAffix(AffixId::Burn))
    {
        target->addAffix(std::make_unique<BurnAffix>(3.0f, "Burn", 5));
    }
}

void RangedTower::update(const float deltaTime, std::vector<std::shared_ptr<Entity>> &entities)
{
    if (isDead())
        return;
    this->updateAffixes(deltaTime);
    lastAttackTimer += deltaTime;
    if (lastAttackTimer >= attackCooldown)
    {
        std::shared_ptr<Enemy> target = nullptr;
        float minDistance = attackRange;
        for (const auto &entity : entities)
        {
            if (!entity || entity->isDead())
                continue;
            auto enemy = std::dynamic_pointer_cast<Enemy>(entity);
            if (!enemy)
                continue;
            if (const float distance = euclideanDistance(getX(), getY(), enemy->getX(), enemy->getY()); distance <= minDistance)
            {
                target = enemy;
                minDistance = distance;
            }
        }
        if (target)
        {
            attack(target.get());
            lastAttackTimer = 0.0f;
        }
    }
}
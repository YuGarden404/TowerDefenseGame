//
// Created by Lenovo on 26-1-12.
//

#include "Enemy.h"

Enemy::Enemy(Map &map, const size_t pathId, const float x, const float y,
             const int hp, const int maxHp, const float speed, const int reward)
    : Entity(x, y, hp, maxHp, speed),
      map(map),
      pathId(pathId),
      pathIndex(0),
      reward(reward),
      isBlocked(false),
      reachedEnd(false),
      attackPower(10),
      attackCooldown(1.0f),
      attackTimer(0.0f),
      innateAffixesApplied(false),
      blinkCooldown(3.0f),
      blinkTimer(0.0f)
{
}

void Enemy::update(const float deltaTime, std::vector<std::shared_ptr<Entity>> &entities)
{
    (void)entities;

    if (isDead() || reachedEnd)
    {
        return;
    }

    const auto &path = map.getEnemyPath(pathId);
    if (path.empty() || pathIndex >= path.size())
    {
        return;
    }

    updateAffixes(deltaTime);
    updateBlinkCooldown(deltaTime);

    if (isBlocked)
    {
        attackTimer += deltaTime;

        auto blocker = blockedBy.lock();
        if (!blocker || blocker->isDead())
        {
            isBlocked = false;
            clearBlockedBy();
            return;
        }

        if (attackTimer >= attackCooldown)
        {
            blocker->takeDamage(attackPower);
            attackTimer = 0.0f;
        }
        return;
    }

    const Point &target = path[pathIndex];
    const auto targetX = static_cast<float>(target.x);
    const auto targetY = static_cast<float>(target.y);
    const float dx = targetX - getX();
    const float dy = targetY - getY();

    if (const float distance = std::sqrt(dx * dx + dy * dy); distance > 0.1f)
    {
        x += (dx / distance) * speed * deltaTime;
        y += (dy / distance) * speed * deltaTime;
    }
    else
    {
        pathIndex++;
        if (pathIndex >= path.size())
        {
            reachedEnd = true;
            takeDamage(hp);
        }
    }
}

int Enemy::getReward() const
{
    return reward;
}

void Enemy::setBlockedBy(std::shared_ptr<Entity> blocker)
{
    blockedBy = blocker;
}

void Enemy::clearBlockedBy()
{
    blockedBy.reset();
}

void Enemy::applyInnateAffixes()
{
    if (innateAffixesApplied)
    {
        return;
    }
    if (hasEquippedAffix(AffixId::Swift))
    {
        setSpeed(getSpeed() * 1.5f);
    }
    innateAffixesApplied = true;
}

void Enemy::updateBlinkCooldown(float deltaTime)
{
    if (blinkTimer > 0.0f)
    {
        blinkTimer -= deltaTime;
    }
}

bool Enemy::canBlink() const
{
    return hasEquippedAffix(AffixId::Blink) && blinkTimer <= 0.0f && !isDead() && !reachedEnd;
}

void Enemy::triggerBlink()
{
    if (canBlink())
    {
        blinkTimer = blinkCooldown;
        blinkForward();
    }
}

void Enemy::blinkForward()
{
    const auto &path = map.getEnemyPath(pathId);
    if (path.empty())
    {
        return;
    }

    if (pathIndex >= path.size())
    {
        return;
    }

    size_t newIndex = pathIndex + 2;
    if (newIndex >= path.size())
    {
        newIndex = path.size() - 1;
    }
    pathIndex = newIndex;
    x = static_cast<float>(path[pathIndex].x);
    y = static_cast<float>(path[pathIndex].y);
}
#include "EnemyFactory.h"

#include <algorithm>

EnemyFactory::EnemyFactory()
    : rng(std::random_device{}()),
      swiftChance(0.2f),
      blinkChance(0.1f)
{
}

std::unique_ptr<Enemy> EnemyFactory::createEnemy(Map &map)
{
    if (map.getPathCount() == 0)
    {
        return nullptr;
    }

    std::uniform_int_distribution<size_t> dist(0, map.getPathCount() - 1);
    return createEnemy(map, dist(rng));
}

std::unique_ptr<Enemy> EnemyFactory::createEnemy(Map &map, const size_t pathId)
{
    return createEnemy(map, pathId, 50, 50, 1.0f, 100);
}

std::unique_ptr<Enemy> EnemyFactory::createEnemy(
    Map &map,
    const size_t pathId,
    const int hp,
    const int maxHp,
    const float speed,
    const int reward)
{
    const auto &path = map.getEnemyPath(pathId);
    if (path.empty())
    {
        return nullptr;
    }

    const Point &start = path.front();

    auto enemy = std::make_unique<Enemy>(
        map,
        pathId,
        static_cast<float>(start.x),
        static_cast<float>(start.y),
        hp,
        maxHp,
        speed,
        reward);

    assignRandomAffixes(*enemy);
    enemy->applyInnateAffixes();
    return enemy;
}

void EnemyFactory::setSwiftChance(const float chance)
{
    swiftChance = clampChance(chance);
}

void EnemyFactory::setBlinkChance(const float chance)
{
    blinkChance = clampChance(chance);
}

void EnemyFactory::assignRandomAffixes(Enemy &enemy)
{
    if (roll(swiftChance))
    {
        enemy.equipAffix(AffixId::Swift);
    }

    if (roll(blinkChance))
    {
        enemy.equipAffix(AffixId::Blink);
    }
    enemy.applyInnateAffixes();
}

bool EnemyFactory::roll(const float chance)
{
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(rng) < chance;
}

float EnemyFactory::clampChance(const float chance)
{
    return std::clamp(chance, 0.0f, 1.0f);
}
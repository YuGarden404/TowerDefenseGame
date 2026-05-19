#ifndef ENEMYFACTORY_H
#define ENEMYFACTORY_H

#include "Map.h"
#include "../entities/Enemy.h"
#include "../components/AffixTypes.h"
#include <memory>
#include <random>

class EnemyFactory
{
private:
    std::mt19937 rng;
    float swiftChance;
    float blinkChance;

public:
    EnemyFactory();

    std::unique_ptr<Enemy> createEnemy(Map &map);
    std::unique_ptr<Enemy> createEnemy(Map &map, size_t pathId);
    std::unique_ptr<Enemy> createEnemy(Map &map, size_t pathId, int hp, int maxHp, float speed, int reward);

    void setSwiftChance(float chance);
    void setBlinkChance(float chance);

private:
    void assignRandomAffixes(Enemy &enemy);
    bool roll(float chance);
    static float clampChance(float chance);
};

#endif
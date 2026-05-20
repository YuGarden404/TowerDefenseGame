#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include "../components/AffixTypes.h"
#include "../main/Map.h"

#include <vector>

enum class EntityKind
{
    None,
    Enemy,
    MeleeTower,
    RangedTower
};

struct EntityView
{
    EntityKind kind = EntityKind::None;

    float x = 0.0f;
    float y = 0.0f;

    int hp = 0;
    int maxHp = 0;

    std::vector<AffixId> equippedAffixes;
};

#endif // GAMEVIEW_H
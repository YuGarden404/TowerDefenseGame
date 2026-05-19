#ifndef AFFIXTYPES_H
#define AFFIXTYPES_H

#include <string>

enum class AffixId
{
    Slow,
    Burn,
    Berserk,
    Swift,
    Blink
};

enum class AffixTargetType
{
    EnemyOnly,
    RangedTowerOnly,
    MeleeTowerOnly
};

// TODO: 将 AffixId 转成显示名称，比如 Slow -> "减速"
std::string affixIdToName(AffixId id);

// TODO: 获取该词缀允许装备到哪类对象
AffixTargetType getAffixTargetType(AffixId id);

// TODO: 获取购买价格
int getAffixBuyPrice(AffixId id);

// TODO: 获取出售价格，等于购买价格的 80%
int getAffixSellPrice(AffixId id);

#endif
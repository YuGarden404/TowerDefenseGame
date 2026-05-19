#include "AffixTypes.h"

std::string affixIdToName(AffixId id)
{
    switch (id)
    {
    case AffixId::Slow:
        return "Slow";
    case AffixId::Burn:
        return "Burn";
    case AffixId::Berserk:
        return "Berserk";
    case AffixId::Swift:
        return "Swift";
    case AffixId::Blink:
        return "Blink";
    default:
        return "Unknown Affix";
    }
}

AffixTargetType getAffixTargetType(AffixId id)
{
    switch (id)
    {
    case AffixId::Slow:
    case AffixId::Burn:
        return AffixTargetType::RangedTowerOnly;
    case AffixId::Berserk:
        return AffixTargetType::MeleeTowerOnly;
    case AffixId::Swift:
    case AffixId::Blink:
        return AffixTargetType::EnemyOnly;
    default:
        return AffixTargetType::EnemyOnly; // 默认返回 EnemyOnly
    }
}

int getAffixBuyPrice(AffixId id)
{
    switch (id)
    {
    case AffixId::Slow:
        return 100;
    case AffixId::Burn:
        return 150;
    case AffixId::Berserk:
        return 200;
    case AffixId::Swift:
    case AffixId::Blink:
    default:
        return 0; // 默认价格
    }
}

int getAffixSellPrice(AffixId id)
{
    return getAffixBuyPrice(id) * 80 / 100;
}
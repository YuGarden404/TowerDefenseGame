//
// Created by Lenovo on 26-1-12.
//

#include "Entity.h"
#include <algorithm>

Entity::Entity() : x(0), y(0), hp(0), maxHp(0), speed(0.0)
{
    affixes.clear();
    equippedAffixes.clear();
}

Entity::Entity(const float x, const float y, const int hp, const int maxHp, const float speed)
    : x(x), y(y), hp(hp), maxHp(maxHp), speed(speed)
{
    affixes.clear();
    equippedAffixes.clear();
    if (hp > 0 && maxHp > 0 && hp <= maxHp && speed >= 0)
    {
        this->hp = hp;
        this->maxHp = maxHp;
        this->speed = speed;
    }
    else
    {
        std::cout << "Incorrect entity construction parameters:" << std::endl;
        if (hp <= 0)
            std::cout << "Entity health (hp: " << hp << " <= 0) is invalid" << std::endl;
        if (maxHp <= 0)
            std::cout << "Entity maximum health (maxHp: " << maxHp << " <= 0) is invalid" << std::endl;
        if (hp > maxHp)
            std::cout << "Entity health (hp: " << hp << ") > maximum health (maxHp: " << maxHp << ") is invalid" << std::endl;
        if (speed <= 0)
            std::cout << "Entity speed (speed: " << speed << " <= 0) is invalid" << std::endl;
        std::cout << "Default construction: x = 0, y = 0, hp = 0, maxHp = 0, speed = 0" << std::endl;

        this->hp = 0;
        this->maxHp = 0;
        this->speed = 0;
    }
}

void Entity::takeDamage(const int damage)
{
    if (damage <= 0)
    {
        return;
    }
    hp -= damage;
    if (hp <= 0)
        hp = 0;
}

bool Entity::isDead() const
{
    return hp <= 0;
}

float Entity::getX() const
{
    return x;
}

float Entity::getY() const
{
    return y;
}

int Entity::getHp() const
{
    return hp;
}

int Entity::getMaxHp() const
{
    return maxHp;
}

float &Entity::getSpeed()
{
    return speed;
}

std::vector<std::unique_ptr<Affix>> &Entity::getAffixes()
{
    return affixes;
}

void Entity::setX(const float x)
{
    this->x = x;
}

void Entity::setY(const float y)
{
    this->y = y;
}

void Entity::setHp(const int hp)
{
    this->hp = hp > 0 && hp <= maxHp ? hp : 0;
}

void Entity::setMaxHp(const int maxHp)
{
    this->maxHp = maxHp > 0 ? maxHp : 0;
}

void Entity::setSpeed(const float speed)
{
    this->speed = speed > 0 ? speed : 0;
}

void Entity::addAffix(std::unique_ptr<Affix> affix)
{
    if (!affix)
        return;
    affix->onAttach(this); // 词缀的 owner 赋值为自己
    affixes.push_back(std::move(affix));
    std::cout << "Affix added: " << affixes.back()->getName() << std::endl;
}

void Entity::updateAffixes(const float deltaTime)
{
    for (const auto &affix : affixes)
    {
        affix->update(deltaTime);
    }

    affixes.erase(
        std::remove_if(affixes.begin(), affixes.end(),
                       [](const std::unique_ptr<Affix> &affix)
                       {
                           if (affix->isExpired())
                           {
                               std::cout << "Affix expired: " << affix->getName() << std::endl;
                               affix->onDetach();
                               return true;
                           }
                           return false;
                       }),
        affixes.end());
}

float Entity::manhattanDistance(const float x1, const float y1, const float x2, const float y2)
{
    float dx = x1 - x2;
    float dy = y1 - y2;
    if (dx < 0)
        dx = -dx;
    if (dy < 0)
        dy = -dy;
    return dx + dy;
}

float Entity::euclideanDistance(float x1, float y1, float x2, float y2)
{
    const float dx = x1 - x2;
    const float dy = y1 - y2;
    return std::sqrt(dx * dx + dy * dy);
}

bool Entity::equipAffix(AffixId id)
{
    if (hasEquippedAffix(id))
    {
        return false;
    }
    equippedAffixes.push_back(id);
    return true;
}

bool Entity::unequipAffix(AffixId id)
{
    auto it = std::find(equippedAffixes.begin(), equippedAffixes.end(), id);
    if (it != equippedAffixes.end())
    {
        equippedAffixes.erase(it);
        return true;
    }
    return false;
}

bool Entity::hasEquippedAffix(AffixId id) const
{
    return std::find(equippedAffixes.begin(), equippedAffixes.end(), id) != equippedAffixes.end();
}

const std::vector<AffixId> &Entity::getEquippedAffixes() const
{
    return equippedAffixes;
}
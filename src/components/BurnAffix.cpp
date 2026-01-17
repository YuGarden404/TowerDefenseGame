//
// Created by Lenovo on 26-1-17.
//

#include "BurnAffix.h"
#include "../entities/Entity.h"

BurnAffix::BurnAffix(const float duration, const std::string& name, const int damagePerTick)
:Affix(duration,name),damagePerTick(damagePerTick),tickTimer(0.0f){}

void BurnAffix::onAttach(Entity* entity)
{
    Affix::onAttach(entity);
    if(owner)
    {
        std::cout << "[词缀生效] " << getName() << " 挂载于实体，受到持续伤害: "
                  << damagePerTick << "/s" << std::endl;
    }
}

void BurnAffix::onDetach()
{
    if (owner) {
        std::cout << "[词缀过期] " << getName() << " 已移除，实体不再受到持续伤害"
                  << std::endl;
    }
}

void BurnAffix::update(const float deltaTime)
{
    Affix::update(deltaTime);
    if(owner&&!owner->isDead())
    {
        tickTimer+=deltaTime;
        if(tickTimer>=1.0f)
        {
            std::cout << "[词缀触发] " << getName() << " 触发，造成伤害: "
                      << damagePerTick << std::endl;
            owner->takeDamage(this->damagePerTick);
            //减去阈值，保留溢出的时间部分
            tickTimer-=1.0f;
        }
    }
}
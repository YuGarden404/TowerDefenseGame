//
// Created by Lenovo on 26-1-15.
//

#include "SlowAffix.h"
#include "../entities/Entity.h"
#include <algorithm>

SlowAffix::SlowAffix(const float duration, const std::string& name, const float slowFactor)
:Affix(duration,name)
{
    this->slowFactor = std::clamp(slowFactor, 0.0f, 0.99f);
}

void SlowAffix::onAttach(Entity* entity)
{
    Affix::onAttach(entity);
    if(owner)
    {
        const float currentSpeed = owner->getSpeed();
        owner->setSpeed(currentSpeed * (1.0f - slowFactor));
        std::cout << "[词缀生效] " << getName() << " 挂载于实体，当前速度降至: "
                  << owner->getSpeed() << std::endl;
    }
}

void SlowAffix::onDetach()
{
    if (owner) {
        const float currentSpeed = owner->getSpeed();
        owner->setSpeed(currentSpeed / (1.0f - slowFactor));

        std::cout << "[词缀过期] " << getName() << " 已移除，实体速度恢复至: "
                  << owner->getSpeed() << std::endl;
    }
}
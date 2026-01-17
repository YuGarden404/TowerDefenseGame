//
// Created by Lenovo on 26-1-15.
//

#include "SlowAffix.h"

#include "../entities/Entity.h"

SlowAffix::SlowAffix(Entity* owner, const float duration, const std::string& name, const float slowAmount)
:Affix(owner,duration,name),slowAmount(slowAmount){}

void SlowAffix::onAttach(Entity* entity)
{
    entity->getSpeed() *= 1 - slowAmount;
}

void SlowAffix::onDetach()
{
    owner->getSpeed() /= 1 - slowAmount;
}
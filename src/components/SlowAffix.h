//
// Created by Lenovo on 26-1-15.
//

#ifndef SLOWAFFIX_H
#define SLOWAFFIX_H

#include "Affix.h"
#include <iostream>

class SlowAffix : public Affix {
    float slowAmount;
public:
    SlowAffix(Entity* owner, float duration, const std::string& name, float slowAmount);
    void onAttach(Entity* entity) override;
    void onDetach() override;
};



#endif //SLOWAFFIX_H

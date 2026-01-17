//
// Created by Lenovo on 26-1-15.
//

#ifndef SLOWAFFIX_H
#define SLOWAFFIX_H

#include "Affix.h"

class SlowAffix : public Affix {
    float slowFactor;
public:
    SlowAffix(float duration, const std::string& name, float slowFactor);
    void onAttach(Entity* entity) override;
    void onDetach() override;
};



#endif //SLOWAFFIX_H

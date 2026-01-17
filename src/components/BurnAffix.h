//
// Created by Lenovo on 26-1-17.
//

#ifndef BURNAFFIX_H
#define BURNAFFIX_H

#include "Affix.h"

class BurnAffix : public Affix {
    //成员变量
    int damagePerTick;  //每次触发造成的伤害
    float tickTimer;    //内部计时器
public:
    //公共成员函数
    //构造函数
    BurnAffix(float duration, const std::string& name, int damagePerTick);
    //燃烧词缀不需要在挂载和卸载时修改基础属性，但仍需重写以确保 owner 正确
    void onAttach(Entity* entity) override;
    void onDetach() override;
    //重写每帧更新逻辑
    void update(float deltaTime) override;
};



#endif //BURNAFFIX_H

//
// Created by Lenovo on 26-1-12.
//

#ifndef AFFIX_H
#define AFFIX_H

#include <utility>
#include <iostream>

class Entity;

class Affix
{
protected:
    //成员变量
    Entity *owner;
    float duration;
    std::string name;
public:
    //公共成员函数
    //构造函数
    Affix():owner(nullptr),duration(0){}
    Affix(const float duration, std::string name)
    :owner(nullptr),duration(duration),name(std::move(name)){}
    //含参构造函数
    Affix(Entity* owner, const float duration, std::string name)
    :owner(owner),duration(duration),name(std::move(name)){}
    //析构函数
    virtual ~Affix() = default;
    //当词缀刚挂到实体上时触发
    //功能：修改宿主的属性
    virtual void onAttach(Entity* entity)
    {
        this->owner = entity;
    }
    //当词缀被移除时触发
    //功能：把修改过的属性还原
    virtual void onDetach() = 0;
    //getter
    [[nodiscard]] Entity *getOwner() const{return owner;}
    [[nodiscard]] const std::string &getName() const{return name;}
    [[nodiscard]] float getDuration() const{return duration;}
    //setter
    void setOwner(Entity *owner){this->owner = owner;}
    void setName(std::string name){this->name = std::move(name);}
    void setDuration(const float duration){this->duration = duration;}
    //虚函数
    //逻辑：
    //减少 duration
    //如果 duration <= 0，标记该词缀已失效。
    //执行每帧触发的效果（如流血扣血）。
    virtual void update(float deltaTime)
    {
        if(duration>0)
        {
            duration -= deltaTime;
        }
    }
    //判断词缀是否到了该移除的时间
    [[nodiscard]] bool isExpired() const{return duration <= 0;}
};

#endif //AFFIX_H

//
// Created by Lenovo on 26-1-12.
//

#include "Entity.h"
#include <iostream>

Entity::Entity(): x(0),y(0),hp(0),maxHp(0),speed(0){}

Entity::Entity(const float x, const float y, const int hp, const int maxHp, const float speed)
:x(x),y(y)
{
    if(hp>0&&maxHp>0&&hp<=maxHp&&speed>0)
    {
        this->hp = hp;
        this->maxHp = maxHp;
        this->speed = speed;
    }else
    {
        std::cout << "实体构造参数有误：" << std::endl;
        if(hp<=0) std::cout << "实体生命值(hp：" << hp << "<=0)信息有误" << std::endl;
        if(maxHp<=0) std::cout << "实体最大生命值(maxHp：" << maxHp << "<=0)信息有误" << std::endl;
        if(hp>maxHp) std::cout << "实体生命值(hp：" << hp << ")>最大生命值(maxHp：" << maxHp << ")信息有误" << std::endl;
        if(speed<=0) std::cout << "实体速度(speed：" << speed << "<=0)信息有误" << std::endl;
        std::cout << "默认构造：x = 0,y = 0,hp = 0,maxHp = 0,speed = 0" << std::endl;

        this->hp = 0;
        this->maxHp = 0;
        this->speed = 0;
    }
}

void Entity::takeDamage(const int damage)
{
    hp -= damage;
    if(hp<=0)hp=0;
    if(isDead()) std::cout << "实体已死亡" << std::endl;
}

bool Entity::isDead() const
{
    return hp<=0;
}

float Entity::getX() const
{
    return x;
}

float Entity::getY() const
{
    return y;
}

void Entity::setX(const float x)
{
    this->x = x;
}

 void Entity::setY(const float y)
 {
     this->y = y;
 }
//
// Created by Lenovo on 26-1-12.
//

#include "Entity.h"

Entity::Entity(): x(0),y(0),hp(0),maxHp(0),speed(0.0)
{
    affixes.clear();
}

Entity::Entity(const float x, const float y, const int hp, const int maxHp, const float speed)
:x(x),y(y)
{
    affixes.clear();
    if(hp>0&&maxHp>0&&hp<=maxHp&&speed>=0)
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

int Entity::getHp() const
{
    return hp;
}

int Entity::getMaxHp() const
{
    return maxHp;
}

float& Entity::getSpeed()
{
    return speed;
}

std::vector<std::unique_ptr<Affix>>& Entity::getAffixes()
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

void Entity::addAffix(std::unique_ptr<Affix> affix)
{
    if(!affix)return;
    affix->onAttach(this);//词缀的 owner 赋值为自己
    affixes.push_back(std::move(affix));
    std::cout << "词缀已添加：" << affixes.back()->getName() << std::endl;
}

void Entity::updateAffixes(const float deltaTime)
{
    for(const auto& affix:affixes)
    {
        affix->update(deltaTime);
    }
    std::erase_if(affixes,[](const std::unique_ptr<Affix>& affix)
    {
        if(affix->isExpired())
        {
            affix->onDetach();//还原实体的属性
            return true;
        }
        return false;
    });
}

void Entity::update(float deltaTime, std::vector<Entity*>& entities)
{
    if(isDead())return;
    updateAffixes(deltaTime);
}


float Entity::manhattanDistance(const float x1, const float y1, const float x2, const float y2)
{
    float dx = x1 - x2;
    float dy = y1 - y2;
    if(dx<0)dx=-dx;
    if(dy<0)dy=-dy;
    return dx+dy;
}

float Entity::euclideanDistance(float x1, float y1, float x2, float y2)
{
    const float dx = x1 - x2;
    const float dy = y1 - y2;
    return std::sqrt(dx*dx+dy*dy);
}

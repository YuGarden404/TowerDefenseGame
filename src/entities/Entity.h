//
// Created by Lenovo on 26-1-12.
//

#ifndef ENTITY_H
#define ENTITY_H

#include <iostream>
#include <cmath>
#include <vector>

class Entity {
protected:
    //成员变量
    float x,y;
    int hp;
    int maxHp;
    float speed;
public:
    //公共成员函数
    //构造函数
    Entity();
    //含参构造函数
    Entity(float x,float y,int hp,int maxHp,float speed);
    //虚析构函数，确保删除基类指针时子类内存能正确释放
    virtual ~Entity() = default;
    //纯虚函数，每一帧都要执行逻辑
    //deltaTime 是两帧之间的时间差，用于平滑计算
    virtual void update(float deltaTime, std::vector<Entity*>& entities) = 0;
    //扣血函数
    void takeDamage(int damage);
    //判断是否死亡(血量<=0)
    [[nodiscard]] bool isDead() const;
    //获取位置
    [[nodiscard]] float getX() const;
    [[nodiscard]] float getY() const;
    //设置位置
    void setX(float x);
    void setY(float y);

    static float manhattanDistance(float x1, float y1, float x2, float y2);
    static float euclideanDistance(float x1, float y1, float x2, float y2);
};



#endif //ENTITY_H

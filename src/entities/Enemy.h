//
// Created by Lenovo on 26-1-12.
//

#ifndef ENEMY_H
#define ENEMY_H

#include "Entity.h"
#include "../main/Map.h"

class Enemy : public Entity {
    //成员变量
    const Map& map; //敌人需要引用地图，以此知道路径在哪里
    int pathIndex{};  //记录当前走到了路径的第几个点

    //私有成员函数
    static float manhattanDistance(float x1, float y1, float x2, float y2);
public:
    //公共成员函数
    //含参构造函数
    Enemy(Map& map, float x, float y, int hp, int maxHp, float speed);
    //纯虚函数，每一帧都要执行逻辑
    void update(float deltaTime) override;
};



#endif //ENEMY_H

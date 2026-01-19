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
    int reward; //敌人死亡时，给玩家奖励的金币
    bool isBlocked; //判断是否被MeleeTower阻挡
    int attackPower;    //敌人对塔造成的伤害
    float attackCooldown;   //攻击间隔
    float attackTimer;  //攻击计时器
public:
    //公共成员函数
    //含参构造函数
    Enemy(Map& map, float x, float y, int hp, int maxHp, float speed, int reward);
    //纯虚函数，每一帧都要执行逻辑
    void update(float deltaTime, std::vector<Entity*>& entities) override;
    //getter
    [[nodiscard]] int getReward() const;
    [[nodiscard]] bool getBlocked() const{return isBlocked;}
    //setter
    void setReward(const int reward){this->reward = reward;}
    void setBlocked(const bool blocked){isBlocked = blocked;}
};



#endif //ENEMY_H

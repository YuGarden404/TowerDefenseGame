//
// Created by Lenovo on 26-1-12.
//

#ifndef ENEMY_H
#define ENEMY_H

#include "Entity.h"
#include "../main/Map.h"

class Enemy : public Entity
{
    // 成员变量
    const Map &map;
    size_t pathId;
    size_t pathIndex;
    int reward;
    bool isBlocked;
    bool reachedEnd;
    int attackPower;
    float attackCooldown;
    float attackTimer;
    std::weak_ptr<Entity> blockedBy;

public:
    // 公共成员函数
    // 含参构造函数
    Enemy(Map &map, size_t pathId, float x, float y, int hp, int maxHp, float speed, int reward);
    void update(float deltaTime, std::vector<std::shared_ptr<Entity>> &entities) override;

    [[nodiscard]] int getReward() const;
    [[nodiscard]] bool getBlocked() const { return isBlocked; }
    [[nodiscard]] bool hasReachedEnd() const { return reachedEnd; }
    [[nodiscard]] size_t getPathId() const { return pathId; }

    void setReward(int reward) { this->reward = reward; }
    void setBlocked(bool blocked) { isBlocked = blocked; }
    void setBlockedBy(std::shared_ptr<Entity> blocker);
    void clearBlockedBy();
};

#endif // ENEMY_H

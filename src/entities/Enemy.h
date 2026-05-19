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
    float blinkCooldown;
    float blinkTimer;
    bool innateAffixesApplied;

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
    void applyInnateAffixes();

    // 根据 deltaTime 更新闪现冷却
    void updateBlinkCooldown(float deltaTime);

    // 是否拥有 Blink 且当前可闪现
    bool canBlink() const;

    // 触发闪现，让敌人前进一段路径并进入冷却
    void triggerBlink();

    // 让敌人沿当前路径跳过若干路径点
    void blinkForward();
};

#endif // ENEMY_H

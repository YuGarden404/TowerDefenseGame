//
// Created by Lenovo on 26-1-12.
//

#include "Enemy.h"

Enemy::Enemy(Map& map, const float x, const float y, const int hp, const int maxHp, const float speed, int reward)
:Entity(x,y,hp,maxHp,speed),map(map),reward(reward){}

void Enemy::update(const float deltaTime, std::vector<Entity*>& entities) {
    if (isDead() || pathIndex >= map.getEnemyPath().size()) return;
    const Point& target = map.getEnemyPath()[pathIndex];
    const auto targetX = static_cast<float>(target.x);
    const auto targetY = static_cast<float>(target.y);
    const float dx = targetX - getX();
    const float dy = targetY - getY();
    if(const float distance = std::sqrt(dx*dx+dy*dy); distance>0.1f)
    {
        //归一化移动：确保每一帧移动的步长是平滑的
        x += (dx / distance) * speed * deltaTime;
        y += (dy / distance) * speed * deltaTime;
    }else
    {
        pathIndex++;
        if (pathIndex >= map.getEnemyPath().size()) {
            std::cout << "敌人已到达终点！" << std::endl;
            takeDamage(hp);
        }
    }
}

int Enemy::getReward() const {
    return reward;
}
//
// Created by Lenovo on 26-1-12.
//

#include "Enemy.h"
#include <cmath>

float Enemy::manhattanDistance(const float x1, const float y1, const float x2, const float y2)
{
    float dx = x1 - x2;
    float dy = y1 - y2;
    if(dx<0)dx=-dx;
    if(dy<0)dy=-dy;
    return dx+dy;
}

Enemy::Enemy(Map& map, const float x, const float y, const int hp, const int maxHp, const float speed)
:Entity(x,y,hp,maxHp,speed),map(map){}

void Enemy::update(const float deltaTime) {
    if (isDead() || pathIndex >= map.getEnemyPath().size()) return;
    const Point& target = map.getEnemyPath()[pathIndex];
    const auto targetX = static_cast<float>(target.x);
    const auto targetY = static_cast<float>(target.y);
    const float dx = targetX - getX();
    const float dy = targetY - getY();
    const float distance = std::sqrt(dx*dx+dy*dy);
    if(distance>0.1f)
    {
        //归一化移动：确保每一帧移动的步长是平滑的
        x += (dx / distance) * speed * deltaTime;
        y += (dy / distance) * speed * deltaTime;
    }else
    {
        pathIndex++;
        if (pathIndex >= map.getEnemyPath().size()) {
            std::cout << "敌人已到达终点！" << std::endl;
            hp = 0;
        }
    }
}
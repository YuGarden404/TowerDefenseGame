//
// Created by Lenovo on 26-1-12.
//

#ifndef GAME_H
#define GAME_H

#include "Map.h"
#include "../entities/Entity.h"
#include "../entities/Enemy.h"
#include "../entities/RangedTower.h"
#include <memory>
#include <thread>

class Game {
    //成员变量
    Map map;    //游戏地图
    std::vector<std::unique_ptr<Entity>>allEntities;    //核心容器
    int money;  //玩家持有的金钱
public:
    //公共成员函数
    //构造函数
    Game(int w, int h);
    //getter
    [[nodiscard]] int getMoney() const {return money;}
    [[nodiscard]] Map& getMap() {return map;}
    [[nodiscard]] const Map& getMap() const {return map;}
    [[nodiscard]] std::vector<std::unique_ptr<Entity>>& getAllEntities() {return allEntities;}
    [[nodiscard]] const std::vector<std::unique_ptr<Entity>>& getAllEntities() const {return allEntities;}
    //每一帧的逻辑更新
    void update(float deltaTime);
    //在地图起点生成一个敌人的快捷方法
    void spawnEnemy();
    //向游戏中添加单位
    void addEntity(std::unique_ptr<Entity> entity);
    //绘制地图
    void render() const;
    //升级塔
    bool purchaseUpgrade(int x, int y, std::string effectName);
};



#endif //GAME_H

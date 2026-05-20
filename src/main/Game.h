//
// Created by Lenovo on 26-1-12.
//

#ifndef GAME_H
#define GAME_H

#include "Map.h"
#include "../components/AffixTypes.h"
#include "../entities/Entity.h"
#include "../entities/Enemy.h"
#include "../entities/RangedTower.h"
#include "../entities/MeleeTower.h"
#include <memory>
#include <thread>
#include "EnemyFactory.h"
#include "../view/GameView.h"

class Game
{
    // 成员变量
    Map map;                                          // 游戏地图
    EnemyFactory enemyFactory;                        // 敌人工厂
    std::vector<std::shared_ptr<Entity>> allEntities; // 核心容器
    int money;                                        // 玩家持有的金钱
    int playerHp;
    int totalEnemiesToSpawn;
    int spawnedEnemyCount;
    bool gameOver;
    bool victory;
    bool paused;

    // 根据地图坐标查找该格上的实体
    std::shared_ptr<Entity> findEntityAt(int x, int y);

    // 判断某个实体是否允许装备指定词缀
    bool canEquipAffixTo(const Entity &entity, AffixId affixId) const;

    bool canPurchaseAffixFor(const Entity &entity, AffixId affixId) const;

    // 给指定坐标的实体购买并装备词缀
    bool purchaseAffix(int x, int y, AffixId affixId);

    // 出售指定坐标实体身上的词缀，返还购买价 80%
    bool sellAffix(int x, int y, AffixId affixId);

    // 向游戏中添加单位
    void addEntity(std::unique_ptr<Entity> entity);
    // 放置塔
    bool placeTower(std::unique_ptr<Tower> tower);

public:
    // 公共成员函数
    // 构造函数
    Game(int w, int h);
    // getter
    [[nodiscard]] int getMoney() const { return money; }
    [[nodiscard]] Map &getMap() { return map; }
    [[nodiscard]] const Map &getMap() const { return map; }
    [[nodiscard]] std::vector<std::shared_ptr<Entity>> &getAllEntities() { return allEntities; }
    [[nodiscard]] const std::vector<std::shared_ptr<Entity>> &getAllEntities() const { return allEntities; }
    [[nodiscard]] int getPlayerHp() const { return playerHp; }
    [[nodiscard]] bool isGameOver() const { return gameOver; }
    [[nodiscard]] bool isVictory() const { return victory; }
    [[nodiscard]] int getSpawnedEnemyCount() const { return spawnedEnemyCount; }
    [[nodiscard]] int getTotalEnemiesToSpawn() const { return totalEnemiesToSpawn; }
    // 返回所有活着实体的只读视图
    [[nodiscard]] std::vector<EntityView> getEntityViews() const;

    // 返回指定地图格子上的实体视图，没有实体则 kind 为 None
    [[nodiscard]] EntityView getEntityViewAt(int x, int y) const;

    // 返回指定地图格子的地形类型
    [[nodiscard]] TileType getTileAt(int x, int y) const;

    void setTotalEnemiesToSpawn(int total);
    bool allEnemiesCleared() const;
    // 每一帧的逻辑更新
    void update(float deltaTime);
    // 在地图起点生成一个敌人的快捷方法
    void spawnEnemy();

    // 判断玩家金币是否足够
    bool canAfford(int cost) const;

    // 扣除金币，成功返回 true，金币不足返回 false
    bool spendMoney(int amount);

    // 增加金币
    void addMoney(int amount);

    // 在指定格子放置近战塔
    bool placeMeleeTowerAt(int x, int y);

    // 在指定格子放置远程塔
    bool placeRangedTowerAt(int x, int y);

    // GUI 友好的购买接口，内部调用 purchaseAffix
    bool buyAffixAt(int x, int y, AffixId affixId);

    // GUI 友好的出售接口，内部调用 sellAffix
    bool sellAffixAt(int x, int y, AffixId affixId);

    // 暂停/继续
    void setPaused(bool paused);
    [[nodiscard]] bool isPaused() const;

    // 重置本局，保留地图
    void reset();
};

#endif // GAME_H

//
// Created by Lenovo on 26-1-12.
//
#include "Game.h"
#include <algorithm>

namespace
{
    EntityView makeEntityViewFromEntity(const std::shared_ptr<Entity> &entity)
    {
        EntityView view;

        if (!entity || entity->isDead())
        {
            return view;
        }

        view.x = entity->getX();
        view.y = entity->getY();
        view.hp = entity->getHp();
        view.maxHp = entity->getMaxHp();
        view.equippedAffixes = entity->getEquippedAffixes();

        if (dynamic_cast<Enemy *>(entity.get()))
        {
            view.kind = EntityKind::Enemy;
        }
        else if (dynamic_cast<MeleeTower *>(entity.get()))
        {
            view.kind = EntityKind::MeleeTower;
        }
        else if (dynamic_cast<RangedTower *>(entity.get()))
        {
            view.kind = EntityKind::RangedTower;
        }

        return view;
    }
}

Game::Game(const int w, const int h)
    : map(w, h),
      enemyFactory(),
      money(1000),
      playerHp(10),
      totalEnemiesToSpawn(20),
      spawnedEnemyCount(0),
      gameOver(false),
      victory(false),
      paused(false)
{
    allEntities.clear();
}

void Game::update(const float deltaTime)
{
    if (gameOver || paused)
    {
        return;
    }

    for (auto &entity : allEntities)
    {
        if (!entity || entity->isDead())
            continue;
        entity->update(deltaTime, allEntities);
    }

    allEntities.erase(
        std::remove_if(allEntities.begin(), allEntities.end(),
                       [this](const std::shared_ptr<Entity> &entity)
                       {
                           if (!entity || !entity->isDead())
                           {
                               return false;
                           }

                           if (const auto *enemy = dynamic_cast<Enemy *>(entity.get()))
                           {
                               if (enemy->hasReachedEnd())
                               {
                                   playerHp--;
                                   if (playerHp <= 0)
                                   {
                                       playerHp = 0;
                                       gameOver = true;
                                       victory = false;
                                   }
                               }
                               else
                               {
                                   money += enemy->getReward();
                               }
                           }

                           return true;
                       }),
        allEntities.end());

    if (!gameOver && spawnedEnemyCount >= totalEnemiesToSpawn && allEnemiesCleared())
    {
        gameOver = true;
        victory = playerHp > 0;
    }
}

void Game::spawnEnemy()
{
    if (gameOver)
    {
        return;
    }

    if (spawnedEnemyCount >= totalEnemiesToSpawn)
    {
        return;
    }

    auto enemy = enemyFactory.createEnemy(map);
    if (!enemy)
    {
        return;
    }

    addEntity(std::move(enemy));
    spawnedEnemyCount++;
}

bool Game::placeTower(std::unique_ptr<Tower> tower)
{
    if (!tower)
    {
        return false;
    }

    int ix = static_cast<int>(tower->getX() + 0.5f);
    int iy = static_cast<int>(tower->getY() + 0.5f);

    bool canPlace = false;
    if (dynamic_cast<MeleeTower *>(tower.get()))
    {
        canPlace = map.canPlaceMeleeTower(ix, iy);
    }
    else if (dynamic_cast<RangedTower *>(tower.get()))
    {
        canPlace = map.canPlaceRangedTower(ix, iy);
    }

    if (!canPlace)
    {
        std::cout << "放置失败：无法在该地块上放置防御塔" << std::endl;
        return false;
    }

    for (const auto &e : allEntities)
    {
        if (dynamic_cast<Tower *>(e.get()) &&
            static_cast<int>(e->getX() + 0.5f) == ix &&
            static_cast<int>(e->getY() + 0.5f) == iy)
        {
            std::cout << "放置失败：该位置已有防御塔" << std::endl;
            return false;
        }
    }

    addEntity(std::move(tower));
    return true;
}

void Game::addEntity(std::unique_ptr<Entity> entity)
{
    if (!entity)
        return;
    allEntities.emplace_back(std::move(entity));
}

bool Game::canAfford(int cost) const
{
    return cost >= 0 && money >= cost;
}

bool Game::spendMoney(int amount)
{
    if (amount < 0)
    {
        std::cout << "Failed to spend money: Amount must be a positive number" << std::endl;
        return false;
    }
    if (canAfford(amount))
    {
        money -= amount;
        return true;
    }
    return false;
}

void Game::addMoney(int amount)
{
    if (amount > 0)
    {
        money += amount;
    }
    else
    {
        std::cout << "Failed to add money: Amount must be a positive number" << std::endl;
    }
}

// 给指定坐标的实体购买并装备词缀
bool Game::canEquipAffixTo(const Entity &entity, AffixId affixId) const
{
    AffixTargetType targetType = getAffixTargetType(affixId);

    if (targetType == AffixTargetType::EnemyOnly)
    {
        return dynamic_cast<const Enemy *>(&entity) != nullptr;
    }

    if (targetType == AffixTargetType::RangedTowerOnly)
    {
        return dynamic_cast<const RangedTower *>(&entity) != nullptr;
    }

    if (targetType == AffixTargetType::MeleeTowerOnly)
    {
        return dynamic_cast<const MeleeTower *>(&entity) != nullptr;
    }

    return false;
}

// 出售指定坐标实体身上的词缀，返还购买价 80%
bool Game::sellAffix(int x, int y, AffixId affixId)
{
    auto entity = findEntityAt(x, y);
    if (!entity)
    {
        return false;
    }

    if (!canPurchaseAffixFor(*entity, affixId))
    {
        return false;
    }

    if (!entity->hasEquippedAffix(affixId))
    {
        return false;
    }

    if (entity->unequipAffix(affixId))
    {
        addMoney(getAffixSellPrice(affixId));
        return true;
    }

    return false;
}

// 根据地图坐标查找该格上的实体
std::shared_ptr<Entity> Game::findEntityAt(int x, int y)
{
    for (auto &entity : allEntities)
    {
        if (!entity || entity->isDead())
            continue;
        if (Entity::euclideanDistance(entity->getX(), entity->getY(), x, y) < 0.5f)
        {
            return entity;
        }
    }
    return nullptr;
}

bool Game::canPurchaseAffixFor(const Entity &entity, AffixId affixId) const
{
    if (entity.isDead())
    {
        return false;
    }

    if (dynamic_cast<const Tower *>(&entity) == nullptr)
    {
        return false;
    }

    if (affixId == AffixId::Swift || affixId == AffixId::Blink)
    {
        return false;
    }

    return canEquipAffixTo(entity, affixId);
}

bool Game::allEnemiesCleared() const
{
    for (const auto &entity : allEntities)
    {
        if (entity && !entity->isDead() && dynamic_cast<Enemy *>(entity.get()))
        {
            return false;
        }
    }

    return true;
}

void Game::setTotalEnemiesToSpawn(const int total)
{
    if (total > 0)
    {
        totalEnemiesToSpawn = total;
    }
}

bool Game::purchaseAffix(int x, int y, AffixId affixId)
{
    auto entity = findEntityAt(x, y);
    if (!entity)
    {
        return false;
    }

    if (!canPurchaseAffixFor(*entity, affixId))
    {
        return false;
    }

    if (entity->hasEquippedAffix(affixId))
    {
        return false;
    }

    int cost = getAffixBuyPrice(affixId);
    if (!spendMoney(cost))
    {
        return false;
    }

    if (entity->equipAffix(affixId))
    {
        return true;
    }
    addMoney(cost);

    return false;
}

std::vector<EntityView> Game::getEntityViews() const
{
    std::vector<EntityView> views;

    for (const auto &entity : allEntities)
    {
        EntityView view = makeEntityViewFromEntity(entity);
        if (view.kind != EntityKind::None)
        {
            views.push_back(view);
        }
    }

    return views;
}

EntityView Game::getEntityViewAt(int x, int y) const
{
    for (const auto &entity : allEntities)
    {
        if (!entity || entity->isDead())
        {
            continue;
        }

        if (static_cast<int>(entity->getX() + 0.5f) == x &&
            static_cast<int>(entity->getY() + 0.5f) == y)
        {
            return makeEntityViewFromEntity(entity);
        }
    }

    return EntityView{};
}

TileType Game::getTileAt(int x, int y) const
{
    return map.getTileType(x, y);
}

bool Game::placeMeleeTowerAt(int x, int y)
{
    auto tower = std::make_unique<MeleeTower>(x, y);
    const int cost = tower->getCost();

    if (!canAfford(cost))
    {
        std::cout << "Placement failed: not enough gold." << std::endl;
        return false;
    }

    if (!placeTower(std::move(tower)))
    {
        return false;
    }

    money -= cost;
    return true;
}

bool Game::placeRangedTowerAt(int x, int y)
{
    auto tower = std::make_unique<RangedTower>(x, y);
    const int cost = tower->getCost();

    if (!canAfford(cost))
    {
        std::cout << "Placement failed: not enough gold." << std::endl;
        return false;
    }

    if (!placeTower(std::move(tower)))
    {
        return false;
    }

    money -= cost;
    return true;
}

bool Game::buyAffixAt(int x, int y, AffixId affixId)
{
    return purchaseAffix(x, y, affixId);
}

bool Game::sellAffixAt(int x, int y, AffixId affixId)
{
    return sellAffix(x, y, affixId);
}

void Game::setPaused(bool paused)
{
    this->paused = paused;
}

bool Game::isPaused() const
{
    return paused;
}

void Game::reset()
{
    allEntities.clear();

    money = 1000;
    playerHp = 10;
    totalEnemiesToSpawn = 20;
    spawnedEnemyCount = 0;
    gameOver = false;
    victory = false;
    paused = false;
}

bool Game::sellTower(int x, int y)
{
    auto entity = findEntityAt(x, y);
    if (!entity)
    {
        std::cout << "Sell tower failed: no entity at selected cell." << std::endl;
        return false;
    }

    auto *tower = dynamic_cast<Tower *>(entity.get());
    if (!tower)
    {
        std::cout << "Sell tower failed: selected entity is not a tower." << std::endl;
        return false;
    }

    const int sellPrice = tower->getCost() * 70 / 100;

    auto it = std::find(allEntities.begin(), allEntities.end(), entity);
    if (it == allEntities.end())
    {
        return false;
    }

    allEntities.erase(it);
    money += sellPrice;
    return true;
}

bool Game::sellTowerAt(int x, int y)
{
    return sellTower(x, y);
}
//
// Created by Lenovo on 26-1-12.
//
#ifdef _WIN32
#include <windows.h>
#endif

#include "Game.h"
#include <algorithm>

Game::Game(const int w, const int h)
    : map(w, h),
      enemyFactory(),
      money(1000),
      playerHp(10),
      totalEnemiesToSpawn(20),
      spawnedEnemyCount(0),
      gameOver(false),
      victory(false)
{
    allEntities.clear();
}

void Game::update(const float deltaTime)
{
    if (gameOver)
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

void Game::render() const
{
#ifdef _WIN32
    system("cls");
#else
    std::cout << "\033[2J\033[H";
#endif

    std::cout << "Gold: " << money
              << "  HP: " << playerHp
              << "  Enemies: " << spawnedEnemyCount << "/" << totalEnemiesToSpawn;

    if (gameOver)
    {
        std::cout << "  Result: " << (victory ? "Victory" : "Defeat");
    }

    std::cout << std::endl;

    for (int i = 0; i < map.getHeight(); i++)
    {
        for (int j = 0; j < map.getWidth(); j++)
        {
            Entity *entityAtPos = nullptr;

            for (auto &e : allEntities)
            {
                if (!e || e->isDead())
                {
                    continue;
                }

                if (static_cast<int>(e->getX() + 0.5f) == j &&
                    static_cast<int>(e->getY() + 0.5f) == i)
                {
                    entityAtPos = e.get();
                    break;
                }
            }

            if (entityAtPos)
            {
                if (dynamic_cast<Enemy *>(entityAtPos))
                {
                    std::cout << "E";
                }
                else
                {
                    std::cout << "T";
                }
            }
            else
            {
                switch (map.getTileType(j, i))
                {
                case TileType::GRASS:
                    std::cout << ".";
                    break;
                case TileType::ROAD:
                    std::cout << "#";
                    break;
                case TileType::ROCK:
                    std::cout << "O";
                    break;
                case TileType::START:
                    std::cout << "S";
                    break;
                case TileType::END:
                    std::cout << "@";
                    break;
                default:
                    std::cout << "?";
                    break;
                }
            }
        }

        std::cout << std::endl;
    }
}

bool Game::placeTower(std::unique_ptr<Tower> tower)
{
    if (!tower)
        return false;
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
    if (canPlace)
    {
        for (const auto &e : allEntities)
        {
            if (dynamic_cast<Tower *>(e.get()) && static_cast<int>(e->getX() + 0.5f) == ix && static_cast<int>(e->getY() + 0.5f) == iy)
            {
                std::cout << "Placement failed: A building already exists at this position" << std::endl;
                return false;
            }
        }
        addEntity(std::move(tower));
        return true;
    }
    std::cout << "Placement failed: The placed object does not match the terrain type" << std::endl;
    return false;
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
        money += getAffixSellPrice(affixId);
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
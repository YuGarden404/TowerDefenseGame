//
// Created by Lenovo on 26-1-12.
//

#include "Game.h"
#include <algorithm>

Game::Game(const int w, const int h)
:map(w,h),money(100)
{
    allEntities.clear();
}

void Game::update(const float deltaTime)
{
    std::vector<Entity*>ptrs;
    for(auto& entity:allEntities)
    {
        ptrs.push_back(entity.get());
    }
    for(const auto& entity:allEntities)
    {
        if(entity->isDead())continue;
        entity->update(deltaTime,ptrs);
    }
    std::erase_if(allEntities,[this](const std::unique_ptr<Entity>& entity)
    {
        if(entity->isDead())
        {
            if(const auto* enemy = dynamic_cast<Enemy*>(entity.get()))
            {
                if(!map.getEnemyPath().empty()&&Entity::manhattanDistance(enemy->getX(), enemy->getY(), map.getEnemyPath().back().x, map.getEnemyPath().back().y)>0.5f)
                {
                    money += enemy->getReward();
                }
            }
            return true;
        }
        return false;
    });
}

void Game::spawnEnemy()
{
    if (map.getEnemyPath().empty()) return;
    const Point& start = map.getEnemyPath()[0];
    auto enemy = std::make_unique<Enemy>(map, start.x, start.y, 100, 100, 1.0f,100);
    addEntity(std::move(enemy));
}

void Game::addEntity(std::unique_ptr<Entity> entity)
{
    allEntities.push_back(std::move(entity));
}

void Game::render() const
{
    system("cls");
    std::cout << "当前金币: " << money << std::endl;
    for (int i = 0; i < map.getHeight(); i++) {
        for (int j = 0; j < map.getWidth(); j++) {
            Entity* entityAtPos = nullptr;
            for(auto& e : allEntities) {
                if (static_cast<int>(e->getX() + 0.5f) == j&&static_cast<int>(e->getY() + 0.5f) == i)
                {
                    entityAtPos = e.get();
                    break;
                }
            }

            if (entityAtPos) {
                if (dynamic_cast<Enemy*>(entityAtPos)) std::cout << "E";
                else std::cout << "T";
            } else {
                switch (map.getTileType(j, i)) {
                case TileType::GROUND: std::cout << "."; break;
                case TileType::PATH:   std::cout << "#"; break;
                case TileType::START:  std::cout << "S"; break;
                case TileType::END:    std::cout << "@"; break;
                default:               std::cout << "?"; break;
                }
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    std::cout << std::endl;
}

bool Game::purchaseUpgrade(const int x, const int y, std::string effectName)
{
    for(auto& entity:allEntities)
    {
        if(Entity::euclideanDistance(entity->getX(), entity->getY(), x, y)<0.5f)
        {
            if(auto* tower = dynamic_cast<Tower*>(entity.get()))
            {
                if(!std::ranges::any_of(tower->getOnHitEffects(),[&effectName](const std::string& effect)
                {
                    return effect == effectName;
                }))
                {
                    if(money>=10)
                    {
                        money -= 10;
                        tower->getOnHitEffects().emplace_back(effectName);
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool Game::placeTower(std::unique_ptr<Tower> tower)
{
    if(!tower)return false;
    int ix = static_cast<int>(tower->getX()+0.5f);
    int iy = static_cast<int>(tower->getY()+0.5f);
    TileType tileType = map.getTileType(ix,iy);
    bool canPlace = false;
    if(dynamic_cast<MeleeTower*>(tower.get()))
    {
        canPlace = tileType == TileType::PATH;
    }else if(dynamic_cast<RangedTower*>(tower.get()))
    {
        canPlace = tileType == TileType::GROUND;
    }
    if(canPlace)
    {
        for(const auto& e : allEntities)
        {
            if(dynamic_cast<Tower*>(e.get())&&static_cast<int>(e->getX()+0.5f)==ix&&static_cast<int>(e->getY()+0.5f)==iy)
            {
                std::cout << "放置失败：该位置已有建筑" << std::endl;
                return false;
            }
        }
        addEntity(std::move(tower));
        return true;
    }
    std::cout << "放置失败：放置对象与地形类型不匹配" << std::endl;
    return false;
}
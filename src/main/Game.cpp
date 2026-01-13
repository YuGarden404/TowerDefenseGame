//
// Created by Lenovo on 26-1-12.
//

#include "Game.h"

Game::Game(int w, int h)
:map(w,h),money(100)
{
    allEntities.clear();
}

void Game::update(float deltaTime)
{
    std::vector<Entity*>ptrs;
    for(auto& entity:allEntities)
    {
        ptrs.push_back(entity.get());
    }
    for(auto& entity:allEntities)
    {
        if(entity->isDead())continue;
        entity->update(deltaTime,ptrs);
    }
    std::erase_if(allEntities,[](const std::unique_ptr<Entity>& entity)
    {
        return entity->isDead();
    });
}

void Game::spawnEnemy()
{
    if (map.getEnemyPath().empty()) return;
    const Point& start = map.getEnemyPath()[0];
    auto enemy = std::make_unique<Enemy>(map, start.x, start.y, 100, 100, 1.0f);
    addEntity(std::move(enemy));
}

void Game::addEntity(std::unique_ptr<Entity> entity)
{
    allEntities.push_back(std::move(entity));
}

void Game::render() const
{
    system("cls");
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

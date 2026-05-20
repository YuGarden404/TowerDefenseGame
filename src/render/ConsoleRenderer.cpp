#include "ConsoleRenderer.h"
#include "../entities/Enemy.h"
#include "../entities/Tower.h"
#include "../entities/MeleeTower.h"
#include "../entities/RangedTower.h"
#include "../main/Game.h"
#include <iostream>
#include <cstdlib>
void ConsoleRenderer::render(const Game &game) const
{
#ifdef _WIN32
    system("cls");
#else
    std::cout << "\033[2J\033[H";
#endif

    std::cout << "Gold: " << game.getMoney()
              << "  HP: " << game.getPlayerHp()
              << "  Enemies: " << game.getSpawnedEnemyCount() << "/" << game.getTotalEnemiesToSpawn()
              << std::endl;

    const Map &map = game.getMap();
    for (int y = 0; y < map.getHeight(); y++)
    {
        for (int x = 0; x < map.getWidth(); x++)
        {
            char tileChar;
            switch (map.getTileType(x, y))
            {
            case TileType::GRASS:
                tileChar = '.';
                break;
            case TileType::ROAD:
                tileChar = '#';
                break;
            case TileType::ROCK:
                tileChar = 'O';
                break;
            case TileType::START:
                tileChar = 'S';
                break;
            case TileType::END:
                tileChar = '@';
                break;
            default:
                tileChar = '?';
                break;
            }

            for (const auto &entity : game.getAllEntities())
            {
                if (!entity || entity->isDead())
                {
                    continue;
                }
                if (static_cast<int>(entity->getX() + 0.5f) == x &&
                    static_cast<int>(entity->getY() + 0.5f) == y)
                {
                    if (dynamic_cast<Enemy *>(entity.get()))
                    {
                        tileChar = 'E';
                    }
                    else if (dynamic_cast<RangedTower *>(entity.get()))
                    {
                        tileChar = 'R';
                    }
                    else if (dynamic_cast<MeleeTower *>(entity.get()))
                    {
                        tileChar = 'M';
                    }

                    break;
                }
            }

            std::cout << tileChar << ' ';
        }
        std::cout << std::endl;
    }
}
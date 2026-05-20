#include "ConsoleRenderer.h"
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
              << "  Enemies: " << game.getSpawnedEnemyCount() << "/" << game.getTotalEnemiesToSpawn();

    if (game.isPaused())
    {
        std::cout << "  Paused";
    }

    if (game.isGameOver())
    {
        std::cout << "  Result: " << (game.isVictory() ? "Victory" : "Defeat");
    }

    std::cout << std::endl;

    for (int y = 0; y < game.getMap().getHeight(); y++)
    {
        for (int x = 0; x < game.getMap().getWidth(); x++)
        {
            char tileChar;
            switch (game.getTileAt(x, y))
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

            EntityView view = game.getEntityViewAt(x, y);

            if (view.kind != EntityKind::None)
            {
                switch (view.kind)
                {
                case EntityKind::Enemy:
                    tileChar = 'E';
                    break;
                case EntityKind::RangedTower:
                    tileChar = 'R';
                    break;
                case EntityKind::MeleeTower:
                    tileChar = 'M';
                    break;
                default:
                    break;
                }
            }

            std::cout << tileChar << ' ';
        }
        std::cout << std::endl;
    }
}
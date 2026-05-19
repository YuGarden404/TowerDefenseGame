#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "src/main/Game.h"

int main()
{
    Game game(1, 1);

    if (!game.getMap().loadFromFile("file/map.txt"))
    {
        std::cout << "Map load failed." << std::endl;
        return 1;
    }

    game.setTotalEnemiesToSpawn(5);

    // 近战塔：必须放在路径上。当前示例 map 第一条路径经过 (5,5)。
    game.placeTower(std::make_unique<MeleeTower>(
        5, 5,
        100, 100,
        10,
        1.0f,
        0.5f,
        0.0f,
        2));

    // 远程塔：必须放在路径外，且与路径上下左右相邻。
    // 如果路径在 y=5，则 (5,4) 通常是合法位置。
    game.placeTower(std::make_unique<RangedTower>(
        8,
        4.0f,
        0.8f,
        5, 4,
        100, 100));

    int frameCount = 0;

    while (!game.isGameOver())
    {
        if (frameCount % 10 == 0)
        {
            game.spawnEnemy();
        }

        game.update(0.1f);
        game.render();

        frameCount++;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    game.render();

    if (game.isVictory())
    {
        std::cout << "Victory!" << std::endl;
    }
    else
    {
        std::cout << "Defeat!" << std::endl;
    }

    return 0;
}
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

    game.placeTower(std::make_unique<MeleeTower>(
        5, 5,
        100, 100,
        10,
        1.0f,
        0.5f,
        0.0f,
        2));

    game.placeTower(std::make_unique<RangedTower>(
        8,
        4.0f,
        0.8f,
        5, 4,
        100, 100));

    bool flag[3] = {false, false, false};
    flag[0] = game.purchaseAffix(5, 4, AffixId::Burn);
    flag[1] = game.purchaseAffix(5, 4, AffixId::Slow);
    flag[2] = game.purchaseAffix(5, 5, AffixId::Berserk);

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
    std::cout << "Buy Burn: " << flag[0] << std::endl;
    std::cout << "Buy Slow: " << flag[1] << std::endl;
    std::cout << "Buy Berserk: " << flag[2] << std::endl;

    return 0;
}
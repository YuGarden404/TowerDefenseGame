#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include <fstream>
#include "src/main/Game.h"

void runInvalidMapTests()
{
    {
        std::ofstream file("file/invalid_diagonal_map.txt");
        file << "SIZE:5 5\n";
        file << "ROAD:0,0 1,1\n";
    }

    {
        std::ofstream file("file/invalid_duplicate_map.txt");
        file << "SIZE:5 5\n";
        file << "ROAD:0,0 1,0 1,0 2,0\n";
    }

    {
        std::ofstream file("file/invalid_rock_map.txt");
        file << "SIZE:5 5\n";
        file << "ROCK:9,9\n";
        file << "ROAD:0,0 1,0\n";
    }

    Map diagonalMap(1, 1);
    Map duplicateMap(1, 1);
    Map rockMap(1, 1);

    std::cout << "Invalid diagonal map should fail: "
              << !diagonalMap.loadFromFile("file/invalid_diagonal_map.txt")
              << std::endl;

    std::cout << "Invalid duplicate map should fail: "
              << !duplicateMap.loadFromFile("file/invalid_duplicate_map.txt")
              << std::endl;

    std::cout << "Invalid rock map should fail: "
              << !rockMap.loadFromFile("file/invalid_rock_map.txt")
              << std::endl;
}

int main()
{
    runInvalidMapTests();
    return 0;
    Game game(1, 1);

    if (!game.getMap().loadFromFile("file/map.txt"))
    {
        std::cout << "Map load failed." << std::endl;
        return 1;
    }

    if (!game.getMap().saveToFile("file/exported_map.txt"))
    {
        std::cout << "Map export failed." << std::endl;
        return 1;
    }

    Map exportedMap(1, 1);
    if (!exportedMap.loadFromFile("file/exported_map.txt"))
    {
        std::cout << "Exported map reload failed." << std::endl;
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

    bool flag[9] = {false, false, false, false, false, false, false, false, false};

    flag[0] = game.purchaseAffix(5, 4, AffixId::Burn);
    flag[1] = game.purchaseAffix(5, 4, AffixId::Burn);
    flag[2] = game.purchaseAffix(5, 4, AffixId::Slow);
    flag[3] = game.purchaseAffix(5, 5, AffixId::Berserk);
    flag[4] = game.purchaseAffix(5, 5, AffixId::Burn);
    flag[5] = game.purchaseAffix(5, 4, AffixId::Berserk);
    flag[6] = game.purchaseAffix(5, 4, AffixId::Blink);
    flag[7] = game.sellAffix(5, 4, AffixId::Slow);
    flag[8] = game.sellAffix(5, 4, AffixId::Slow);

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

    std::cout << "==================================" << std::endl;

    std::cout << "Buy Burn: " << flag[0] << std::endl;
    std::cout << "Buy Burn again: " << flag[1] << std::endl;
    std::cout << "Buy Slow: " << flag[2] << std::endl;
    std::cout << "Buy Berserk: " << flag[3] << std::endl;

    std::cout << "==================================" << std::endl;

    std::cout << "Wrong Burn on melee: " << flag[4] << std::endl;
    std::cout << "Wrong Berserk on ranged: " << flag[5] << std::endl;
    std::cout << "Wrong Blink on tower: " << flag[6] << std::endl;

    std::cout << "==================================" << std::endl;

    std::cout << "Sell Slow: " << flag[7] << std::endl;
    std::cout << "Sell Slow again: " << flag[8] << std::endl;

    return 0;
}
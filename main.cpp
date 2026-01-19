#include <iostream>

#include "src/main/Game.h"

int main()
{
    if(Game game(1,1); game.getMap().loadFromFile("../file/map.txt"))
    {
        game.placeTower(std::make_unique<RangedTower>(5, 10, 0.5f, 4, 7, 10, 10));
        game.placeTower(std::make_unique<MeleeTower>(5, 5, 100, 100, 10, 1.0f, 0.5f, 0, 5));
        int frameCount = 0; //帧计数器
        while(true)
        {
            if(frameCount%10==0) game.spawnEnemy();
            game.update(0.1f);
            game.render();
            frameCount++;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    return 0;
}

#include <iostream>

#include "src/main/Game.h"

int main()
{
    Game game(1,1);
    if(game.getMap().loadFromFile("../file/map.txt"))
    {
        game.addEntity(std::make_unique<RangedTower>(30,10,0.5f,4,7,10,10));
        game.addEntity(std::make_unique<RangedTower>(30,10,0.5f,12,7,10,10));
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

#include <iostream>

#include "src/main/Game.h"

int main()
{
    Game game(20,10);
    for(int i = 0;i<20;i++)
    {
        game.getMap().setTileType(i,5,TileType::PATH);
        if(i==0) game.getMap().setTileType(i,5,TileType::START);
        if(i==19) game.getMap().setTileType(i,5,TileType::END);
        game.getMap().addPathPoint(i,5);
    }
    game.addEntity(std::make_unique<RangedTower>(1000,10,0.5f,4,7,10,10));
    game.addEntity(std::make_unique<RangedTower>(1000,10,0.5f,12,7,10,10));
    int frameCount = 0; //帧计数器
    while(true)
    {
        if(frameCount%10==0) game.spawnEnemy();
        game.update(0.1f);
        game.render();
        frameCount++;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return 0;
}

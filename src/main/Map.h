//
// Created by Lenovo on 26-1-12.
//

#ifndef MAP_H
#define MAP_H

#include <vector>
#include <iostream>
#include <fstream>
#include <ranges>
#include <sstream>


enum class TileType
{
    GROUND, //地面，可放远程塔
    PATH,   //路径，可放近战塔及敌人通过
    START,  //起点
    END,    //终点
    ERROR,  //错误(仅软件调试阶段使用)
};

struct Point
{
    int x, y;
    Point():x(0),y(0){}
    Point(const int x, const int y):x(x),y(y){}
};

class Map {
    //成员变量
    int width,height;   //地图宽高
    std::vector<std::vector<TileType>>grid; //地图
    std::vector<Point> enemyPath;   //敌人移动的有序坐标序列

    //私有成员函数
    [[nodiscard]] bool isValidPos(int x,int y) const;
    static std::string TileTypeToString(TileType type);
public:
    //公共成员函数
    //getter
    [[nodiscard]] int getWidth() const {return width;}
    [[nodiscard]] int getHeight() const {return height;}
    //获取指定位置的地块类型，需包含边界检查
    [[nodiscard]] TileType getTileType(int x, int y) const;
    //构造函数
    Map(int w,int h);
    //设置指定位置的地块类型
    void setTileType(int x, int y, TileType type);
    //向 enemyPath 中添加一个路径点
    void addPathPoint(int x, int y);
    //返回路径序列供敌人使用
    [[nodiscard]] const std::vector<Point>& getEnemyPath() const;
    bool loadFromFile(const std::string& filename);
};



#endif //MAP_H

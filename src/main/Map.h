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
#include <string>

enum class TileType
{
    GROUND,     // 地面，可放远程塔
    PATH,       // 路径，可放近战塔及敌人通过
    START,      // 起点
    END,        // 终点
    TILE_ERROR, // 错误(仅软件调试阶段使用)
};

struct Point
{
    int x, y;
    Point() : x(0), y(0) {}
    Point(const int x, const int y) : x(x), y(y) {}
    bool operator==(const Point &other) const
    {
        return x == other.x && y == other.y;
    }
};

class Map
{
    // 成员变量
    int width, height;                          // 地图宽高
    std::vector<std::vector<TileType>> grid;    // 地图
    std::vector<std::vector<Point>> enemyPaths; // 敌人移动的有序坐标序列

    // 私有成员函数
    [[nodiscard]] bool isValidPos(int x, int y) const;
    static std::string TileTypeToString(TileType type);
    static bool areAdjacent(const Point &a, const Point &b);
    static bool hasDuplicatePoint(const std::vector<Point> &path);

public:
    // 公共成员函数
    // getter
    [[nodiscard]] int getWidth() const { return width; }
    [[nodiscard]] int getHeight() const { return height; }
    // 获取指定位置的地块类型，需包含边界检查
    [[nodiscard]] TileType getTileType(int x, int y) const;
    // 构造函数
    Map(int w, int h);
    // 设置指定位置的地块类型
    void setTileType(int x, int y, TileType type);
    bool addPath(const std::vector<Point> &path);

    [[nodiscard]] const std::vector<Point> &getEnemyPath() const;
    [[nodiscard]] const std::vector<Point> &getEnemyPath(size_t pathId) const;
    [[nodiscard]] const std::vector<std::vector<Point>> &getEnemyPaths() const;
    [[nodiscard]] size_t getPathCount() const;

    [[nodiscard]] bool isPathTile(int x, int y) const;
    [[nodiscard]] bool isAdjacentToPath(int x, int y) const;
    [[nodiscard]] bool canPlaceMeleeTower(int x, int y) const;
    [[nodiscard]] bool canPlaceRangedTower(int x, int y) const;
    bool loadFromFile(const std::string &filename);
};

#endif // MAP_H

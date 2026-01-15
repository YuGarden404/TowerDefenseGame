//
// Created by Lenovo on 26-1-12.
//

#include "Map.h"

bool Map::isValidPos(const int x, const int y) const
{
    return x>=0&&x<width&&y>=0&&y<height;
}

std::string Map::TileTypeToString(const TileType type)
{
    switch (type)
    {
        case TileType::GROUND:return "GROUND";
        case TileType::PATH:return "PATH";
        case TileType::START:return "START";
        case TileType::END:return "END";
        default:return "ERROR";
    }
}


Map::Map(const int w, const int h)
{
    //默认是空地
    if(w<0||h<0)
    {
        std::cout << "地图构造参数有误：" << std::endl;
        if(w<0)
        {
            std::cout << "width = " << w << std::endl;
        }
        if(h<0)
        {
            std::cout << "height = " << h << std::endl;
        }
        std::cout << "默认构造：width = 0,height = 0" << std::endl;
        width = 0, height = 0;
        grid.clear();
        enemyPath.clear();
    }else
    {
        width = w, height = h;
        grid.resize(height,std::vector(width,TileType::GROUND));
        enemyPath.clear();
    }
}

TileType Map::getTileType(const int x, const int y) const
{
    if(isValidPos(x,y))
    {
        return grid[y][x];
    }
    std::cout << "获取地图(" << x << "," << y << ")信息有误，非法索引" << std::endl;
    return TileType::ERROR;
}

void Map::setTileType(const int x, const int y, const TileType type)
{
    if(isValidPos(x,y))
    {
        std::cout << "修改地图(" << x << "," << y << ")成功，由" << TileTypeToString(grid[y][x]) << "修改为" << TileTypeToString(type) << std::endl;
        grid[y][x] = type;
    }else std::cout << "获取地图(" << x << "," << y << ")信息有误，非法索引" << std::endl;
}

void Map::addPathPoint(const int x, const int y)
{
    if(isValidPos(x,y))
    {
        enemyPath.emplace_back(x,y);
        std::cout << "添加路径(" << x << "," << y << ")成功" << std::endl;
    }else std::cout << "添加路径(" << x << "," << y << ")信息有误，非法索引" << std::endl;
}

const std::vector<Point>& Map::getEnemyPath() const
{
    return enemyPath;
}

bool Map::loadFromFile(const std::string& filename)
{
    std::ifstream file(filename);
    if(!file.is_open())
    {
        std::cout << "地图文件打开错误" << std::endl;
        return false;
    }
    enemyPath.clear();
    grid.clear();

    std::string line;
    while(std::getline(file,line))
    {
        if(line.empty()||line.starts_with("#"))continue;
        if(line.starts_with("SIZE:"))
        {
            std::stringstream ss(line.substr(5));
            ss >> width >> height;
            grid.resize(height, std::vector(width, TileType::GROUND));
        }else if(line.starts_with("PATH:"))
        {
            std::stringstream ss(line.substr(5));
            std::string pointStr;
            while(ss >> pointStr)
            {
                if(size_t pos = pointStr.find(','); pos!=std::string::npos)
                {
                    int x = std::stoi(pointStr.substr(0,pos));
                    int y = std::stoi(pointStr.substr(pos+1));

                    setTileType(x,y,TileType::PATH);
                    addPathPoint(x,y);
                }
            }
        }
    }
    if(!enemyPath.empty())
    {
        setTileType(enemyPath.front().x,enemyPath.front().y,TileType::START);
        setTileType(enemyPath.back().x,enemyPath.back().y,TileType::END);
    }
    file.close();
    std::cout << "地图加载成功" << std::endl;
    return true;
}
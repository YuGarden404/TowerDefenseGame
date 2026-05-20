//
// Created by Lenovo on 26-1-12.
//

#include "Map.h"
#include <cmath>
#include "../../third_party/nlohmann/json.hpp"

using json = nlohmann::json;

bool Map::isValidPos(const int x, const int y) const
{
    return x >= 0 && x < width && y >= 0 && y < height;
}

std::string Map::TileTypeToString(const TileType type)
{
    switch (type)
    {
    case TileType::GRASS:
        return "GRASS";
    case TileType::ROAD:
        return "ROAD";
    case TileType::ROCK:
        return "ROCK";
    case TileType::START:
        return "START";
    case TileType::END:
        return "END";
    default:
        return "ERROR";
    }
}

bool Map::areAdjacent(const Point &a, const Point &b)
{
    return std::abs(a.x - b.x) + std::abs(a.y - b.y) == 1;
}

bool Map::hasDuplicatePoint(const std::vector<Point> &path)
{
    for (size_t i = 0; i < path.size(); ++i)
    {
        for (size_t j = i + 1; j < path.size(); ++j)
        {
            if (path[i] == path[j])
            {
                return true;
            }
        }
    }
    return false;
}

Map::Map(const int w, const int h)
{
    // 默认是空地
    if (w <= 0 || h <= 0)
    {
        width = 0, height = 0;
        grid.clear();
        enemyPaths.clear();
        return;
    }
    width = w, height = h;
    grid.assign(height, std::vector(width, TileType::GRASS));
    enemyPaths.clear();
}

TileType Map::getTileType(const int x, const int y) const
{
    if (isValidPos(x, y))
    {
        return grid[y][x];
    }
    return TileType::TILE_ERROR;
}

void Map::setTileType(const int x, const int y, const TileType type)
{
    if (isValidPos(x, y))
    {
        grid[y][x] = type;
    }
}

bool Map::addPath(const std::vector<Point> &path)
{
    if (path.size() < 2)
    {
        std::cout << "Add path failed: path must contain at least 2 points." << std::endl;
        return false;
    }

    for (const auto &point : path)
    {
        if (!isValidPos(point.x, point.y))
        {
            std::cout << "Add path failed: point out of map." << std::endl;
            return false;
        }
    }

    if (hasDuplicatePoint(path))
    {
        std::cout << "Add path failed: path contains duplicate points." << std::endl;
        return false;
    }

    for (size_t i = 1; i < path.size(); ++i)
    {
        if (!areAdjacent(path[i - 1], path[i]))
        {
            std::cout << "Add path failed: adjacent path points must be 4-neighbor cells." << std::endl;
            return false;
        }
    }

    enemyPaths.push_back(path);

    for (const auto &point : path)
    {
        TileType current = getTileType(point.x, point.y);
        if (current != TileType::START && current != TileType::END)
        {
            setTileType(point.x, point.y, TileType::ROAD);
        }
    }

    setTileType(path.front().x, path.front().y, TileType::START);
    setTileType(path.back().x, path.back().y, TileType::END);

    return true;
}

const std::vector<Point> &Map::getEnemyPath() const
{
    static const std::vector<Point> emptyPath;
    if (enemyPaths.empty())
    {
        return emptyPath;
    }
    return enemyPaths.front();
}

const std::vector<Point> &Map::getEnemyPath(const size_t pathId) const
{
    static const std::vector<Point> emptyPath;
    if (pathId >= enemyPaths.size())
    {
        return emptyPath;
    }
    return enemyPaths[pathId];
}

const std::vector<std::vector<Point>> &Map::getEnemyPaths() const
{
    return enemyPaths;
}

size_t Map::getPathCount() const
{
    return enemyPaths.size();
}

bool Map::isPathTile(const int x, const int y) const
{
    const TileType type = getTileType(x, y);
    return type == TileType::ROAD || type == TileType::START || type == TileType::END;
}

bool Map::isAdjacentToPath(const int x, const int y) const
{
    static constexpr int dx[4] = {1, -1, 0, 0};
    static constexpr int dy[4] = {0, 0, 1, -1};

    for (int i = 0; i < 4; ++i)
    {
        if (isPathTile(x + dx[i], y + dy[i]))
        {
            return true;
        }
    }

    return false;
}

bool Map::canPlaceMeleeTower(const int x, const int y) const
{
    // return isPathTile(x, y);
    return getTileType(x, y) == TileType::ROAD;
}

bool Map::canPlaceRangedTower(const int x, const int y) const
{
    return getTileType(x, y) == TileType::GRASS && isAdjacentToPath(x, y);
}

bool Map::loadFromFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cout << "Map file open failed." << std::endl;
        return false;
    }

    width = 0;
    height = 0;
    grid.clear();
    enemyPaths.clear();

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line.starts_with("#"))
        {
            continue;
        }

        if (line.starts_with("SIZE:"))
        {
            std::stringstream ss(line.substr(5));
            ss >> width >> height;
            if (width <= 0 || height <= 0)
            {
                std::cout << "Invalid map size." << std::endl;
                return false;
            }
            grid.assign(height, std::vector<TileType>(width, TileType::GRASS));
        }
        else if (line.starts_with("ROAD:"))
        {
            if (grid.empty())
            {
                std::cout << "ROAD appears before SIZE." << std::endl;
                return false;
            }

            std::stringstream ss(line.substr(5));
            std::string pointStr;
            std::vector<Point> path;

            while (ss >> pointStr)
            {
                const size_t pos = pointStr.find(',');
                if (pos == std::string::npos)
                {
                    std::cout << "Invalid point format." << std::endl;
                    return false;
                }

                const int x = std::stoi(pointStr.substr(0, pos));
                const int y = std::stoi(pointStr.substr(pos + 1));
                path.emplace_back(x, y);
            }

            if (!addPath(path))
            {
                return false;
            }
        }
        else if (line.starts_with("ROCK:"))
        {
            if (grid.empty())
            {
                std::cout << "ROCK appears before SIZE." << std::endl;
                return false;
            }

            std::stringstream ss(line.substr(5));
            std::string pointStr;

            while (ss >> pointStr)
            {
                const size_t pos = pointStr.find(',');
                if (pos == std::string::npos)
                {
                    std::cout << "Invalid point format." << std::endl;
                    return false;
                }

                const int x = std::stoi(pointStr.substr(0, pos));
                const int y = std::stoi(pointStr.substr(pos + 1));
                if (!isValidPos(x, y))
                {
                    std::cout << "Invalid rock position." << std::endl;
                    return false;
                }
                addRock(x, y);
            }
        }
        else
        {
            std::cout << "Unknown line type." << std::endl;
            return false;
        }
    }

    return validate();
}

bool Map::saveToFile(const std::string &filename) const
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        std::cout << "Failed to open file for saving." << std::endl;
        return false;
    }

    file << "SIZE:" << width << " " << height << std::endl;

    bool hasRock = false;
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            if (grid[y][x] == TileType::ROCK)
            {
                hasRock = true;
            }
        }
    }

    if (hasRock)
    {
        file << "ROCK:";
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                if (grid[y][x] == TileType::ROCK)
                {
                    file << x << "," << y << " ";
                }
            }
        }
        file << std::endl;
    }

    for (const auto &path : enemyPaths)
    {
        file << "ROAD:";
        for (const auto &point : path)
        {
            file << point.x << "," << point.y << " ";
        }
        file << std::endl;
    }

    return true;
}

bool Map::addRock(int x, int y)
{
    if (!isValidPos(x, y))
    {
        return false;
    }

    if (isPathTile(x, y))
    {
        return false;
    }

    grid[y][x] = TileType::ROCK;
    return true;
}

bool Map::setGrass(int x, int y)
{
    if (!isValidPos(x, y))
    {
        return false;
    }

    if (isPathTile(x, y))
    {
        return false;
    }

    grid[y][x] = TileType::GRASS;
    return true;
}

bool Map::removeRock(int x, int y)
{
    if (!isValidPos(x, y))
    {
        return false;
    }

    if (grid[y][x] != TileType::ROCK)
    {
        return false;
    }

    grid[y][x] = TileType::GRASS;
    return true;
}

bool Map::validate() const
{
    if (width <= 0 || height <= 0 || grid.empty())
    {
        return false;
    }

    if (enemyPaths.empty())
    {
        return false;
    }

    for (const auto &path : enemyPaths)
    {
        if (path.size() < 2)
        {
            return false;
        }

        if (hasDuplicatePoint(path))
        {
            return false;
        }

        for (const auto &point : path)
        {
            if (!isValidPos(point.x, point.y))
            {
                return false;
            }
        }

        for (size_t i = 1; i < path.size(); ++i)
        {
            if (!areAdjacent(path[i - 1], path[i]))
            {
                return false;
            }
        }
    }

    return true;
}

bool Map::loadFromJsonFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cout << "JSON map file open failed." << std::endl;
        return false;
    }

    json data;
    try
    {
        file >> data;
    }
    catch (const std::exception &e)
    {
        std::cout << "JSON parse failed: " << e.what() << std::endl;
        return false;
    }

    if (!data.contains("width") || !data.contains("height") || !data.contains("roads"))
    {
        std::cout << "JSON map missing required fields." << std::endl;
        return false;
    }

    width = data["width"].get<int>();
    height = data["height"].get<int>();

    if (width <= 0 || height <= 0)
    {
        std::cout << "Invalid JSON map size." << std::endl;
        return false;
    }

    grid.assign(height, std::vector<TileType>(width, TileType::GRASS));
    enemyPaths.clear();

    if (data.contains("rocks"))
    {
        for (const auto &rock : data["rocks"])
        {
            if (!rock.is_array() || rock.size() != 2)
            {
                std::cout << "Invalid rock format in JSON." << std::endl;
                return false;
            }

            int x = rock[0].get<int>();
            int y = rock[1].get<int>();

            if (!isValidPos(x, y))
            {
                std::cout << "Invalid rock position in JSON." << std::endl;
                return false;
            }

            addRock(x, y);
        }
    }

    for (const auto &road : data["roads"])
    {
        if (!road.is_array())
        {
            std::cout << "Invalid road format in JSON." << std::endl;
            return false;
        }

        std::vector<Point> path;

        for (const auto &point : road)
        {
            if (!point.is_array() || point.size() != 2)
            {
                std::cout << "Invalid road point format in JSON." << std::endl;
                return false;
            }

            int x = point[0].get<int>();
            int y = point[1].get<int>();
            path.emplace_back(x, y);
        }

        if (!addPath(path))
        {
            return false;
        }
    }

    return validate();
}

bool Map::saveToJsonFile(const std::string &filename) const
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        std::cout << "Failed to open JSON map file for saving." << std::endl;
        return false;
    }

    json data;
    data["width"] = width;
    data["height"] = height;
    data["rocks"] = json::array();
    data["roads"] = json::array();

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            if (grid[y][x] == TileType::ROCK)
            {
                data["rocks"].push_back({x, y});
            }
        }
    }

    for (const auto &path : enemyPaths)
    {
        json road = json::array();

        for (const auto &point : path)
        {
            road.push_back({point.x, point.y});
        }

        data["roads"].push_back(road);
    }

    file << data.dump(4) << std::endl;
    return true;
}
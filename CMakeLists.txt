cmake_minimum_required(VERSION 3.29)
project(TowerDefense)

set(CMAKE_CXX_STANDARD 20)

add_executable(TowerDefense main.cpp
        src/entities/Enemy.cpp
        src/entities/Enemy.h
        src/entities/Entity.cpp
        src/entities/Entity.h
        src/entities/Tower.cpp
        src/entities/Tower.h
        src/entities/MeleeTower.cpp
        src/entities/MeleeTower.h
        src/entities/RangedTower.cpp
        src/entities/RangedTower.h
        src/components/Affix.h
        src/main/Game.cpp
        src/main/Game.h
        src/main/Map.cpp
        src/main/Map.h
        src/components/SlowAffix.cpp
        src/components/SlowAffix.h
        src/components/SlowAffix.h
        src/components/BurnAffix.cpp
        src/components/BurnAffix.h
        src/components/BurnAffix.h)

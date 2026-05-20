cmake_minimum_required(VERSION 3.29)
project(TowerDefense)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt6 REQUIRED COMPONENTS Widgets)


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
        src/main/EnemyFactory.cpp
        src/main/EnemyFactory.h
        src/components/SlowAffix.cpp
        src/components/SlowAffix.h
        src/components/SlowAffix.h
        src/components/BurnAffix.cpp
        src/components/BurnAffix.h
        src/components/BurnAffix.h
        src/components/AffixTypes.cpp
        src/components/AffixTypes.h
        src/render/ConsoleRenderer.h
        src/render/ConsoleRenderer.cpp
        src/view/GameView.h
        src/qt/MainWindow.cpp
        src/qt/MainWindow.h
        src/qt/GameWidget.cpp
        src/qt/GameWidget.h
        )


target_link_libraries(TowerDefense PRIVATE Qt6::Widgets)

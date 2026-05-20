#include "MainWindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      game(1, 1),
      gameWidget(new GameWidget(game, this)),
      timer(new QTimer(this))
{
    initializeGame();

    setCentralWidget(gameWidget);
    setWindowTitle("Tower Defense");
    resize(900, 560);

    connect(timer, &QTimer::timeout, this, [this]()
            {
                if (!game.isGameOver())
                {
                    game.spawnEnemy();
                    game.update(0.05f);
                    gameWidget->update();
                } });

    timer->start(50);
}

void MainWindow::initializeGame()
{
    if (!game.getMap().loadFromJsonFile("file/map.json"))
    {
        QMessageBox::critical(this, "Error", "Map load failed: file/map.json");
        return;
    }

    game.setTotalEnemiesToSpawn(5);

    game.placeMeleeTowerAt(5, 5);
    game.placeRangedTowerAt(5, 4);

    game.buyAffixAt(5, 4, AffixId::Burn);
    game.buyAffixAt(5, 4, AffixId::Slow);
    game.buyAffixAt(5, 5, AffixId::Berserk);
}
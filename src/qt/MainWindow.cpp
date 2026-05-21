#include "MainWindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      game(1, 1),
      gameWidget(new GameWidget(game, this)),
      timer(new QTimer(this)),
      spawnTimer(0.0f),
      spawnInterval(1.0f),
      waveStarted(false)
{
    initializeGame();

    setCentralWidget(gameWidget);
    setWindowTitle("Tower Defense");
    resize(1120, 620);

    setupConnections();

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
    gameWidget->setWaveStarted(waveStarted);
}

void MainWindow::setupConnections()
{
    connect(timer, &QTimer::timeout, this, [this]()
            {
                constexpr float deltaTime = 0.05f;

                if (!game.isGameOver() && !game.isPaused() && waveStarted)
                {
                    spawnTimer += deltaTime;

                    if (spawnTimer >= spawnInterval)
                    {
                        game.spawnEnemy();
                        spawnTimer = 0.0f;
                    }

                    game.update(deltaTime);
                }

                gameWidget->update(); });

    connect(gameWidget, &GameWidget::startWaveRequested, this, &MainWindow::handleStartWave);
    connect(gameWidget, &GameWidget::pauseRequested, this, &MainWindow::handlePause);
    connect(gameWidget, &GameWidget::resumeRequested, this, &MainWindow::handleResume);
    connect(gameWidget, &GameWidget::resetRequested, this, &MainWindow::handleReset);
}

void MainWindow::handleStartWave()
{
    if (game.isGameOver() || waveStarted)
    {
        return;
    }

    waveStarted = true;
    spawnTimer = spawnInterval;
    gameWidget->setWaveStarted(true);
    gameWidget->update();
}

void MainWindow::handlePause()
{
    if (game.isGameOver())
    {
        return;
    }

    game.setPaused(true);
    gameWidget->update();
}

void MainWindow::handleResume()
{
    game.setPaused(false);
    gameWidget->update();
}

void MainWindow::handleReset()
{
    game.reset();

    waveStarted = false;
    spawnTimer = 0.0f;
    game.setPaused(false);

    initializeGame();

    gameWidget->setWaveStarted(false);
    gameWidget->clearSelection();
    gameWidget->update();
}
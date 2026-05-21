#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../main/Game.h"
#include "GameWidget.h"

#include <QMainWindow>
#include <QTimer>

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    Game game;
    GameWidget *gameWidget;
    QTimer *timer;

    float spawnTimer;
    float spawnInterval;
    bool waveStarted;

    void initializeGame();
    void setupConnections();

    void handleStartWave();
    void handlePause();
    void handleResume();
    void handleReset();
};

#endif // MAINWINDOW_H
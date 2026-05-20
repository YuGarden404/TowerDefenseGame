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

    void initializeGame();
};

#endif // MAINWINDOW_H
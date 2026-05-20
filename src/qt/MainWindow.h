#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../main/Game.h"
#include "GameWidget.h"

#include <QLabel>
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
    QLabel *selectedInfoLabel;

    void initializeGame();
    void setupUi();
    void updateSelectedInfo(int x, int y);

    [[nodiscard]] QString tileTypeToText(TileType type) const;
    [[nodiscard]] QString entityKindToText(EntityKind kind) const;
    [[nodiscard]] QString affixIdToText(AffixId affixId) const;
};

#endif // MAINWINDOW_H
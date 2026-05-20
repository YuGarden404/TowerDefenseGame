#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../main/Game.h"
#include "GameWidget.h"

#include <QLabel>
#include <QMainWindow>
#include <QTimer>
#include <QPushButton>

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    Game game;
    GameWidget *gameWidget;
    QTimer *timer;

    QLabel *selectedInfoLabel;

    QPushButton *placeMeleeButton;
    QPushButton *placeRangedButton;

    QPushButton *buyBurnButton;
    QPushButton *buySlowButton;
    QPushButton *buyBerserkButton;

    QPushButton *sellBurnButton;
    QPushButton *sellSlowButton;
    QPushButton *sellBerserkButton;

    QPushButton *pauseButton;
    QPushButton *resetButton;

    QLabel *messageLabel;
    QPushButton *sellTowerButton;

    void initializeGame();
    void setupUi();
    void setupConnections();

    void updateSelectedInfo(int x, int y);
    void refreshSelectedInfo();

    [[nodiscard]] bool hasSelectedCell() const;
    [[nodiscard]] int selectedX() const;
    [[nodiscard]] int selectedY() const;

    void handlePlaceMelee();
    void handlePlaceRanged();

    void handleBuyAffix(AffixId affixId);
    void handleSellAffix(AffixId affixId);

    void handleTogglePause();
    void handleReset();

    [[nodiscard]] QString tileTypeToText(TileType type) const;
    [[nodiscard]] QString entityKindToText(EntityKind kind) const;
    [[nodiscard]] QString affixIdToText(AffixId affixId) const;

    void updateMessageLabel();
    void handleSellTower();
};

#endif // MAINWINDOW_H
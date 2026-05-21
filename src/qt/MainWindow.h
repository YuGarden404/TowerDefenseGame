#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../main/Game.h"
#include "GameWidget.h"

#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QString>
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
    QLabel *messageLabel;

    QPushButton *placeMeleeButton;
    QPushButton *placeRangedButton;
    QPushButton *sellTowerButton;

    QPushButton *buyBurnButton;
    QPushButton *buySlowButton;
    QPushButton *buyBerserkButton;

    QPushButton *sellBurnButton;
    QPushButton *sellSlowButton;
    QPushButton *sellBerserkButton;

    QPushButton *pauseButton;
    QPushButton *resetButton;

    QPushButton *startWaveButton;

    float spawnTimer;
    float spawnInterval;
    bool waveStarted;

    void initializeGame();
    void setupUi();
    void setupConnections();

    void updateSelectedInfo(int x, int y);
    void refreshSelectedInfo();
    void updateMessageLabel();
    void refreshActionButtons();

    [[nodiscard]] bool hasSelectedCell() const;
    [[nodiscard]] int selectedX() const;
    [[nodiscard]] int selectedY() const;

    [[nodiscard]] EntityView selectedEntity() const;
    [[nodiscard]] bool selectedEntityHasAffix(AffixId affixId) const;
    [[nodiscard]] bool selectedEntityCanBuyAffix(AffixId affixId) const;

    void handlePlaceMelee();
    void handlePlaceRanged();
    void handleSellTower();

    void handleBuyAffix(AffixId affixId);
    void handleSellAffix(AffixId affixId);

    void handleTogglePause();
    void handleReset();

    [[nodiscard]] QString tileTypeToText(TileType type) const;
    [[nodiscard]] QString entityKindToText(EntityKind kind) const;
    [[nodiscard]] QString affixIdToText(AffixId affixId) const;

    void handleStartWave();
};

#endif // MAINWINDOW_H
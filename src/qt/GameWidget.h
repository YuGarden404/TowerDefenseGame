#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include "../main/Game.h"

#include <QPainter>
#include <QRect>
#include <QWidget>

enum class UiMode
{
    Normal,
    BuildPreview,
    PauseMenu
};

enum class BuildChoice
{
    None,
    MeleeTower,
    RangedTower
};

enum class AffixChoice
{
    Burn,
    Slow,
    Berserk
};

class GameWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GameWidget(Game &game, QWidget *parent = nullptr);

    void setSelectedCell(int x, int y);
    void clearSelection();

    void setWaveStarted(bool started);

    [[nodiscard]] int getSelectedX() const { return selectedX; }
    [[nodiscard]] int getSelectedY() const { return selectedY; }

signals:
    void cellClicked(int x, int y);
    void startWaveRequested();
    void pauseRequested();
    void resumeRequested();
    void resetRequested();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    Game &game;

    int tileSize = 40;
    int mapOffsetX = 20;
    int mapOffsetY = 56;

    int selectedX = -1;
    int selectedY = -1;

    UiMode uiMode = UiMode::Normal;
    BuildChoice buildChoice = BuildChoice::None;
    bool waveStarted = false;

    void drawHud(QPainter &painter);
    void drawTopButtons(QPainter &painter);
    void drawMap(QPainter &painter);
    void drawEntities(QPainter &painter);
    void drawSelection(QPainter &painter);
    void drawBottomHint(QPainter &painter);
    void drawPauseOverlay(QPainter &painter);
    void drawGameOverOverlay(QPainter &painter);

    void drawBuildCards(QPainter &painter);
    void drawBuildCard(QPainter &painter, const QRect &rect, BuildChoice choice, bool enabled);
    void drawBuildPreview(QPainter &painter);
    void drawTowerActions(QPainter &painter);
    void drawAffixCards(QPainter &painter);
    void drawAffixCard(QPainter &painter, const QRect &rect, AffixChoice choice, bool enabled, bool equipped);
    void drawBuildGhost(QPainter &painter, const QRect &cell);
    void drawAttackRange(QPainter &painter);
    void drawSelectedTowerRange(QPainter &painter);
    void drawEquippedAffixBadges(QPainter &painter, const EntityView &entity, int centerX, int centerY);

    void handleMapClick(int pixelX, int pixelY);
    void handleTopButtonClick(int pixelX, int pixelY);
    void handlePauseOverlayClick(int pixelX, int pixelY);
    void handleGameOverOverlayClick(int pixelX, int pixelY);
    void handleBuildCardClick(int pixelX, int pixelY);
    void handleBuildPreviewClick(int pixelX, int pixelY);
    void handleTowerActionClick(int pixelX, int pixelY);

    void confirmBuild();
    void cancelBuild();

    [[nodiscard]] EntityView selectedEntity() const;
    [[nodiscard]] bool selectedEntityIsTower() const;
    [[nodiscard]] bool selectedEntityHasAffix(AffixId affixId) const;
    [[nodiscard]] bool selectedEntityCanBuyAffix(AffixId affixId) const;

    [[nodiscard]] bool isInsideMapPixel(int pixelX, int pixelY) const;
    [[nodiscard]] bool hasSelectedCell() const;
    [[nodiscard]] bool selectedCellIsEmpty() const;
    [[nodiscard]] bool selectedCellCanShowBuildCards() const;
    [[nodiscard]] bool selectedCellCanPlaceMelee() const;
    [[nodiscard]] bool selectedCellCanPlaceRanged() const;
    [[nodiscard]] float selectedBuildRange() const;
    [[nodiscard]] float selectedTowerRange() const;
    [[nodiscard]] QString equippedAffixesText(const EntityView &entity) const;
    [[nodiscard]] QString activeAffixesText(const EntityView &entity) const;

    [[nodiscard]] QRect mapCellRect(int x, int y) const;
    [[nodiscard]] QRect pauseButtonRect() const;
    [[nodiscard]] QRect startWaveButtonRect() const;
    [[nodiscard]] QRect pauseOverlayRect() const;
    [[nodiscard]] QRect resumeButtonRect() const;
    [[nodiscard]] QRect resetButtonRect() const;
    [[nodiscard]] QRect closePauseButtonRect() const;
    [[nodiscard]] QRect gameOverPanelRect() const;
    [[nodiscard]] QRect gameOverResetButtonRect() const;

    [[nodiscard]] QRect meleeCardRect() const;
    [[nodiscard]] QRect rangedCardRect() const;
    [[nodiscard]] QRect confirmBuildRect() const;
    [[nodiscard]] QRect cancelBuildRect() const;

    [[nodiscard]] QRect selectedTowerSellRect() const;
    [[nodiscard]] QRect burnAffixCardRect() const;
    [[nodiscard]] QRect slowAffixCardRect() const;
    [[nodiscard]] QRect berserkAffixCardRect() const;
};

#endif // GAMEWIDGET_H
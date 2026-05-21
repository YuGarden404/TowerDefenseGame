#include "GameWidget.h"

#include <QBrush>
#include <QColor>
#include <QFont>
#include <QMouseEvent>
#include <QPen>

#include <algorithm>

namespace
{
    constexpr int CardStartX = 24;
    constexpr int CardBottomMargin = 136;
    constexpr int CardWidth = 128;
    constexpr int CardHeight = 82;
    constexpr int CardGap = 12;
    constexpr int TwoCardHitWidth = CardWidth * 2 + CardGap;

    bool hasActiveAffix(const EntityView &entity, const std::string &name)
    {
        return std::find(entity.activeAffixNames.begin(),
                         entity.activeAffixNames.end(),
                         name) != entity.activeAffixNames.end();
    }
}

GameWidget::GameWidget(Game &game, QWidget *parent)
    : QWidget(parent), game(game)
{
    setMinimumSize(920, 560);
}

void GameWidget::setSelectedCell(int x, int y)
{
    const Map &map = game.getMap();

    if (x < 0 || y < 0 || x >= map.getWidth() || y >= map.getHeight())
    {
        clearSelection();
        return;
    }

    selectedX = x;
    selectedY = y;
    buildChoice = BuildChoice::None;
    uiMode = UiMode::Normal;

    update();
}

void GameWidget::clearSelection()
{
    selectedX = -1;
    selectedY = -1;
    buildChoice = BuildChoice::None;
    uiMode = UiMode::Normal;
    update();
}

void GameWidget::setWaveStarted(bool started)
{
    waveStarted = started;
    update();
}

void GameWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    painter.fillRect(rect(), QColor(238, 241, 245));

    painter.setRenderHint(QPainter::Antialiasing, false);
    drawHud(painter);
    drawTopButtons(painter);
    drawMap(painter);

    if (uiMode == UiMode::BuildPreview)
    {
        drawAttackRange(painter);
    }
    else
    {
        drawSelectedTowerRange(painter);
    }

    drawSelection(painter);

    painter.setRenderHint(QPainter::Antialiasing, true);
    drawEntities(painter);

    if (uiMode == UiMode::BuildPreview)
    {
        drawBuildPreview(painter);
    }
    else if (selectedCellCanShowBuildCards())
    {
        drawBuildCards(painter);
    }
    else if (selectedEntityIsTower())
    {
        drawTowerActions(painter);
        drawAffixCards(painter);
    }

    drawBottomHint(painter);

    if (game.isGameOver())
    {
        drawGameOverOverlay(painter);
    }
    else if (game.isPaused())
    {
        drawPauseOverlay(painter);
    }
}

void GameWidget::mousePressEvent(QMouseEvent *event)
{
    const int px = static_cast<int>(event->position().x());
    const int py = static_cast<int>(event->position().y());

    if (game.isGameOver())
    {
        handleGameOverOverlayClick(px, py);
        return;
    }

    if (game.isPaused())
    {
        handlePauseOverlayClick(px, py);
        return;
    }

    if (uiMode == UiMode::BuildPreview)
    {
        handleBuildPreviewClick(px, py);
        return;
    }

    if (selectedEntityIsTower() &&
        (selectedTowerSellRect().contains(px, py) ||
         QRect(CardStartX, height() - CardBottomMargin, TwoCardHitWidth, CardHeight).contains(px, py)))
    {
        handleTowerActionClick(px, py);
        return;
    }

    if (selectedCellCanShowBuildCards() &&
        QRect(CardStartX, height() - CardBottomMargin, TwoCardHitWidth, CardHeight).contains(px, py))
    {
        handleBuildCardClick(px, py);
        return;
    }

    if (pauseButtonRect().contains(px, py) || startWaveButtonRect().contains(px, py))
    {
        handleTopButtonClick(px, py);
        return;
    }

    if (isInsideMapPixel(px, py))
    {
        handleMapClick(px, py);
        return;
    }

    clearSelection();
}

void GameWidget::handleTopButtonClick(int pixelX, int pixelY)
{
    if (pauseButtonRect().contains(pixelX, pixelY))
    {
        emit pauseRequested();
        return;
    }

    if (!waveStarted && !game.isGameOver() && startWaveButtonRect().contains(pixelX, pixelY))
    {
        emit startWaveRequested();
    }
}

void GameWidget::handlePauseOverlayClick(int pixelX, int pixelY)
{
    if (resumeButtonRect().contains(pixelX, pixelY) ||
        closePauseButtonRect().contains(pixelX, pixelY))
    {
        emit resumeRequested();
        return;
    }

    if (resetButtonRect().contains(pixelX, pixelY))
    {
        emit resetRequested();
    }
}

void GameWidget::handleGameOverOverlayClick(int pixelX, int pixelY)
{
    if (gameOverResetButtonRect().contains(pixelX, pixelY))
    {
        emit resetRequested();
    }
}

void GameWidget::handleMapClick(int pixelX, int pixelY)
{
    const int x = (pixelX - mapOffsetX) / tileSize;
    const int y = (pixelY - mapOffsetY) / tileSize;

    setSelectedCell(x, y);
}

void GameWidget::handleBuildCardClick(int pixelX, int pixelY)
{
    int cardIndex = 0;

    if (selectedCellCanPlaceMelee())
    {
        if (cardRectAt(cardIndex).contains(pixelX, pixelY))
        {
            buildChoice = BuildChoice::MeleeTower;
            uiMode = UiMode::BuildPreview;
            update();
            return;
        }

        ++cardIndex;
    }

    if (selectedCellCanPlaceRanged())
    {
        if (cardRectAt(cardIndex).contains(pixelX, pixelY))
        {
            buildChoice = BuildChoice::RangedTower;
            uiMode = UiMode::BuildPreview;
            update();
        }
    }
}

void GameWidget::handleBuildPreviewClick(int pixelX, int pixelY)
{
    if (confirmBuildRect().contains(pixelX, pixelY))
    {
        confirmBuild();
        return;
    }

    if (cancelBuildRect().contains(pixelX, pixelY))
    {
        cancelBuild();
        return;
    }

    if (isInsideMapPixel(pixelX, pixelY))
    {
        const int x = (pixelX - mapOffsetX) / tileSize;
        const int y = (pixelY - mapOffsetY) / tileSize;
        setSelectedCell(x, y);
        return;
    }

    cancelBuild();
}

void GameWidget::confirmBuild()
{
    if (!hasSelectedCell())
    {
        cancelBuild();
        return;
    }

    if (buildChoice == BuildChoice::MeleeTower)
    {
        game.placeMeleeTowerAt(selectedX, selectedY);
    }
    else if (buildChoice == BuildChoice::RangedTower)
    {
        game.placeRangedTowerAt(selectedX, selectedY);
    }

    uiMode = UiMode::Normal;
    buildChoice = BuildChoice::None;
    update();
}

void GameWidget::cancelBuild()
{
    uiMode = UiMode::Normal;
    buildChoice = BuildChoice::None;
    update();
}

QRect GameWidget::selectedTowerSellRect() const
{
    if (!hasSelectedCell())
    {
        return QRect();
    }

    const QRect cell = mapCellRect(selectedX, selectedY);
    return QRect(cell.right() + 6, cell.top() + 4, 28, 28);
}

void GameWidget::handleTowerActionClick(int pixelX, int pixelY)
{
    if (!hasSelectedCell())
    {
        return;
    }

    if (selectedTowerSellRect().contains(pixelX, pixelY))
    {
        game.sellTowerAt(selectedX, selectedY);
        clearSelection();
        return;
    }

    const EntityView entity = selectedEntity();

    if (entity.kind == EntityKind::RangedTower)
    {
        const QRect burnRect = cardRectAt(0);
        if (burnRect.contains(pixelX, pixelY) &&
            game.canBuyAffixAt(selectedX, selectedY, AffixId::Burn))
        {
            game.buyAffixAt(selectedX, selectedY, AffixId::Burn);
            update();
            return;
        }

        const QRect slowRect = cardRectAt(1);
        if (slowRect.contains(pixelX, pixelY) &&
            game.canBuyAffixAt(selectedX, selectedY, AffixId::Slow))
        {
            game.buyAffixAt(selectedX, selectedY, AffixId::Slow);
            update();
            return;
        }
    }
    else if (entity.kind == EntityKind::MeleeTower)
    {
        const QRect berserkRect = cardRectAt(0);
        if (berserkRect.contains(pixelX, pixelY) &&
            game.canBuyAffixAt(selectedX, selectedY, AffixId::Berserk))
        {
            game.buyAffixAt(selectedX, selectedY, AffixId::Berserk);
            update();
            return;
        }
    }

    update();
}

EntityView GameWidget::selectedEntity() const
{
    if (!hasSelectedCell())
    {
        return EntityView{};
    }

    return game.getEntityViewAt(selectedX, selectedY);
}

bool GameWidget::selectedEntityIsTower() const
{
    const EntityView entity = selectedEntity();
    return entity.kind == EntityKind::MeleeTower ||
           entity.kind == EntityKind::RangedTower;
}

bool GameWidget::selectedEntityHasAffix(AffixId affixId) const
{
    const EntityView entity = selectedEntity();

    return std::find(entity.equippedAffixes.begin(),
                     entity.equippedAffixes.end(),
                     affixId) != entity.equippedAffixes.end();
}

bool GameWidget::isInsideMapPixel(int pixelX, int pixelY) const
{
    const Map &map = game.getMap();

    return pixelX >= mapOffsetX &&
           pixelY >= mapOffsetY &&
           pixelX < mapOffsetX + map.getWidth() * tileSize &&
           pixelY < mapOffsetY + map.getHeight() * tileSize;
}

bool GameWidget::hasSelectedCell() const
{
    return selectedX >= 0 && selectedY >= 0;
}

bool GameWidget::selectedCellIsEmpty() const
{
    if (!hasSelectedCell())
    {
        return false;
    }

    return game.getEntityViewAt(selectedX, selectedY).kind == EntityKind::None;
}

bool GameWidget::selectedCellCanShowBuildCards() const
{
    return selectedCellIsEmpty() &&
           (selectedCellCanPlaceMelee() || selectedCellCanPlaceRanged());
}

bool GameWidget::selectedCellCanPlaceMelee() const
{
    return hasSelectedCell() && game.canPlaceMeleeTowerAt(selectedX, selectedY);
}

bool GameWidget::selectedCellCanPlaceRanged() const
{
    return hasSelectedCell() && game.canPlaceRangedTowerAt(selectedX, selectedY);
}

float GameWidget::selectedBuildRange() const
{
    if (buildChoice == BuildChoice::MeleeTower)
    {
        return 1.0f;
    }

    if (buildChoice == BuildChoice::RangedTower)
    {
        return 3.0f;
    }

    return 0.0f;
}

QRect GameWidget::mapCellRect(int x, int y) const
{
    return QRect(mapOffsetX + x * tileSize, mapOffsetY + y * tileSize, tileSize, tileSize);
}

QRect GameWidget::pauseButtonRect() const
{
    return QRect(width() - 118, 14, 96, 32);
}

QRect GameWidget::startWaveButtonRect() const
{
    return QRect(width() - 236, 14, 108, 32);
}

QRect GameWidget::pauseOverlayRect() const
{
    return QRect(width() / 2 - 170, height() / 2 - 120, 340, 240);
}

QRect GameWidget::resumeButtonRect() const
{
    const QRect panel = pauseOverlayRect();
    return QRect(panel.left() + 70, panel.top() + 82, 200, 38);
}

QRect GameWidget::resetButtonRect() const
{
    const QRect panel = pauseOverlayRect();
    return QRect(panel.left() + 70, panel.top() + 132, 200, 38);
}

QRect GameWidget::closePauseButtonRect() const
{
    const QRect panel = pauseOverlayRect();
    return QRect(panel.right() - 42, panel.top() + 12, 28, 28);
}

QRect GameWidget::gameOverPanelRect() const
{
    return QRect(width() / 2 - 190, height() / 2 - 120, 380, 240);
}

QRect GameWidget::gameOverResetButtonRect() const
{
    const QRect panel = gameOverPanelRect();
    return QRect(panel.left() + 90, panel.top() + 148, 200, 40);
}

QRect GameWidget::cardRectAt(int index) const
{
    return QRect(CardStartX + index * (CardWidth + CardGap),
                 height() - CardBottomMargin,
                 CardWidth,
                 CardHeight);
}

QRect GameWidget::confirmBuildRect() const
{
    if (!hasSelectedCell())
    {
        return QRect();
    }

    const QRect cell = mapCellRect(selectedX, selectedY);
    return QRect(cell.left() - 34, cell.center().y() - 14, 28, 28);
}

QRect GameWidget::cancelBuildRect() const
{
    if (!hasSelectedCell())
    {
        return QRect();
    }

    const QRect cell = mapCellRect(selectedX, selectedY);
    return QRect(cell.right() + 6, cell.center().y() - 14, 28, 28);
}

void GameWidget::drawHud(QPainter &painter)
{
    painter.setPen(QColor(30, 35, 40));
    painter.setFont(QFont("Arial", 12, QFont::Bold));

    QString status = QString("金币：%1   生命：%2   敌人：%3/%4")
                         .arg(game.getMoney())
                         .arg(game.getPlayerHp())
                         .arg(game.getSpawnedEnemyCount())
                         .arg(game.getTotalEnemiesToSpawn());

    if (game.isGameOver())
    {
        status += game.isVictory() ? "   结果：胜利" : "   结果：失败";
    }

    painter.drawText(20, 30, status);
}

void GameWidget::drawTopButtons(QPainter &painter)
{
    painter.setFont(QFont("Arial", 10, QFont::Bold));

    if (!waveStarted && !game.isGameOver())
    {
        QRect startRect = startWaveButtonRect();
        painter.setBrush(QColor(70, 150, 95));
        painter.setPen(QPen(QColor(45, 105, 65), 2));
        painter.drawRoundedRect(startRect, 6, 6);

        painter.setPen(Qt::white);
        painter.drawText(startRect, Qt::AlignCenter, "开始波次");
    }

    if (!game.isGameOver())
    {
        QRect pauseRect = pauseButtonRect();
        painter.setBrush(QColor(65, 90, 130));
        painter.setPen(QPen(QColor(40, 55, 85), 2));
        painter.drawRoundedRect(pauseRect, 6, 6);

        painter.setPen(Qt::white);
        painter.drawText(pauseRect, Qt::AlignCenter, "暂停");
    }
}

void GameWidget::drawMap(QPainter &painter)
{
    painter.setBrush(Qt::NoBrush);
    const Map &map = game.getMap();

    for (int y = 0; y < map.getHeight(); ++y)
    {
        for (int x = 0; x < map.getWidth(); ++x)
        {
            const TileType tileType = map.getTileType(x, y);

            QColor color;
            QString label;

            switch (tileType)
            {
            case TileType::GRASS:
                color = QColor(118, 176, 105);
                break;
            case TileType::ROAD:
                color = QColor(198, 163, 105);
                break;
            case TileType::ROCK:
                color = QColor(95, 100, 105);
                break;
            case TileType::START:
                color = QColor(58, 137, 215);
                label = "S";
                break;
            case TileType::END:
                color = QColor(218, 78, 70);
                label = "E";
                break;
            default:
                color = QColor(35, 35, 35);
                label = "?";
                break;
            }

            const QRect cell = mapCellRect(x, y);

            painter.fillRect(cell, color);

            if (tileType == TileType::GRASS)
            {
                painter.setPen(QColor(95, 145, 85, 130));
                painter.drawLine(cell.left() + 8, cell.top() + 28, cell.left() + 14, cell.top() + 20);
                painter.drawLine(cell.left() + 24, cell.top() + 30, cell.left() + 30, cell.top() + 22);
            }
            else if (tileType == TileType::ROAD || tileType == TileType::START || tileType == TileType::END)
            {
                painter.setPen(QColor(150, 115, 70, 150));
                painter.drawLine(cell.left() + 4, cell.center().y(), cell.right() - 4, cell.center().y());
            }
            else if (tileType == TileType::ROCK)
            {
                painter.setPen(QColor(65, 70, 75));
                painter.drawLine(cell.topLeft() + QPoint(8, 8), cell.bottomRight() - QPoint(8, 8));
                painter.drawLine(cell.topRight() + QPoint(-8, 8), cell.bottomLeft() + QPoint(8, -8));
            }

            painter.setBrush(Qt::NoBrush);
            painter.setPen(QColor(45, 55, 60, 130));
            painter.drawRect(cell);

            if (!label.isEmpty())
            {
                painter.setPen(Qt::white);
                painter.setFont(QFont("Arial", 12, QFont::Bold));
                painter.drawText(cell, Qt::AlignCenter, label);
            }
        }
    }
}

void GameWidget::drawSelection(QPainter &painter)
{
    if (!hasSelectedCell())
    {
        return;
    }

    QRect selectedCell = mapCellRect(selectedX, selectedY);

    painter.fillRect(selectedCell.adjusted(3, 3, -3, -3), QColor(255, 230, 90, 70));

    QPen pen(QColor(255, 220, 70));
    pen.setWidth(4);

    painter.setBrush(Qt::NoBrush);
    painter.setPen(pen);
    painter.drawRect(selectedCell.adjusted(2, 2, -2, -2));
}

void GameWidget::drawEntities(QPainter &painter)
{
    for (const EntityView &entity : game.getEntityViews())
    {
        const int centerX = mapOffsetX + static_cast<int>(entity.x * tileSize) + tileSize / 2;
        const int centerY = mapOffsetY + static_cast<int>(entity.y * tileSize) + tileSize / 2;

        if (uiMode == UiMode::BuildPreview &&
            static_cast<int>(entity.x + 0.5f) == selectedX &&
            static_cast<int>(entity.y + 0.5f) == selectedY)
        {
            continue;
        }

        if (entity.kind == EntityKind::Enemy)
        {
            QRect body(centerX - 13, centerY - 13, 26, 26);

            painter.setBrush(QColor(210, 65, 65));
            painter.setPen(QPen(QColor(120, 35, 35), 2));
            painter.drawEllipse(body);

            if (hasActiveAffix(entity, "Slow"))
            {
                painter.setBrush(QColor(110, 210, 255, 120));
                painter.setPen(QPen(QColor(160, 235, 255, 210), 2));
                painter.drawEllipse(body.adjusted(-3, -3, 3, 3));

                painter.setPen(QPen(QColor(230, 250, 255, 210), 2));
                painter.drawLine(centerX - 9, centerY, centerX + 9, centerY);
                painter.drawLine(centerX, centerY - 9, centerX, centerY + 9);
            }

            painter.setPen(Qt::white);
            painter.setFont(QFont("Arial", 10, QFont::Bold));
            painter.drawText(body, Qt::AlignCenter, "E");
        }
        else if (entity.kind == EntityKind::MeleeTower)
        {
            QRect base(centerX - 15, centerY - 15, 30, 30);

            painter.setBrush(QColor(75, 105, 210));
            painter.setPen(QPen(QColor(40, 60, 130), 2));
            painter.drawRect(base.adjusted(2, 2, -2, -2));

            painter.setPen(Qt::white);
            painter.setFont(QFont("Arial", 10, QFont::Bold));
            painter.drawText(base, Qt::AlignCenter, "M");
        }
        else if (entity.kind == EntityKind::RangedTower)
        {
            QPolygon triangle;
            triangle << QPoint(centerX, centerY - 16)
                     << QPoint(centerX - 16, centerY + 14)
                     << QPoint(centerX + 16, centerY + 14);

            painter.setBrush(QColor(65, 155, 95));
            painter.setPen(QPen(QColor(35, 100, 60), 2));
            painter.drawPolygon(triangle);

            painter.setPen(Qt::white);
            painter.setFont(QFont("Arial", 10, QFont::Bold));
            painter.drawText(QRect(centerX - 14, centerY - 8, 28, 22), Qt::AlignCenter, "R");
        }

        if (entity.kind == EntityKind::MeleeTower || entity.kind == EntityKind::RangedTower)
        {
            drawEquippedAffixBadges(painter, entity, centerX, centerY);
        }

        if (entity.maxHp > 0)
        {
            const int barWidth = tileSize - 8;
            const int barHeight = 5;
            const int barX = centerX - barWidth / 2;
            const int barY = centerY + tileSize / 3 + 5;
            const double hpRatio = std::max(0.0, std::min(1.0, static_cast<double>(entity.hp) / entity.maxHp));

            painter.setPen(Qt::NoPen);
            painter.fillRect(barX, barY, barWidth, barHeight, QColor(55, 55, 55));
            painter.fillRect(barX, barY, static_cast<int>(barWidth * hpRatio), barHeight, QColor(90, 220, 110));
        }
    }
}

void GameWidget::drawTowerActions(QPainter &painter)
{
    const QRect sellRect = selectedTowerSellRect();

    painter.setBrush(QColor(210, 75, 70));
    painter.setPen(QPen(QColor(140, 45, 40), 2));
    painter.drawRoundedRect(sellRect, 5, 5);

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 14, QFont::Bold));
    painter.drawText(sellRect, Qt::AlignCenter, "X");
}

void GameWidget::drawAffixCards(QPainter &painter)
{
    const EntityView entity = selectedEntity();

    if (entity.kind == EntityKind::RangedTower)
    {
        drawAffixCard(painter,
                      cardRectAt(0),
                      AffixChoice::Burn,
                      game.canBuyAffixAt(selectedX, selectedY, AffixId::Burn),
                      selectedEntityHasAffix(AffixId::Burn));

        drawAffixCard(painter,
                      cardRectAt(1),
                      AffixChoice::Slow,
                      game.canBuyAffixAt(selectedX, selectedY, AffixId::Slow),
                      selectedEntityHasAffix(AffixId::Slow));
    }
    else if (entity.kind == EntityKind::MeleeTower)
    {
        drawAffixCard(painter,
                      cardRectAt(0),
                      AffixChoice::Berserk,
                      game.canBuyAffixAt(selectedX, selectedY, AffixId::Berserk),
                      selectedEntityHasAffix(AffixId::Berserk));
    }
}

void GameWidget::drawAffixCard(QPainter &painter, const QRect &rect, AffixChoice choice, bool enabled, bool equipped)
{
    const QColor background = enabled ? QColor(255, 255, 255, 235) : QColor(210, 214, 218, 210);
    const QColor border = equipped ? QColor(80, 150, 95) : QColor(90, 105, 125);
    const QColor textColor = enabled ? QColor(30, 35, 40) : QColor(110, 115, 120);

    painter.setBrush(background);
    painter.setPen(QPen(border, equipped ? 3 : 2));
    painter.drawRoundedRect(rect, 8, 8);

    QRect iconRect(rect.left() + 12, rect.top() + 8, rect.width() - 24, 42);
    QRect costRect(rect.left() + 8, rect.top() + 52, rect.width() - 16, 24);

    QString name;
    QColor iconColor;
    int cost = 0;

    switch (choice)
    {
    case AffixChoice::Burn:
        name = "燃烧";
        iconColor = QColor(220, 95, 55);
        cost = getAffixBuyPrice(AffixId::Burn);
        break;
    case AffixChoice::Slow:
        name = "减速";
        iconColor = QColor(95, 190, 235);
        cost = getAffixBuyPrice(AffixId::Slow);
        break;
    case AffixChoice::Berserk:
        name = "狂暴";
        iconColor = QColor(180, 80, 190);
        cost = getAffixBuyPrice(AffixId::Berserk);
        break;
    }

    painter.setBrush(enabled ? iconColor : QColor(145, 145, 145));
    painter.setPen(QPen(QColor(70, 75, 85), 2));
    painter.drawEllipse(iconRect.center(), 15, 15);

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 9, QFont::Bold));
    painter.drawText(iconRect, Qt::AlignCenter, name.left(1));

    painter.setPen(textColor);
    painter.setFont(QFont("Arial", 9, QFont::Bold));

    if (equipped)
    {
        painter.drawText(costRect, Qt::AlignCenter, "已装备");
    }
    else
    {
        painter.drawText(costRect, Qt::AlignCenter, QString("%1 %2").arg(name).arg(cost));
    }
}

void GameWidget::drawBuildCards(QPainter &painter)
{
    int cardIndex = 0;

    if (selectedCellCanPlaceMelee())
    {
        drawBuildCard(painter,
                      cardRectAt(cardIndex),
                      BuildChoice::MeleeTower,
                      true);
        ++cardIndex;
    }

    if (selectedCellCanPlaceRanged())
    {
        drawBuildCard(painter,
                      cardRectAt(cardIndex),
                      BuildChoice::RangedTower,
                      true);
    }
}

void GameWidget::drawBuildCard(QPainter &painter, const QRect &rect, BuildChoice choice, bool enabled)
{
    const QColor background = enabled ? QColor(255, 255, 255, 235) : QColor(210, 214, 218, 210);
    const QColor border = enabled ? QColor(90, 105, 125) : QColor(160, 165, 170);
    const QColor text = enabled ? QColor(30, 35, 40) : QColor(110, 115, 120);

    painter.setBrush(background);
    painter.setPen(QPen(border, 2));
    painter.drawRoundedRect(rect, 8, 8);

    QRect iconRect(rect.left() + 12, rect.top() + 8, rect.width() - 24, 42);
    QRect costRect(rect.left() + 8, rect.top() + 52, rect.width() - 16, 24);

    if (choice == BuildChoice::MeleeTower)
    {
        QRect body(iconRect.center().x() - 15, iconRect.center().y() - 15, 30, 30);
        painter.setBrush(enabled ? QColor(75, 105, 210) : QColor(135, 145, 160));
        painter.setPen(QPen(QColor(55, 65, 85), 2));
        painter.drawRect(body);
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        painter.drawText(body, Qt::AlignCenter, "M");

        painter.setPen(text);
        painter.setFont(QFont("Arial", 9, QFont::Bold));
        painter.drawText(costRect, Qt::AlignCenter, QString("花费 %1").arg(MeleeTower::COST));
    }
    else if (choice == BuildChoice::RangedTower)
    {
        const QPoint c = iconRect.center();
        QPolygon triangle;
        triangle << QPoint(c.x(), c.y() - 16)
                 << QPoint(c.x() - 16, c.y() + 14)
                 << QPoint(c.x() + 16, c.y() + 14);

        painter.setBrush(enabled ? QColor(65, 155, 95) : QColor(135, 150, 140));
        painter.setPen(QPen(QColor(45, 80, 55), 2));
        painter.drawPolygon(triangle);
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        painter.drawText(QRect(c.x() - 14, c.y() - 8, 28, 22), Qt::AlignCenter, "R");

        painter.setPen(text);
        painter.setFont(QFont("Arial", 9, QFont::Bold));
        painter.drawText(costRect, Qt::AlignCenter, QString("花费 %1").arg(RangedTower::COST));
    }
}

void GameWidget::drawBuildPreview(QPainter &painter)
{
    if (!hasSelectedCell() || buildChoice == BuildChoice::None)
    {
        return;
    }

    const QRect cell = mapCellRect(selectedX, selectedY);

    drawBuildGhost(painter, cell);

    const QRect confirmRect = confirmBuildRect();
    painter.setBrush(QColor(70, 170, 95));
    painter.setPen(QPen(QColor(35, 110, 60), 2));
    painter.drawRoundedRect(confirmRect, 5, 5);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 14, QFont::Bold));
    painter.drawText(confirmRect, Qt::AlignCenter, "✓");

    const QRect cancelRect = cancelBuildRect();
    painter.setBrush(QColor(210, 75, 70));
    painter.setPen(QPen(QColor(140, 45, 40), 2));
    painter.drawRoundedRect(cancelRect, 5, 5);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 14, QFont::Bold));
    painter.drawText(cancelRect, Qt::AlignCenter, "X");
}

void GameWidget::drawBuildGhost(QPainter &painter, const QRect &cell)
{
    const QPoint center = cell.center();

    if (buildChoice == BuildChoice::MeleeTower)
    {
        QRect body(center.x() - 15, center.y() - 15, 30, 30);

        painter.setBrush(QColor(75, 105, 210, 120));
        painter.setPen(QPen(QColor(40, 60, 130, 180), 2));
        painter.drawRect(body.adjusted(2, 2, -2, -2));

        painter.setPen(QColor(255, 255, 255, 190));
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        painter.drawText(body, Qt::AlignCenter, "M");
    }
    else if (buildChoice == BuildChoice::RangedTower)
    {
        QPolygon triangle;
        triangle << QPoint(center.x(), center.y() - 16)
                 << QPoint(center.x() - 16, center.y() + 14)
                 << QPoint(center.x() + 16, center.y() + 14);

        painter.setBrush(QColor(65, 155, 95, 120));
        painter.setPen(QPen(QColor(35, 100, 60, 180), 2));
        painter.drawPolygon(triangle);

        painter.setPen(QColor(255, 255, 255, 190));
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        painter.drawText(QRect(center.x() - 14, center.y() - 8, 28, 22), Qt::AlignCenter, "R");
    }
}

void GameWidget::drawAttackRange(QPainter &painter)
{
    if (!hasSelectedCell() || buildChoice == BuildChoice::None)
    {
        return;
    }

    const float range = selectedBuildRange();
    if (range <= 0.0f)
    {
        return;
    }

    const QRect cell = mapCellRect(selectedX, selectedY);
    const QPoint center = cell.center();
    const int radius = static_cast<int>(range * tileSize);

    painter.setBrush(QColor(120, 190, 255, 45));
    painter.setPen(QPen(QColor(70, 145, 220, 150), 2));
    painter.drawEllipse(center, radius, radius);
}

void GameWidget::drawSelectedTowerRange(QPainter &painter)
{
    if (!hasSelectedCell() || uiMode == UiMode::BuildPreview)
    {
        return;
    }

    const float range = selectedTowerRange();
    if (range <= 0.0f)
    {
        return;
    }

    const QRect cell = mapCellRect(selectedX, selectedY);
    const QPoint center = cell.center();
    const int radius = static_cast<int>(range * tileSize);

    painter.setBrush(QColor(255, 215, 90, 35));
    painter.setPen(QPen(QColor(245, 175, 55, 150), 2));
    painter.drawEllipse(center, radius, radius);
}

void GameWidget::drawEquippedAffixBadges(QPainter &painter, const EntityView &entity, int centerX, int centerY)
{
    if (entity.equippedAffixes.empty())
    {
        return;
    }

    int badgeX = centerX - 18;
    const int badgeY = centerY - 28;

    for (AffixId affixId : entity.equippedAffixes)
    {
        QColor color;
        QString label;

        switch (affixId)
        {
        case AffixId::Burn:
            color = QColor(220, 90, 50);
            label = "B";
            break;
        case AffixId::Slow:
            color = QColor(90, 190, 235);
            label = "S";
            break;
        case AffixId::Berserk:
            color = QColor(180, 80, 190);
            label = "Z";
            break;
        default:
            color = QColor(120, 120, 120);
            label = "?";
            break;
        }

        QRect badge(badgeX, badgeY, 16, 16);

        painter.setBrush(color);
        painter.setPen(QPen(QColor(45, 50, 60), 1));
        painter.drawEllipse(badge);

        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 8, QFont::Bold));
        painter.drawText(badge, Qt::AlignCenter, label);

        badgeX += 18;
    }
}

float GameWidget::selectedTowerRange() const
{
    const EntityView entity = selectedEntity();

    if (entity.kind == EntityKind::MeleeTower)
    {
        return 1.0f;
    }

    if (entity.kind == EntityKind::RangedTower)
    {
        return 3.0f;
    }

    return 0.0f;
}

QString GameWidget::equippedAffixesText(const EntityView &entity) const
{
    if (entity.equippedAffixes.empty())
    {
        return "无";
    }

    QString text;

    for (size_t i = 0; i < entity.equippedAffixes.size(); ++i)
    {
        if (i > 0)
        {
            text += ", ";
        }

        switch (entity.equippedAffixes[i])
        {
        case AffixId::Burn:
            text += "燃烧";
            break;
        case AffixId::Slow:
            text += "减速";
            break;
        case AffixId::Berserk:
            text += "狂暴";
            break;
        case AffixId::Swift:
            text += "迅捷";
            break;
        case AffixId::Blink:
            text += "闪现";
            break;
        default:
            text += "未知";
            break;
        }
    }

    return text;
}

QString GameWidget::activeAffixesText(const EntityView &entity) const
{
    if (entity.activeAffixNames.empty())
    {
        return "无";
    }

    QString text;

    for (size_t i = 0; i < entity.activeAffixNames.size(); ++i)
    {
        if (i > 0)
        {
            text += ", ";
        }

        const std::string &name = entity.activeAffixNames[i];

        if (name == "Burn")
        {
            text += "燃烧";
        }
        else if (name == "Slow")
        {
            text += "减速";
        }
        else if (name == "Berserk")
        {
            text += "狂暴";
        }
        else if (name == "Swift")
        {
            text += "迅捷";
        }
        else if (name == "Blink")
        {
            text += "闪现";
        }
        else
        {
            text += QString::fromStdString(name);
        }
    }

    return text;
}

void GameWidget::drawBottomHint(QPainter &painter)
{
    QRect hintRect(20, height() - 42, width() - 40, 28);

    painter.setBrush(QColor(255, 255, 255, 220));
    painter.setPen(QColor(180, 185, 190));
    painter.drawRoundedRect(hintRect, 6, 6);

    QString text;

    if (game.isGameOver())
    {
        text = game.isVictory() ? "胜利！点击重置可重新开始。" : "失败。点击重置可再次挑战。";
    }
    else if (uiMode == UiMode::BuildPreview)
    {
        text = "确认或取消本次防御塔建造。";
    }
    else if (selectedCellCanShowBuildCards())
    {
        text = "选择一张防御塔卡牌进行建造预览。";
    }
    else if (selectedX >= 0 && selectedY >= 0)
    {
        const EntityView entity = game.getEntityViewAt(selectedX, selectedY);
        text = QString("已选择 (%1, %2)").arg(selectedX).arg(selectedY);

        if (entity.kind == EntityKind::MeleeTower)
        {
            text += QString(" - 近战塔 | 生命 %1/%2 | 词缀：%3 | 出售时返还防御塔和词缀金币")
                        .arg(entity.hp)
                        .arg(entity.maxHp)
                        .arg(equippedAffixesText(entity));
        }
        else if (entity.kind == EntityKind::RangedTower)
        {
            text += QString(" - 远程塔 | 生命 %1/%2 | 词缀：%3 | 出售时返还防御塔和词缀金币")
                        .arg(entity.hp)
                        .arg(entity.maxHp)
                        .arg(equippedAffixesText(entity));
        }
        else if (entity.kind == EntityKind::Enemy)
        {
            text += QString(" - 敌人 | 生命 %1/%2 | 状态：%3")
                        .arg(entity.hp)
                        .arg(entity.maxHp)
                        .arg(activeAffixesText(entity));
        }
        else
        {
            text += " - 空地";
        }
    }
    else
    {
        text = "请选择一个地图格子。";
    }

    if (!game.getLastMessage().empty())
    {
        text += "    ";
        text += QString::fromStdString(game.getLastMessage());
    }

    painter.setPen(QColor(40, 45, 50));
    painter.setFont(QFont("Arial", 10));
    painter.drawText(hintRect.adjusted(10, 0, -10, 0), Qt::AlignVCenter | Qt::AlignLeft, text);
}

void GameWidget::drawPauseOverlay(QPainter &painter)
{
    painter.fillRect(rect(), QColor(20, 25, 30, 120));

    QRect panel = pauseOverlayRect();

    painter.setBrush(QColor(245, 247, 250));
    painter.setPen(QPen(QColor(80, 90, 105), 2));
    painter.drawRoundedRect(panel, 10, 10);

    painter.setPen(QColor(30, 35, 40));
    painter.setFont(QFont("Arial", 18, QFont::Bold));
    painter.drawText(QRect(panel.left(), panel.top() + 24, panel.width(), 36), Qt::AlignCenter, "已暂停");

    const QRect resumeRect = resumeButtonRect();
    painter.setBrush(QColor(70, 150, 95));
    painter.setPen(QPen(QColor(45, 105, 65), 2));
    painter.drawRoundedRect(resumeRect, 6, 6);

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.drawText(resumeRect, Qt::AlignCenter, "继续");

    const QRect resetRect = resetButtonRect();
    painter.setBrush(QColor(210, 95, 80));
    painter.setPen(QPen(QColor(145, 55, 45), 2));
    painter.drawRoundedRect(resetRect, 6, 6);

    painter.setPen(Qt::white);
    painter.drawText(resetRect, Qt::AlignCenter, "重置");

    const QRect closeRect = closePauseButtonRect();
    painter.setBrush(QColor(95, 105, 115));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(closeRect, 4, 4);

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.drawText(closeRect, Qt::AlignCenter, "X");
}

void GameWidget::drawGameOverOverlay(QPainter &painter)
{
    painter.fillRect(rect(), QColor(20, 25, 30, 150));

    const QRect panel = gameOverPanelRect();

    painter.setBrush(QColor(245, 247, 250));
    painter.setPen(QPen(QColor(80, 90, 105), 2));
    painter.drawRoundedRect(panel, 12, 12);

    const bool victory = game.isVictory();

    const QString title = victory ? "胜利" : "失败";
    const QString subtitle = victory ? "所有敌人都已被清除。" : "基地已被攻破。";
    const QColor titleColor = victory ? QColor(60, 150, 90) : QColor(200, 75, 65);

    painter.setPen(titleColor);
    painter.setFont(QFont("Arial", 24, QFont::Bold));
    painter.drawText(QRect(panel.left(), panel.top() + 32, panel.width(), 42), Qt::AlignCenter, title);

    painter.setPen(QColor(55, 60, 68));
    painter.setFont(QFont("Arial", 12));
    painter.drawText(QRect(panel.left() + 24, panel.top() + 88, panel.width() - 48, 36),
                     Qt::AlignCenter,
                     subtitle);

    const QRect resetRect = gameOverResetButtonRect();

    painter.setBrush(QColor(65, 90, 130));
    painter.setPen(QPen(QColor(40, 55, 85), 2));
    painter.drawRoundedRect(resetRect, 6, 6);

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.drawText(resetRect, Qt::AlignCenter, "重置");
}

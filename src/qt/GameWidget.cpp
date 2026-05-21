#include "GameWidget.h"

#include <QBrush>
#include <QColor>
#include <QFont>
#include <QMouseEvent>
#include <QPen>

#include <algorithm>

namespace
{
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
    setMinimumSize(820, 480);
}

void GameWidget::setSelectedCell(int x, int y)
{
    const Map &map = game.getMap();

    if (x < 0 || y < 0 || x >= map.getWidth() || y >= map.getHeight())
    {
        selectedX = -1;
        selectedY = -1;
    }
    else
    {
        selectedX = x;
        selectedY = y;
    }

    update();
}

void GameWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    painter.fillRect(rect(), QColor(238, 241, 245));

    painter.setRenderHint(QPainter::Antialiasing, false);
    drawHud(painter);
    drawMap(painter);
    drawSelection(painter);

    painter.setRenderHint(QPainter::Antialiasing, true);
    drawEntities(painter);
}

void GameWidget::mousePressEvent(QMouseEvent *event)
{
    if (!isInsideMapPixel(event->position().x(), event->position().y()))
    {
        return;
    }

    const int x = static_cast<int>((event->position().x() - mapOffsetX) / tileSize);
    const int y = static_cast<int>((event->position().y() - mapOffsetY) / tileSize);

    setSelectedCell(x, y);
    emit cellClicked(x, y);
}

bool GameWidget::isInsideMapPixel(int pixelX, int pixelY) const
{
    const Map &map = game.getMap();

    return pixelX >= mapOffsetX &&
           pixelY >= mapOffsetY &&
           pixelX < mapOffsetX + map.getWidth() * tileSize &&
           pixelY < mapOffsetY + map.getHeight() * tileSize;
}

void GameWidget::drawHud(QPainter &painter)
{
    painter.setPen(QColor(30, 35, 40));
    painter.setFont(QFont("Arial", 12, QFont::Bold));

    QString status = QString("Gold: %1   HP: %2   Enemies: %3/%4")
                         .arg(game.getMoney())
                         .arg(game.getPlayerHp())
                         .arg(game.getSpawnedEnemyCount())
                         .arg(game.getTotalEnemiesToSpawn());

    if (game.isGameOver())
    {
        status += game.isVictory() ? "   Result: Victory" : "   Result: Defeat";
    }

    painter.drawText(20, 28, status);
}

void GameWidget::drawMap(QPainter &painter)
{
    const Map &map = game.getMap();

    for (int y = 0; y < map.getHeight(); ++y)
    {
        for (int x = 0; x < map.getWidth(); ++x)
        {
            QColor color;
            QString label;

            switch (map.getTileType(x, y))
            {
            case TileType::GRASS:
                color = QColor(126, 184, 116);
                break;
            case TileType::ROAD:
                color = QColor(194, 166, 116);
                break;
            case TileType::ROCK:
                color = QColor(100, 105, 110);
                break;
            case TileType::START:
                color = QColor(74, 145, 220);
                label = "S";
                break;
            case TileType::END:
                color = QColor(220, 85, 75);
                label = "E";
                break;
            default:
                color = QColor(45, 45, 45);
                break;
            }

            QRect cell(mapOffsetX + x * tileSize, mapOffsetY + y * tileSize, tileSize, tileSize);

            painter.fillRect(cell, color);

            painter.setPen(QColor(70, 80, 85, 90));
            painter.drawRect(cell);

            if (!label.isEmpty())
            {
                painter.setPen(Qt::white);
                painter.setFont(QFont("Arial", 12, QFont::Bold));
                painter.drawText(cell, Qt::AlignCenter, label);
            }

            if (map.getTileType(x, y) == TileType::ROCK)
            {
                painter.setPen(QColor(70, 75, 80));
                painter.drawLine(cell.topLeft() + QPoint(8, 8), cell.bottomRight() - QPoint(8, 8));
                painter.drawLine(cell.topRight() + QPoint(-8, 8), cell.bottomLeft() + QPoint(8, -8));
            }
        }
    }
}

void GameWidget::drawSelection(QPainter &painter)
{
    if (selectedX < 0 || selectedY < 0)
    {
        return;
    }

    QRect selectedCell(
        mapOffsetX + selectedX * tileSize,
        mapOffsetY + selectedY * tileSize,
        tileSize,
        tileSize);

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
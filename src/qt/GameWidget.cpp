#include "GameWidget.h"

#include <QBrush>
#include <QColor>
#include <QFont>
#include <QPen>

GameWidget::GameWidget(Game &game, QWidget *parent)
    : QWidget(parent), game(game)
{
    setMinimumSize(820, 480);
}

void GameWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.fillRect(rect(), QColor(245, 247, 250));

    drawHud(painter);
    drawMap(painter);
    drawEntities(painter);
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
    const int offsetX = 20;
    const int offsetY = 50;

    for (int y = 0; y < map.getHeight(); ++y)
    {
        for (int x = 0; x < map.getWidth(); ++x)
        {
            QColor color;

            switch (map.getTileType(x, y))
            {
            case TileType::GRASS:
                color = QColor(170, 210, 140);
                break;
            case TileType::ROAD:
                color = QColor(205, 185, 150);
                break;
            case TileType::ROCK:
                color = QColor(120, 125, 130);
                break;
            case TileType::START:
                color = QColor(90, 170, 240);
                break;
            case TileType::END:
                color = QColor(230, 100, 90);
                break;
            default:
                color = QColor(40, 40, 40);
                break;
            }

            QRect cell(offsetX + x * tileSize, offsetY + y * tileSize, tileSize, tileSize);

            painter.fillRect(cell, color);
            painter.setPen(QColor(220, 225, 230));
            painter.drawRect(cell);
        }
    }
}

void GameWidget::drawEntities(QPainter &painter)
{
    const int offsetX = 20;
    const int offsetY = 50;

    for (const EntityView &entity : game.getEntityViews())
    {
        const int centerX = offsetX + static_cast<int>(entity.x * tileSize) + tileSize / 2;
        const int centerY = offsetY + static_cast<int>(entity.y * tileSize) + tileSize / 2;

        QColor color;
        QString label;

        switch (entity.kind)
        {
        case EntityKind::Enemy:
            color = QColor(220, 70, 70);
            label = "E";
            break;
        case EntityKind::MeleeTower:
            color = QColor(80, 110, 210);
            label = "M";
            break;
        case EntityKind::RangedTower:
            color = QColor(80, 160, 100);
            label = "R";
            break;
        default:
            continue;
        }

        painter.setBrush(QBrush(color));
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QPoint(centerX, centerY), tileSize / 3, tileSize / 3);

        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 11, QFont::Bold));
        painter.drawText(QRect(centerX - 12, centerY - 12, 24, 24), Qt::AlignCenter, label);

        if (entity.maxHp > 0)
        {
            const int barWidth = tileSize - 8;
            const int barHeight = 5;
            const int barX = centerX - barWidth / 2;
            const int barY = centerY + tileSize / 3 + 4;
            const double hpRatio = static_cast<double>(entity.hp) / entity.maxHp;

            painter.fillRect(barX, barY, barWidth, barHeight, QColor(70, 70, 70));
            painter.fillRect(barX, barY, static_cast<int>(barWidth * hpRatio), barHeight, QColor(90, 220, 110));
        }
    }
}
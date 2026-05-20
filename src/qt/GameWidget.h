#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include "../main/Game.h"

#include <QPainter>
#include <QWidget>

class GameWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GameWidget(Game &game, QWidget *parent = nullptr);

    void setSelectedCell(int x, int y);
    [[nodiscard]] int getSelectedX() const { return selectedX; }
    [[nodiscard]] int getSelectedY() const { return selectedY; }

signals:
    void cellClicked(int x, int y);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    Game &game;
    int tileSize = 40;
    int mapOffsetX = 20;
    int mapOffsetY = 50;
    int selectedX = -1;
    int selectedY = -1;

    void drawHud(QPainter &painter);
    void drawMap(QPainter &painter);
    void drawEntities(QPainter &painter);
    void drawSelection(QPainter &painter);

    [[nodiscard]] bool isInsideMapPixel(int pixelX, int pixelY) const;
};

#endif // GAMEWIDGET_H
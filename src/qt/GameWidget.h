#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include "../main/Game.h"

#include <QWidget>
#include <QPainter>

class GameWidget : public QWidget
{
public:
    explicit GameWidget(Game &game, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Game &game;
    int tileSize = 40;

    void drawHud(QPainter &painter);
    void drawMap(QPainter &painter);
    void drawEntities(QPainter &painter);
};

#endif // GAMEWIDGET_H
#ifndef CONSOLERENDERER_H
#define CONSOLERENDERER_H

class Game;

class ConsoleRenderer
{
public:
    void render(const Game &game) const;
};

#endif // CONSOLERENDERER_H
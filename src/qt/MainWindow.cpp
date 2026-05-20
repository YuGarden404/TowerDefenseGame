#include "MainWindow.h"

#include <QHBoxLayout>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      game(1, 1),
      gameWidget(new GameWidget(game, this)),
      timer(new QTimer(this)),
      selectedInfoLabel(new QLabel(this))
{
    initializeGame();
    setupUi();

    connect(gameWidget, &GameWidget::cellClicked, this, [this](int x, int y)
            { updateSelectedInfo(x, y); });

    connect(timer, &QTimer::timeout, this, [this]()
            {
                if (!game.isGameOver())
                {
                    game.spawnEnemy();
                    game.update(0.05f);
                    gameWidget->update();

                    if (gameWidget->getSelectedX() >= 0 && gameWidget->getSelectedY() >= 0)
                    {
                        updateSelectedInfo(gameWidget->getSelectedX(), gameWidget->getSelectedY());
                    }
                } });

    timer->start(50);
}

void MainWindow::initializeGame()
{
    if (!game.getMap().loadFromJsonFile("file/map.json"))
    {
        QMessageBox::critical(this, "Error", "Map load failed: file/map.json");
        return;
    }

    game.setTotalEnemiesToSpawn(5);

    game.placeMeleeTowerAt(5, 5);
    game.placeRangedTowerAt(5, 4);

    game.buyAffixAt(5, 4, AffixId::Burn);
    game.buyAffixAt(5, 4, AffixId::Slow);
    game.buyAffixAt(5, 5, AffixId::Berserk);
}

void MainWindow::setupUi()
{
    auto *central = new QWidget(this);
    auto *rootLayout = new QHBoxLayout(central);
    auto *sideLayout = new QVBoxLayout();

    selectedInfoLabel->setMinimumWidth(220);
    selectedInfoLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    selectedInfoLabel->setWordWrap(true);
    selectedInfoLabel->setText("Selected: none");

    sideLayout->addWidget(selectedInfoLabel);
    sideLayout->addStretch();

    rootLayout->addWidget(gameWidget);
    rootLayout->addLayout(sideLayout);

    setCentralWidget(central);
    setWindowTitle("Tower Defense");
    resize(1060, 560);
}

void MainWindow::updateSelectedInfo(int x, int y)
{
    const TileType tileType = game.getTileAt(x, y);
    const EntityView entity = game.getEntityViewAt(x, y);

    QString text;
    text += QString("Selected: (%1, %2)\n").arg(x).arg(y);
    text += QString("Tile: %1\n").arg(tileTypeToText(tileType));

    if (entity.kind == EntityKind::None)
    {
        text += "Entity: none\n";
    }
    else
    {
        text += QString("Entity: %1\n").arg(entityKindToText(entity.kind));
        text += QString("HP: %1 / %2\n").arg(entity.hp).arg(entity.maxHp);

        text += "Affixes: ";
        if (entity.equippedAffixes.empty())
        {
            text += "none";
        }
        else
        {
            for (size_t i = 0; i < entity.equippedAffixes.size(); ++i)
            {
                if (i > 0)
                {
                    text += ", ";
                }
                text += affixIdToText(entity.equippedAffixes[i]);
            }
        }
        text += "\n";
    }

    selectedInfoLabel->setText(text);
}

QString MainWindow::tileTypeToText(TileType type) const
{
    switch (type)
    {
    case TileType::GRASS:
        return "Grass";
    case TileType::ROAD:
        return "Road";
    case TileType::ROCK:
        return "Rock";
    case TileType::START:
        return "Start";
    case TileType::END:
        return "End";
    default:
        return "Unknown";
    }
}

QString MainWindow::entityKindToText(EntityKind kind) const
{
    switch (kind)
    {
    case EntityKind::Enemy:
        return "Enemy";
    case EntityKind::MeleeTower:
        return "Melee Tower";
    case EntityKind::RangedTower:
        return "Ranged Tower";
    default:
        return "None";
    }
}

QString MainWindow::affixIdToText(AffixId affixId) const
{
    switch (affixId)
    {
    case AffixId::Slow:
        return "Slow";
    case AffixId::Burn:
        return "Burn";
    case AffixId::Berserk:
        return "Berserk";
    case AffixId::Swift:
        return "Swift";
    case AffixId::Blink:
        return "Blink";
    default:
        return "Unknown";
    }
}
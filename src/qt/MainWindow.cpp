#include "MainWindow.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      game(1, 1),
      gameWidget(new GameWidget(game, this)),
      timer(new QTimer(this)),
      selectedInfoLabel(new QLabel(this)),
      messageLabel(new QLabel(this)),
      sellTowerButton(new QPushButton("Sell Tower", this)),
      placeMeleeButton(new QPushButton("Place Melee", this)),
      placeRangedButton(new QPushButton("Place Ranged", this)),
      buyBurnButton(new QPushButton("Buy Burn", this)),
      buySlowButton(new QPushButton("Buy Slow", this)),
      buyBerserkButton(new QPushButton("Buy Berserk", this)),
      sellBurnButton(new QPushButton("Sell Burn", this)),
      sellSlowButton(new QPushButton("Sell Slow", this)),
      sellBerserkButton(new QPushButton("Sell Berserk", this)),
      pauseButton(new QPushButton("Pause", this)),
      resetButton(new QPushButton("Reset", this))
{
    initializeGame();
    setupUi();
    setupConnections();

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

    selectedInfoLabel->setMinimumWidth(230);
    selectedInfoLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    selectedInfoLabel->setWordWrap(true);
    selectedInfoLabel->setText("Selected: none");

    messageLabel->setMinimumWidth(230);
    messageLabel->setWordWrap(true);
    messageLabel->setText(QString::fromStdString(game.getLastMessage()));

    auto *buildGroup = new QGroupBox("Build", this);
    auto *buildLayout = new QVBoxLayout(buildGroup);
    buildLayout->addWidget(placeMeleeButton);
    buildLayout->addWidget(placeRangedButton);
    buildLayout->addWidget(sellTowerButton);

    auto *buyGroup = new QGroupBox("Buy Affix", this);
    auto *buyLayout = new QVBoxLayout(buyGroup);
    buyLayout->addWidget(buyBurnButton);
    buyLayout->addWidget(buySlowButton);
    buyLayout->addWidget(buyBerserkButton);

    auto *sellGroup = new QGroupBox("Sell Affix", this);
    auto *sellLayout = new QVBoxLayout(sellGroup);
    sellLayout->addWidget(sellBurnButton);
    sellLayout->addWidget(sellSlowButton);
    sellLayout->addWidget(sellBerserkButton);

    sideLayout->addWidget(selectedInfoLabel);
    sideLayout->addWidget(messageLabel);
    sideLayout->addWidget(buildGroup);
    sideLayout->addWidget(buyGroup);
    sideLayout->addWidget(sellGroup);
    sideLayout->addWidget(pauseButton);
    sideLayout->addWidget(resetButton);
    sideLayout->addStretch();

    rootLayout->addWidget(gameWidget);
    rootLayout->addLayout(sideLayout);

    setCentralWidget(central);
    setWindowTitle("Tower Defense");
    resize(1100, 560);
}

void MainWindow::setupConnections()
{
    connect(gameWidget, &GameWidget::cellClicked, this, [this](int x, int y)
            { updateSelectedInfo(x, y); });

    connect(timer, &QTimer::timeout, this, [this]()
            {
                if (!game.isGameOver())
                {
                    game.spawnEnemy();
                    game.update(0.05f);
                    gameWidget->update();
                    refreshSelectedInfo();
                } });

    connect(placeMeleeButton, &QPushButton::clicked, this, &MainWindow::handlePlaceMelee);
    connect(placeRangedButton, &QPushButton::clicked, this, &MainWindow::handlePlaceRanged);

    connect(buyBurnButton, &QPushButton::clicked, this, [this]()
            { handleBuyAffix(AffixId::Burn); });
    connect(buySlowButton, &QPushButton::clicked, this, [this]()
            { handleBuyAffix(AffixId::Slow); });
    connect(buyBerserkButton, &QPushButton::clicked, this, [this]()
            { handleBuyAffix(AffixId::Berserk); });

    connect(sellBurnButton, &QPushButton::clicked, this, [this]()
            { handleSellAffix(AffixId::Burn); });
    connect(sellSlowButton, &QPushButton::clicked, this, [this]()
            { handleSellAffix(AffixId::Slow); });
    connect(sellBerserkButton, &QPushButton::clicked, this, [this]()
            { handleSellAffix(AffixId::Berserk); });

    connect(pauseButton, &QPushButton::clicked, this, &MainWindow::handleTogglePause);
    connect(resetButton, &QPushButton::clicked, this, &MainWindow::handleReset);

    connect(sellTowerButton, &QPushButton::clicked, this, &MainWindow::handleSellTower);
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

void MainWindow::refreshSelectedInfo()
{
    if (!hasSelectedCell())
    {
        return;
    }

    updateSelectedInfo(selectedX(), selectedY());
}

bool MainWindow::hasSelectedCell() const
{
    return selectedX() >= 0 && selectedY() >= 0;
}

int MainWindow::selectedX() const
{
    return gameWidget->getSelectedX();
}

int MainWindow::selectedY() const
{
    return gameWidget->getSelectedY();
}

void MainWindow::handlePlaceMelee()
{
    if (!hasSelectedCell())
    {
        messageLabel->setText("请先选择一个地图格子。");
        return;
    }

    game.placeMeleeTowerAt(selectedX(), selectedY());
    gameWidget->update();
    refreshSelectedInfo();
    updateMessageLabel();
}

void MainWindow::handlePlaceRanged()
{
    if (!hasSelectedCell())
    {
        messageLabel->setText("请先选择一个地图格子。");
        return;
    }

    game.placeRangedTowerAt(selectedX(), selectedY());
    gameWidget->update();
    refreshSelectedInfo();
    updateMessageLabel();
}

void MainWindow::handleBuyAffix(AffixId affixId)
{
    if (!hasSelectedCell())
    {
        messageLabel->setText("请先选择一个地图格子。");
        return;
    }

    game.buyAffixAt(selectedX(), selectedY(), affixId);
    gameWidget->update();
    refreshSelectedInfo();
    updateMessageLabel();
}

void MainWindow::handleSellAffix(AffixId affixId)
{
    if (!hasSelectedCell())
    {
        messageLabel->setText("请先选择一个地图格子。");
        return;
    }

    game.sellAffixAt(selectedX(), selectedY(), affixId);
    gameWidget->update();
    refreshSelectedInfo();
    updateMessageLabel();
}

void MainWindow::handleTogglePause()
{
    game.setPaused(!game.isPaused());
    pauseButton->setText(game.isPaused() ? "Resume" : "Pause");

    gameWidget->update();
    refreshSelectedInfo();
    updateMessageLabel();
}

void MainWindow::handleReset()
{
    game.reset();
    initializeGame();

    pauseButton->setText("Pause");

    gameWidget->setSelectedCell(-1, -1);
    selectedInfoLabel->setText("Selected: none");

    gameWidget->update();
    updateMessageLabel();
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

void MainWindow::updateMessageLabel()
{
    const QString message = QString::fromStdString(game.getLastMessage());

    if (message.isEmpty())
    {
        messageLabel->setText("暂无操作消息。");
        return;
    }

    messageLabel->setText(message);
}

void MainWindow::handleSellTower()
{
    if (!hasSelectedCell())
    {
        return;
    }

    game.sellTowerAt(selectedX(), selectedY());
    gameWidget->update();
    refreshSelectedInfo();
    updateMessageLabel();
}
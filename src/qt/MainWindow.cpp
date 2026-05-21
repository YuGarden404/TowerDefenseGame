#include "MainWindow.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QWidget>

#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      game(1, 1),
      gameWidget(new GameWidget(game, this)),
      timer(new QTimer(this)),
      selectedInfoLabel(new QLabel(this)),
      messageLabel(new QLabel(this)),
      placeMeleeButton(new QPushButton(this)),
      placeRangedButton(new QPushButton(this)),
      sellTowerButton(new QPushButton("Sell Tower", this)),
      buyBurnButton(new QPushButton(this)),
      buySlowButton(new QPushButton(this)),
      buyBerserkButton(new QPushButton(this)),
      sellBurnButton(new QPushButton(this)),
      sellSlowButton(new QPushButton(this)),
      sellBerserkButton(new QPushButton(this)),
      pauseButton(new QPushButton("暂停", this)),
      resetButton(new QPushButton("重置", this)),
      spawnTimer(0.0f),
      spawnInterval(1.0f),
      startWaveButton(new QPushButton("启动波动", this)),
      waveStarted(false)
{
    initializeGame();
    setupUi();
    setupConnections();
    refreshActionButtons();

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
}

void MainWindow::setupUi()
{
    placeMeleeButton->setText(QString("部署 Melee (%1)").arg(MeleeTower::COST));
    placeRangedButton->setText(QString("部署 Ranged (%1)").arg(RangedTower::COST));

    buyBurnButton->setText(QString("购买 Burn (%1)").arg(getAffixBuyPrice(AffixId::Burn)));
    buySlowButton->setText(QString("购买 Slow (%1)").arg(getAffixBuyPrice(AffixId::Slow)));
    buyBerserkButton->setText(QString("购买 Berserk (%1)").arg(getAffixBuyPrice(AffixId::Berserk)));

    sellBurnButton->setText(QString("出售 Burn (%1)").arg(getAffixSellPrice(AffixId::Burn)));
    sellSlowButton->setText(QString("出售 Slow (%1)").arg(getAffixSellPrice(AffixId::Slow)));
    sellBerserkButton->setText(QString("出售 Berserk (%1)").arg(getAffixSellPrice(AffixId::Berserk)));

    auto *central = new QWidget(this);
    auto *rootLayout = new QHBoxLayout(central);
    auto *sideLayout = new QVBoxLayout();

    selectedInfoLabel->setMinimumWidth(250);
    selectedInfoLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    selectedInfoLabel->setWordWrap(true);
    selectedInfoLabel->setText("已选择：无");

    messageLabel->setMinimumWidth(250);
    messageLabel->setWordWrap(true);
    messageLabel->setText(QString::fromStdString(game.getLastMessage()));

    auto *buildGroup = new QGroupBox("建造", this);
    auto *buildLayout = new QVBoxLayout(buildGroup);
    buildLayout->addWidget(placeMeleeButton);
    buildLayout->addWidget(placeRangedButton);
    buildLayout->addWidget(sellTowerButton);

    auto *buyGroup = new QGroupBox("购买词缀", this);
    auto *buyLayout = new QVBoxLayout(buyGroup);
    buyLayout->addWidget(buyBurnButton);
    buyLayout->addWidget(buySlowButton);
    buyLayout->addWidget(buyBerserkButton);

    auto *sellGroup = new QGroupBox("出售词缀", this);
    auto *sellLayout = new QVBoxLayout(sellGroup);
    sellLayout->addWidget(sellBurnButton);
    sellLayout->addWidget(sellSlowButton);
    sellLayout->addWidget(sellBerserkButton);

    sideLayout->addWidget(selectedInfoLabel);
    sideLayout->addWidget(messageLabel);
    sideLayout->addWidget(buildGroup);
    sideLayout->addWidget(buyGroup);
    sideLayout->addWidget(sellGroup);
    sideLayout->addWidget(startWaveButton);
    sideLayout->addWidget(pauseButton);
    sideLayout->addWidget(resetButton);
    sideLayout->addStretch();

    rootLayout->addWidget(gameWidget);
    rootLayout->addLayout(sideLayout);

    setCentralWidget(central);
    setWindowTitle("塔防游戏");
    resize(1120, 560);
}

void MainWindow::setupConnections()
{
    connect(gameWidget, &GameWidget::cellClicked, this, [this](int x, int y)
            {
                updateSelectedInfo(x, y);
                refreshActionButtons(); });

    connect(timer, &QTimer::timeout, this, [this]()
            {
            constexpr float deltaTime = 0.05f;

            if (!game.isGameOver() && !game.isPaused())
            {
                if (waveStarted)
                {
                    spawnTimer += deltaTime;

                    if (spawnTimer >= spawnInterval)
                    {
                        game.spawnEnemy();
                        spawnTimer = 0.0f;
                    }

                    game.update(deltaTime);
                }
            }

            gameWidget->update();
            refreshSelectedInfo();

            if (game.isGameOver())
            {
                refreshActionButtons();
            } });

    connect(placeMeleeButton, &QPushButton::clicked, this, &MainWindow::handlePlaceMelee);
    connect(placeRangedButton, &QPushButton::clicked, this, &MainWindow::handlePlaceRanged);
    connect(sellTowerButton, &QPushButton::clicked, this, &MainWindow::handleSellTower);
    connect(startWaveButton, &QPushButton::clicked, this, &MainWindow::handleStartWave);

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
}

void MainWindow::updateSelectedInfo(int x, int y)
{
    const TileType tileType = game.getTileAt(x, y);
    const EntityView entity = game.getEntityViewAt(x, y);

    QString text;
    text += QString("选择：(%1, %2)\n").arg(x).arg(y);
    text += QString("地块类型：%1\n").arg(tileTypeToText(tileType));

    if (entity.kind == EntityKind::None)
    {
        text += "实体：无\n";
    }
    else
    {
        text += QString("实体：%1\n").arg(entityKindToText(entity.kind));
        text += QString("HP：%1 / %2\n").arg(entity.hp).arg(entity.maxHp);

        text += "词缀：";
        if (entity.equippedAffixes.empty())
        {
            text += "无";
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

void MainWindow::updateMessageLabel()
{
    const QString message = QString::fromStdString(game.getLastMessage());

    if (message.isEmpty())
    {
        messageLabel->setText("无操作信息。");
        return;
    }

    messageLabel->setText(message);
}

void MainWindow::refreshActionButtons()
{
    const bool gameRunning = !game.isGameOver();
    const bool hasSelection = hasSelectedCell();

    placeMeleeButton->setEnabled(false);
    placeRangedButton->setEnabled(false);
    sellTowerButton->setEnabled(false);

    buyBurnButton->setEnabled(false);
    buySlowButton->setEnabled(false);
    buyBerserkButton->setEnabled(false);

    sellBurnButton->setEnabled(false);
    sellSlowButton->setEnabled(false);
    sellBerserkButton->setEnabled(false);

    pauseButton->setEnabled(gameRunning);
    resetButton->setEnabled(true);
    startWaveButton->setEnabled(gameRunning && !waveStarted);

    if (!gameRunning || !hasSelection)
    {
        return;
    }

    const int x = selectedX();
    const int y = selectedY();
    const EntityView entity = game.getEntityViewAt(x, y);

    if (entity.kind == EntityKind::None)
    {
        placeMeleeButton->setEnabled(game.canPlaceMeleeTowerAt(x, y));
        placeRangedButton->setEnabled(game.canPlaceRangedTowerAt(x, y));
        return;
    }

    const bool isTower = entity.kind == EntityKind::MeleeTower ||
                         entity.kind == EntityKind::RangedTower;

    sellTowerButton->setEnabled(isTower);

    buyBurnButton->setEnabled(selectedEntityCanBuyAffix(AffixId::Burn));
    buySlowButton->setEnabled(selectedEntityCanBuyAffix(AffixId::Slow));
    buyBerserkButton->setEnabled(selectedEntityCanBuyAffix(AffixId::Berserk));

    sellBurnButton->setEnabled(selectedEntityHasAffix(AffixId::Burn));
    sellSlowButton->setEnabled(selectedEntityHasAffix(AffixId::Slow));
    sellBerserkButton->setEnabled(selectedEntityHasAffix(AffixId::Berserk));
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

EntityView MainWindow::selectedEntity() const
{
    if (!hasSelectedCell())
    {
        return EntityView{};
    }

    return game.getEntityViewAt(selectedX(), selectedY());
}

bool MainWindow::selectedEntityHasAffix(AffixId affixId) const
{
    const EntityView entity = selectedEntity();

    return std::find(entity.equippedAffixes.begin(),
                     entity.equippedAffixes.end(),
                     affixId) != entity.equippedAffixes.end();
}

bool MainWindow::selectedEntityCanBuyAffix(AffixId affixId) const
{
    const EntityView entity = selectedEntity();

    if (selectedEntityHasAffix(affixId))
    {
        return false;
    }

    if (!game.canAfford(getAffixBuyPrice(affixId)))
    {
        return false;
    }

    switch (affixId)
    {
    case AffixId::Burn:
    case AffixId::Slow:
        return entity.kind == EntityKind::RangedTower;
    case AffixId::Berserk:
        return entity.kind == EntityKind::MeleeTower;
    default:
        return false;
    }
}

void MainWindow::handlePlaceMelee()
{
    if (!hasSelectedCell())
    {
        messageLabel->setText("请先选中一个单元格。");
        return;
    }

    game.placeMeleeTowerAt(selectedX(), selectedY());
    gameWidget->update();
    refreshSelectedInfo();
    updateMessageLabel();
    refreshActionButtons();
}

void MainWindow::handlePlaceRanged()
{
    if (!hasSelectedCell())
    {
        messageLabel->setText("请先选中一个单元格。");
        return;
    }

    game.placeRangedTowerAt(selectedX(), selectedY());
    gameWidget->update();
    refreshSelectedInfo();
    updateMessageLabel();
    refreshActionButtons();
}

void MainWindow::handleSellTower()
{
    if (!hasSelectedCell())
    {
        messageLabel->setText("请先选中一个单元格。");
        return;
    }

    game.sellTowerAt(selectedX(), selectedY());
    gameWidget->update();
    refreshSelectedInfo();
    updateMessageLabel();
    refreshActionButtons();
}

void MainWindow::handleBuyAffix(AffixId affixId)
{
    if (!hasSelectedCell())
    {
        messageLabel->setText("请先选中一个单元格。");
        return;
    }

    game.buyAffixAt(selectedX(), selectedY(), affixId);
    gameWidget->update();
    refreshSelectedInfo();
    updateMessageLabel();
    refreshActionButtons();
}

void MainWindow::handleSellAffix(AffixId affixId)
{
    if (!hasSelectedCell())
    {
        messageLabel->setText("请先选中一个单元格。");
        return;
    }

    game.sellAffixAt(selectedX(), selectedY(), affixId);
    gameWidget->update();
    refreshSelectedInfo();
    updateMessageLabel();
    refreshActionButtons();
}

void MainWindow::handleTogglePause()
{
    game.setPaused(!game.isPaused());
    pauseButton->setText(game.isPaused() ? "继续" : "暂停");

    gameWidget->update();
    refreshSelectedInfo();
    updateMessageLabel();
    refreshActionButtons();
}

void MainWindow::handleReset()
{
    game.reset();
    initializeGame();

    waveStarted = false;
    spawnTimer = 0.0f;

    pauseButton->setText("暂停");
    startWaveButton->setEnabled(true);

    gameWidget->setSelectedCell(-1, -1);
    selectedInfoLabel->setText("已选择：无");

    gameWidget->update();
    updateMessageLabel();
    refreshActionButtons();
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

void MainWindow::handleStartWave()
{
    if (game.isGameOver())
    {
        return;
    }

    waveStarted = true;
    spawnTimer = spawnInterval;
    startWaveButton->setEnabled(false);

    messageLabel->setText("Wave started.");
    gameWidget->update();
    refreshActionButtons();
}
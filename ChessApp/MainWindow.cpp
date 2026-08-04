#include "MainWindow.h"
#include "GamePage.h"
#include "GameContext.h"
#include "GamePageRenderer.h"
#include "GameEndChecker.h"

#include <QStackedWidget>
#include <QTcpSocket>
#include <QDebug>

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QFont>
#include <QStyleFactory>
#include <QFrame>
#include <QScrollArea>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    resize(1550, 800);

    network = new NetworkManager(this);

    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    setupMenuUI();
    setupLobbyUI();

    gamePage = new GamePage();
    connectGamePage();

    stackedWidget->addWidget(menuPage);
    stackedWidget->addWidget(lobbyPage);
    stackedWidget->addWidget(gamePage);

    stackedWidget->setCurrentWidget(menuPage);

    connect(network, &NetworkManager::connected,
        this, [=]()
        {
            connectButton->setText("Connected");

            connectButton->setEnabled(false);
            serverIpEdit->setEnabled(false);

            createGameButton->setEnabled(true);
            browseGamesButton->setEnabled(true);
        });

    connect(network, &NetworkManager::lobbyCleared,
        this, &MainWindow::clearLobby);

    connect(network, &NetworkManager::lobbyEntryReceived,
        this, &MainWindow::addLobbyEntry);

    connect(network, &NetworkManager::matchFound,
        this, [=]
        (QString color, QString variant, QString timeControl, 
            QString startingPosition, QString opponentNickname)
        {
            stackedWidget->removeWidget(gamePage);
            delete gamePage;

            gamePage = new GamePage();
            stackedWidget->addWidget(gamePage);
            connectGamePage();

            gamePage->getContext()->setVariant(chessVariants.at(variant));
            gamePage->getContext()->getGameRenderer()->setPlayerNames(this->nickname, opponentNickname);

            gamePage->startGame(color, startingPosition);
            gamePage->getContext()->setTimeControl(parseTimeControl(timeControl));

            stackedWidget->setCurrentWidget(gamePage);
        });

    connect(network, &NetworkManager::moveReceived,
        this, [=]
        (int from, int to, int rookFrom, int rookTo, int promotionPiece)
        {
            gamePage->applyNetworkMove(from, to, rookFrom, rookTo, promotionPiece);
        });

    connect(network, &NetworkManager::drawOfferReceived,
        this, [=](PieceColor offerer)
        {
            gamePage->getContext()->receiveDrawOffer(offerer);
        });

    connect(network, &NetworkManager::drawAcceptedReceived,
        this, [=]()
        {
            gamePage->getContext()->receiveDrawAccepted();
        });

    connect(network, &NetworkManager::resignationReceived,
        this, [=](PieceColor loser)
        {
            gamePage->getContext()->receiveResignation(loser);
        });

    connect(connectButton, &QPushButton::clicked, this, [=]() {
            network->connectToServer(serverIpEdit->text());
        });

    connect(createGameButton, &QPushButton::clicked, this, [=]() {
        nickname = nicknameEdit->text();

        network->sendCreateGame(
            nickname,
            variantBox->currentText(),
            timeBox->currentData().toString(),
            skillBox->currentText());

        stackedWidget->setCurrentWidget(lobbyPage);
        });

    connect(browseGamesButton, &QPushButton::clicked, this, [=]() {
        nickname = nicknameEdit->text();
        stackedWidget->setCurrentWidget(lobbyPage);
        });

    connect(backToMenuButton, &QPushButton::clicked, this, [=]() {
        stackedWidget->setCurrentWidget(menuPage);
        });
}

MainWindow::~MainWindow()
{
}

void MainWindow::connectGamePage() {
    connect(gamePage->getContext(), &GameContext::movePlayed,
        network, &NetworkManager::sendMove);

    connect(gamePage->getContext(), &GameContext::drawOffered,
        network, &NetworkManager::sendDrawOffer);

    connect(gamePage->getContext(), &GameContext::drawAccepted,
        network, &NetworkManager::sendDrawAccepted);

    connect(gamePage->getContext(), &GameContext::playerResigned,
        network, &NetworkManager::sendResignation);

    connect(gamePage->getContext(), &GameContext::gameFinished,
        network, &NetworkManager::sendGameOver);

    connect(gamePage->getContext(), &GameContext::returnToLobbyRequested,
        this, [this]()
        {
            stackedWidget->setCurrentWidget(lobbyPage);
        });
}

void MainWindow::setupMenuUI() {
    menuPage = new QWidget();

    createMenuWidgets();
    setupMenuStyles();
    setupMenuLayout();
}

void MainWindow::createMenuWidgets() {
    serverIpEdit = new QLineEdit();
    serverIpEdit->setText("127.0.0.1");

    connectButton = new QPushButton("Connect");

    nicknameEdit = new QLineEdit();
    nicknameEdit->setPlaceholderText("Nickname");

    skillBox = new QComboBox();
    skillBox->addItems({ "Beginner", "Intermediate", "Advanced", "Expert" });

    timeBox = new QComboBox();
    addTimeControls();

    variantBox = new QComboBox();
    variantBox->addItems({ "Classic", "Atomic", "Chess960" });

    createGameButton = new QPushButton("Create game");
    browseGamesButton = new QPushButton("Browse games");

    createGameButton->setEnabled(false);
    browseGamesButton->setEnabled(false);
}

void MainWindow::addTimeControls() {
    timeBox->addItem("1 min", "1+0");
    timeBox->addItem("1 min, +1 sec per move", "1+1");
    timeBox->addItem("3 min", "3+0");
    timeBox->addItem("3 min, +2 sec per move", "3+2");
    timeBox->addItem("5 min", "5+0");
    timeBox->addItem("5 min, +2 sec per move", "5+2");
    timeBox->addItem("10 min", "10");
    timeBox->addItem("15 min, +10 sec per move", "15+10");
    timeBox->addItem("30 min", "30");
}

void MainWindow::setupMenuStyles()
{
    serverIpEdit->setFixedWidth(513);
    nicknameEdit->setFixedWidth(513);
    skillBox->setFixedWidth(513);
    timeBox->setFixedWidth(513);
    variantBox->setFixedWidth(513);

    menuPage->setStyleSheet(R"(

        QLabel {
            font:22px Arial;
            color:black;
        }

        QLabel#titleLabel {
            font:42px Arial;
            font-weight:bold;
        }

        QLineEdit,QComboBox {
            font:22px Arial;
            padding:12px;
            border:2px solid #4A90E2;
            border-radius:14px;
            background:white;
        }

        QComboBox::drop-down {
            width:45px;
            border-left:2px solid #4A90E2;
            border-top-right-radius:14px;
            border-bottom-right-radius:14px;
        }

        QComboBox::down-arrow {
            image: url(:/assets/arrowDown.jpg);
            width: 12px;
            height: 12px;
        }

        QComboBox QAbstractItemView {
            font:22px Arial;
            border:2px solid #4A90E2;
            border-radius:10px;
            background:white;
            selection-background-color:#4A90E2;
            selection-color:white;
            outline:0px;
        }

        QComboBox QAbstractItemView::item:hover {
            background:#4A90E2;
            color:white;
        }

        QPushButton {
            font:24px Arial;
            font-weight:bold;
            padding:14px;
            border-radius:18px;
            color:black;
            min-width:220px;
        }

    )");

    connectButton->setStyleSheet(
        "QPushButton {"
        "background:#A9D4FF;"
        "border:3px solid #4A90E2;"
        "border-radius:18px;"
        "font:24px Arial;"
        "font-weight:bold;"
        "}"
        "QPushButton:hover {"
        "background:#87C2FF;"
        "}"
    );

    createGameButton->setStyleSheet(
        "QPushButton {"
        "background:#B9F6B3;"
        "border:3px solid #2E9B2E;"
        "border-radius:18px;"
        "font:24px Arial;"
        "font-weight:bold;"
        "}"
        "QPushButton:hover {"
        "background:#97EB8E;"
        "}"
    );

    browseGamesButton->setStyleSheet(
        "QPushButton {"
        "background:#FFD7A3;"
        "border:3px solid #D8831C;"
        "border-radius:18px;"
        "font:24px Arial;"
        "font-weight:bold;"
        "}"
        "QPushButton:hover {"
        "background:#FFC773;"
        "}"
    );
}

void MainWindow::setupMenuLayout() {
    auto outerLayout = new QVBoxLayout(menuPage);
    outerLayout->setAlignment(Qt::AlignCenter);

    auto layout = new QVBoxLayout();

    auto title = new QLabel("ChessApp");

    title->setAlignment(Qt::AlignCenter);
    title->setObjectName("titleLabel");

    auto buttonLayout = new QHBoxLayout();

    buttonLayout->addWidget(createGameButton);
    buttonLayout->addWidget(browseGamesButton);

    layout->addWidget(title);

    layout->addWidget(new QLabel("Server IP"));
    layout->addWidget(serverIpEdit);

    layout->addWidget(connectButton);

    layout->addSpacing(15);

    layout->addWidget(new QLabel("Nickname"));
    layout->addWidget(nicknameEdit);

    layout->addWidget(new QLabel("Skill level"));
    layout->addWidget(skillBox);

    layout->addWidget(new QLabel("Time control"));
    layout->addWidget(timeBox);

    layout->addWidget(new QLabel("Variant"));
    layout->addWidget(variantBox);

    layout->addSpacing(20);
    layout->addLayout(buttonLayout);

    outerLayout->addLayout(layout);
}

void MainWindow::setupLobbyUI() {
    lobbyPage = new QWidget();

    auto outerLayout = new QVBoxLayout(lobbyPage);
    outerLayout->setAlignment(Qt::AlignCenter);
    outerLayout->setSpacing(25);

    lobbyFrame = new QFrame();
    lobbyFrame->setAttribute(Qt::WA_StyledBackground, true);
    lobbyFrame->setObjectName("lobbyFrame");
    lobbyFrame->setFixedSize(800, 600);

    auto frameLayout = new QVBoxLayout(lobbyFrame);
    frameLayout->setContentsMargins(0, 0, 0, 0);

    auto scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->viewport()->setAutoFillBackground(false);

    auto contentWidget = new QWidget();
    contentWidget->setObjectName("contentWidget");

    gamesLayout = new QVBoxLayout(contentWidget);
    gamesLayout->setContentsMargins(0, 0, 0, 0);
    gamesLayout->setSpacing(0);
    gamesLayout->setAlignment(Qt::AlignTop);

    scrollArea->setWidget(contentWidget);

    frameLayout->addWidget(scrollArea);

    backToMenuButton = new QPushButton("Back to main menu");
    backToMenuButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

    outerLayout->addWidget(lobbyFrame, 0, Qt::AlignCenter);
    outerLayout->addWidget(backToMenuButton, 0, Qt::AlignCenter);

    lobbyPage->setStyleSheet(R"(

        QFrame#lobbyFrame {
            border:3px solid #4A90E2;
            border-radius:20px;
            background:white;
        }

        QScrollArea {
            border:none;
            background:transparent;
        }

        QScrollArea > QWidget > QWidget {
            background:white;
            border-radius:18px;
        }

        QLabel {
            font:22px Arial;
            color:black;
        }

        QPushButton {
            font:20px Arial;
            font-weight:bold;
            border-radius:14px;
        }

    )");

    backToMenuButton->setStyleSheet(
        "QPushButton {"
        "background:#D94B4B;"
        "color:white;"
        "border:none;"
        "border-radius:14px;"
        "padding:12px 22px;"
        "}"
        "QPushButton:hover {"
        "background:#BF3333;"
        "}"
    );

    gamesLayout->addStretch();
}

void MainWindow::addLobbyEntry(QString hostNickname, QString variant, QString timeControl, QString skill) {
    auto row = new QWidget();
    row->setFixedHeight(100);

    auto rowLayout = new QHBoxLayout(row);
    rowLayout->setContentsMargins(20, 10, 20, 10);

    auto info = new QLabel(hostNickname + ", " + variant + ", " + timeControl + ", " + skill);
    info->setFont(QFont("Arial", 18));

    auto joinGameButton = new QPushButton("Join");

    connect(joinGameButton, &QPushButton::clicked, this, [=]() {
        network->sendJoinGame(nickname, variant, timeControl, skill);
        });

    joinGameButton->setStyleSheet(
        "QPushButton {"
        "background:#B9F6B3;"
        "border:2px solid #2E9B2E;"
        "border-radius:14px;"
        "padding:8px 18px;"
        "font:20px Arial;"
        "font-weight:bold;"
        "}"
        "QPushButton:hover {"
        "background:#97EB8E;"
        "}"
    );

    rowLayout->addWidget(info);
    rowLayout->addStretch();
    rowLayout->addWidget(joinGameButton);

    if (this->nickname == hostNickname) {
        joinGameButton->setVisible(false);
        joinGameButton->setEnabled(false);
    }

    auto separator = new QFrame();
    separator->setFixedHeight(2);

    separator->setStyleSheet(
        "background:#4A90E2;"
        "border:none;"
    );

    gamesLayout->insertWidget(gamesLayout->count() - 1, row);
    gamesLayout->insertWidget(gamesLayout->count() - 1, separator);
}

void MainWindow::clearLobby() {
    while (gamesLayout->count() > 1) {
        auto item = gamesLayout->takeAt(0);
        delete item->widget();
        delete item;
    }
}
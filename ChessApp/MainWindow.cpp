#include "MainWindow.h"
#include "GamePage.h"
#include "GameContext.h"

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

    socket = new QTcpSocket(this);

    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    setupMenuUI();
    setupLobbyUI();

    gamePage = new GamePage();

    connect(gamePage->getContext(), &GameContext::movePlayed, this,
        [=](int from, int to, int rookFrom, int rookTo, int promotionPiece) {
            QString msg = "MOVE|" +
                QString::number(from) + "|" +
                QString::number(to) + "|" +
                QString::number(rookFrom) + "|" +
                QString::number(rookTo) + "|" +
                QString::number(promotionPiece);

            socket->write(msg.toUtf8());
        });

    connect(gamePage->getContext(), &GameContext::drawOffered, this,
        [=](PieceColor offerer) {
            socket->write(QString("DRAW_OFFER|%1\n")
                .arg(static_cast<int>(offerer))
                .toUtf8());
        });

    connect(gamePage->getContext(), &GameContext::drawAccepted, this,
        [=]() {
            socket->write("DRAW_ACCEPTED\n");
        });

    connect(gamePage->getContext(), &GameContext::playerResigned, this,
        [=](PieceColor loser) {
            socket->write(QString("RESIGN|%1\n")
                .arg(static_cast<int>(loser))
                .toUtf8());
        });

    stackedWidget->addWidget(menuPage);
    stackedWidget->addWidget(lobbyPage);
    stackedWidget->addWidget(gamePage);

    stackedWidget->setCurrentWidget(menuPage);

    socket->connectToHost("127.0.0.1", 12345);

    connect(socket, &QTcpSocket::connected, this, [=]() {
        qDebug() << "Connected to ChessServer!";
        });

    connect(socket, &QTcpSocket::readyRead, this, [=]() {

        QString msg = socket->readAll();
        qDebug() << "CLIENT RECEIVED:\n" << msg;

        QStringList lines = msg.split('\n');

        for (const QString& line : lines) {

            if (line.startsWith("LOBBY_UPDATE")) {
                clearLobby();
                continue;
            }

            if (line.startsWith("MATCH_FOUND")) {

                QStringList parts = line.split('|');

                if (parts.size() != 5)
                    continue;

                QString color = parts[1];
                QString variant = parts[2];
                QString timeControl = parts[3];
                QString startingPosition = parts[4];

                qDebug() << "MATCH RECEIVED";
                qDebug() << "Color:" << color;
                qDebug() << "Variant:" << variant;
                qDebug() << "Time:" << timeControl;
                qDebug() << "Starting position:" << startingPosition;

                gamePage->getContext()->setVariant(chessVariants.at(variant));

                gamePage->startGame(color, startingPosition);

                gamePage->getContext()->setTimeControl(parseTimeControl(timeControl));

                stackedWidget->setCurrentWidget(gamePage);

                continue;
            }

            if (line.startsWith("MOVE|")) {
                qDebug() << "MOVE RECEIVED";

                QStringList parts = line.split('|');

                if (parts.size() != 6)
                    continue;

                int from = parts[1].toInt();
                int to = parts[2].toInt();
                int rookFrom = parts[3].toInt();
                int rookTo = parts[4].toInt();
                int promotionPiece = parts[5].toInt();

                gamePage->applyNetworkMove(from, to, rookFrom, rookTo, promotionPiece);

                continue;
            }

            if (line.startsWith("DRAW_OFFER|")) {
                QStringList parts = line.split('|');
                PieceColor offerer = static_cast<PieceColor>(parts[1].toInt());

                gamePage->getContext()->receiveDrawOffer(offerer);
                continue;
            }

            if (line.startsWith("DRAW_ACCEPTED")) {
                qDebug() << "DRAW ACCEPTED RECEIVED";
                gamePage->getContext()->receiveDrawAccepted();
                continue;
            }

            if (line.startsWith("RESIGN|")) {
                QStringList parts = line.split('|');
                PieceColor loser = static_cast<PieceColor>(parts[1].toInt());

                gamePage->getContext()->receiveResignation(loser);
                continue;
            }

            QStringList parts = line.split('|');

            if (parts.size() == 4)
                addLobbyEntry(parts[0], parts[1], parts[2], parts[3]);
        }
        });

    connect(createGameButton, &QPushButton::clicked, this, [=]() {
        nickname = nicknameEdit->text();

        QString request =
            nickname + "|" +
            variantBox->currentText() + "|" +
            timeBox->currentText() + "|" +
            skillBox->currentText();

        socket->write(request.toUtf8());

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


void MainWindow::setupMenuUI() {
    menuPage = new QWidget();

    createMenuWidgets();
    setupMenuStyles();
    setupMenuLayout();
}

void MainWindow::createMenuWidgets() {
    nicknameEdit = new QLineEdit();
    nicknameEdit->setPlaceholderText("Nickname");

    skillBox = new QComboBox();
    skillBox->addItems({ "Beginner", "Intermediate", "Advanced", "Expert" });

    timeBox = new QComboBox();
    timeBox->addItems({ "1+0","1+1","3+0","3+2","5+0","5+2","10","15+10","30" });

    variantBox = new QComboBox();
    variantBox->addItems({ "Classic","Atomic","Chess960" });

    createGameButton = new QPushButton("Create Game");
    browseGamesButton = new QPushButton("Browse Games");
}

void MainWindow::setupMenuStyles()
{
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

    layout->addWidget(new QLabel("Nickname"));
    layout->addWidget(nicknameEdit);

    layout->addWidget(new QLabel("Skill Level"));
    layout->addWidget(skillBox);

    layout->addWidget(new QLabel("Time Control"));
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

    backToMenuButton = new QPushButton("Back to Main Menu");
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

    joinGameButton = new QPushButton("Join");

    connect(joinGameButton, &QPushButton::clicked, this, [=]() {

        QString request =
            this->nickname + "|" +
            variant + "|" +
            timeControl + "|" +
            skill;

        socket->write(request.toUtf8());
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
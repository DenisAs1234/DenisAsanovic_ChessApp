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
        [=](int from, int to) {
            QString msg = "MOVE|" + QString::number(from) + "|" + QString::number(to);
            socket->write(msg.toUtf8());
        });
    /*
    connect(gamePage->getContext(), &GameContext::positionChanged, this,
        [=](QString fen, int fromIndex, int toIndex) {
            QString msg =
                "POSITION|" +
                fen + "|" +
                QString::number(fromIndex) + "|" +
                QString::number(toIndex);

            socket->write(msg.toUtf8());
        });*/

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
                if (parts.size() != 2) continue;

                QString color = parts[1].trimmed();

                qDebug() << "MATCH RECEIVED";
                qDebug() << "COLOR:" << color;

                gamePage->startGame(color);
                stackedWidget->setCurrentWidget(gamePage);

                continue;
            }

            if (line.startsWith("MOVE|")) {
                qDebug() << "MOVE RECEIVED";

                QStringList parts = line.split('|');

                int from = parts[1].toInt();
                int to = parts[2].toInt();

                gamePage->applyNetworkMove(from, to);

                return;
            }
            /*
            if (line.startsWith("POSITION|"))
            {
                QStringList parts = line.split('|');

                if (parts.size() != 4)
                    continue;

                QString fen = parts[1];
                int fromIndex = parts[2].toInt();
                int toIndex = parts[3].toInt();

                gamePage->loadPosition(fen, fromIndex, toIndex);

                continue;
            }*/

            QStringList parts = line.split('|');

            if (parts.size() == 4)
                addLobbyEntry(parts[0], parts[1], parts[2], parts[3]);
        }
        });

    connect(createGameButton, &QPushButton::clicked, this, [=]() {
        QString request =
            nicknameEdit->text() + "|" +
            variantBox->currentText() + "|" +
            timeBox->currentText() + "|" +
            skillBox->currentText();

        socket->write(request.toUtf8());

        stackedWidget->setCurrentWidget(lobbyPage);
        });

    connect(joinGameButton, &QPushButton::clicked, this, [=]() {
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
    variantBox->addItems({ "Classic","Atomic","Fischer Random" });

    createGameButton = new QPushButton("Create Game");
    joinGameButton = new QPushButton("Join Game");
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

    joinGameButton->setStyleSheet(
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
    buttonLayout->addWidget(joinGameButton);

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

void MainWindow::addLobbyEntry(QString nickname, QString variant, QString timeControl, QString skill) {
    auto row = new QWidget();
    row->setFixedHeight(100);

    auto rowLayout = new QHBoxLayout(row);
    rowLayout->setContentsMargins(20, 10, 20, 10);

    auto info = new QLabel(nickname + ", " + variant + ", " + timeControl + ", " + skill);
    info->setFont(QFont("Arial", 18));

    auto joinButton = new QPushButton("Join");

    joinButton->setStyleSheet(
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
    rowLayout->addWidget(joinButton);

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
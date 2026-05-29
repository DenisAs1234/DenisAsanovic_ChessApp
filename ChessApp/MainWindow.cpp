#include "MainWindow.h"
#include "GamePage.h"

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

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    resize(1550, 800);

    socket = new QTcpSocket(this);

    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    setupMenuUI();

    lobbyPage = new QWidget();
    gamePage = new GamePage();

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
        qDebug() << msg;

        if (msg.startsWith("MATCH_FOUND")) {
            gamePage->startGame();
            stackedWidget->setCurrentWidget(gamePage);
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
    timeBox->addItems({ "1","1+1","3","3+2","5","5+2","10","15+10","30" });

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
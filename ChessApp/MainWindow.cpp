#include "MainWindow.h"
#include "GameContext.h"
#include "BoardRenderer.h"
#include "GameState.h"
#include "PositionAnalyzer.h"
#include "SpecialMoveHandler.h"
#include "GameEndChecker.h"
#include "PieceFactory.h"
#include <QGraphicsView>
#include <QTcpSocket>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    resize(1550, 800);

    board = new BoardRenderer(new QGraphicsScene(this));
    auto state = new GameState();
    auto analyzer = new PositionAnalyzer(state, board);
    auto specialMoves = new SpecialMoveHandler(state, board, analyzer);
    auto gameEndings = new GameEndChecker(state, board, analyzer);
    
    auto context = new GameContext(state, board, analyzer, specialMoves, gameEndings);

    board->setContext(context);
    auto factory = new PieceFactory(context);
    specialMoves->setFactory(factory);
    context->setFactory(factory);

    view = new QGraphicsView(board->getScene(), this);
    setCentralWidget(view);

    board->drawBoard();
    board->drawButtons();
    board->drawClocks();
    context->setupStartingPosition();

    QTcpSocket* socket = new QTcpSocket(this);

    socket->connectToHost("127.0.0.1", 12345);

    connect(socket, &QTcpSocket::connected, this, [=]()
        {
            qDebug() << "Connected to ChessServer!";

            QString request =
                "Denis|Standard|5+0|1500";

            socket->write(request.toUtf8());
        });

    connect(socket, &QTcpSocket::readyRead, this, [=]()
        {
            qDebug() << socket->readAll();
        });
}

MainWindow::~MainWindow()
{
}


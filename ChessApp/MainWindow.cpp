#include "MainWindow.h"
#include "ChessBoard.h"
#include <QGraphicsView>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    resize(1550, 800);

    board = new ChessBoard(new QGraphicsScene(this));

    view = new QGraphicsView(board->getScene(), this);
    setCentralWidget(view);

    board->drawBoard();
    board->setStartingPosition();
}

MainWindow::~MainWindow()
{
}


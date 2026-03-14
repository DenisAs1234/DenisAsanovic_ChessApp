#include "MainWindow.h"
#include "ChessBoard.h"
#include <QGraphicsView>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    board = new ChessBoard(new QGraphicsScene(this));
    board->drawChessBoard();

    view = new QGraphicsView(board->getScene(), this);
    setCentralWidget(view);
}

MainWindow::~MainWindow()
{
}


#pragma once

#include <QMainWindow>
#include <QGraphicsView>
#include "ChessBoard.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    ChessBoard* board;
    QGraphicsView* view;
};


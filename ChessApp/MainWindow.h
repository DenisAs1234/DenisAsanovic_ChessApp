#pragma once

#include <QMainWindow>
#include <QGraphicsView>
#include "BoardRenderer.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    BoardRenderer* board;
    QGraphicsView* view;
};


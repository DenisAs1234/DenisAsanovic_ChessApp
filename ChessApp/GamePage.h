#pragma once

#include <QWidget>

class BoardRenderer;
class GameContext;
class QGraphicsView;

class GamePage : public QWidget
{
    Q_OBJECT

public:
    GamePage(QWidget* parent = nullptr);

    void startGame();

private:
    BoardRenderer* board;
    GameContext* context;
    QGraphicsView* view;
};

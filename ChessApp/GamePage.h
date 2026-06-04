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

    void startGame(QString& playerColor);
    GameContext* getContext();
    void applyNetworkMove(int fromIndex, int toIndex);
    //void loadPosition(QString fen, int fromIndex, int toIndex);

private:
    BoardRenderer* board;
    GameContext* context;
    QGraphicsView* view;
};

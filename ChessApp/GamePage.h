#pragma once

#include <QWidget>
#include "enums.h"
using namespace std;

class GamePageRenderer;
class GameContext;
class GameState;
class GameEndChecker;
class PositionAnalyzer;
class SpecialMoveHandler;
class PieceFactory;
class QGraphicsView;

class GamePage : public QWidget
{
    Q_OBJECT

public:
    GamePage(QWidget* parent = nullptr);

    void startGame(QString playerColor, QString orderOfPieces);
    GameContext* getContext();
    void applyNetworkMove(int fromIndex, int toIndex, int rookFrom, int rookTo, int promotionPiece);

private:
    QGraphicsView* view;

    unique_ptr<GameState> state;
    unique_ptr<GamePageRenderer> gameRenderer;
    unique_ptr<PositionAnalyzer> analyzer;
    unique_ptr<SpecialMoveHandler> specialMoves;
    unique_ptr<GameEndChecker> gameEndings;
    unique_ptr<GameContext> context;
    unique_ptr<PieceFactory> factory;
};

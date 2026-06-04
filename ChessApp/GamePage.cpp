#include "GamePage.h"

#include "GameContext.h"
#include "BoardRenderer.h"
#include "GameState.h"
#include "PositionAnalyzer.h"
#include "SpecialMoveHandler.h"
#include "GameEndChecker.h"
#include "PieceFactory.h"
#include "Square.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QVBoxLayout>

GamePage::GamePage(QWidget* parent)
    : QWidget(parent)
{
    board = new BoardRenderer(new QGraphicsScene(this));

    auto state = new GameState();
    auto analyzer = new PositionAnalyzer(state, board);
    auto specialMoves = new SpecialMoveHandler(state, board, analyzer);
    auto gameEndings = new GameEndChecker(state, board, analyzer);

    context = new GameContext(state, board, analyzer, specialMoves, gameEndings);

    board->setContext(context);
    gameEndings->setContext(context);

    auto factory = new PieceFactory(context);

    specialMoves->setFactory(factory);
    context->setFactory(factory);

    view = new QGraphicsView(board->getScene());

    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(view);
}

void GamePage::startGame(QString& playerColor) {
    PieceColor localPlayerColor = playerColor == "White" ? PieceColor::White : PieceColor::Black;
    context->setLocalPlayerColor(localPlayerColor);
    
    board->getScene()->clear();

    board->drawBoard(localPlayerColor);
    board->drawButtons();
    board->drawClocks();

    context->setupStartingPosition();
    context->startClock();
}

GameContext* GamePage::getContext() { return context; }
/*
void GamePage::loadPosition(QString fen, int fromIndex, int toIndex) {
    context->loadPositionFromFen(fen);

    auto allSquares = context->getState()->getAllSquares();
    auto startingSquare = allSquares[fromIndex];
    auto destination = allSquares[toIndex];
    board->highlightLastMove(startingSquare, destination);
}*/

void GamePage::applyNetworkMove(int fromIndex, int toIndex) {
    qDebug() << fromIndex << toIndex;

    context->setApplyingNetworkMove(true);

    Square* from = context->getState()->getAllSquares()[fromIndex];
    Square* to = context->getState()->getAllSquares()[toIndex];

    from->getPiece()->moveTo(to);
    context->updateGameStateAfterMove();

    board->resetHighlightedMove();
    board->highlightLastMove(from, to);

    context->setApplyingNetworkMove(false);
}
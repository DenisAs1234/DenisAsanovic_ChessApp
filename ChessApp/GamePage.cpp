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
    specialMoves->setContext(context);
    gameEndings->setContext(context);

    auto factory = new PieceFactory(context);

    specialMoves->setFactory(factory);
    context->setFactory(factory);

    view = new QGraphicsView(board->getScene());

    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(view);
}

void GamePage::startGame(QString playerColor, QString orderOfPieces) {
    PieceColor localPlayerColor = playerColor == "White" ? PieceColor::White : PieceColor::Black;
    context->setLocalPlayerColor(localPlayerColor);
    
    board->getScene()->clear();

    board->drawBoard(localPlayerColor);
    board->drawButtons();
    board->drawClocks();

    context->setupStartingPosition(orderOfPieces);
    context->startClock();
}

GameContext* GamePage::getContext() { return context; }

void GamePage::applyNetworkMove(int fromIndex, int toIndex, int rookFrom, int rookTo, int promotionPiece) {
    qDebug() << fromIndex << toIndex;

    context->setApplyingNetworkMove(true);

    auto allSquares = context->getState()->getAllSquares();

    Square* from = allSquares[fromIndex];
    Square* to = allSquares[toIndex];

    if (rookFrom != -1) {
        Square* rookStart = allSquares[rookFrom];
        Square* rookEnd = allSquares[rookTo];

        if (rookStart->getPiece())
            rookStart->getPiece()->moveTo(rookEnd);
    }

    from->getPiece()->moveTo(to);

    if (promotionPiece != -1) {
        PieceColor color = context->getState()->getTurnColor();

        context->getSpecialMoves()->executePromotionFromNetwork(
            color, static_cast<PieceType>(promotionPiece), to);
    }

    context->updateGameStateAfterMove();
    context->clearDrawOffer();

    board->resetHighlightedMove();
    board->highlightLastMove(from, to);

    context->setApplyingNetworkMove(false);
}
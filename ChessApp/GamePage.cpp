#include "GamePage.h"

#include "GameContext.h"
#include "GamePageRenderer.h"
#include "GameState.h"
#include "PositionAnalyzer.h"
#include "SpecialMoveHandler.h"
#include "GameEndChecker.h"
#include "PieceFactory.h"
#include "Square.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QVBoxLayout>
using namespace std;

GamePage::GamePage(QWidget* parent)
    : QWidget(parent)
{
    gameRenderer = make_unique<GamePageRenderer>(new QGraphicsScene(this));
    state = make_unique<GameState>();
    analyzer = make_unique<PositionAnalyzer>(state.get(), gameRenderer.get());
    specialMoves = make_unique<SpecialMoveHandler>(state.get(), gameRenderer.get(), analyzer.get());
    gameEndings = make_unique<GameEndChecker>(state.get(), gameRenderer.get(), analyzer.get());

    context = make_unique<GameContext>(
        state.get(),                     
        gameRenderer.get(),
        analyzer.get(),                       
        specialMoves.get(),
        gameEndings.get());

    gameRenderer->setContext(context.get());
    specialMoves->setContext(context.get());
    gameEndings->setContext(context.get());

    factory = make_unique<PieceFactory>(context.get());

    specialMoves->setFactory(factory.get());
    context->setFactory(factory.get());

    view = new QGraphicsView(gameRenderer->getScene());

    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(view);
}

void GamePage::startGame(QString playerColor, QString orderOfPieces) {
    PieceColor localPlayerColor = playerColor == "White" ? PieceColor::White : PieceColor::Black;
    context->setLocalPlayerColor(localPlayerColor);
    
    gameRenderer->getScene()->clear();

    gameRenderer->drawBoard(localPlayerColor);
    gameRenderer->drawButtons();
    gameRenderer->drawNicknames();
    gameRenderer->drawClocks();

    context->setupStartingPosition(orderOfPieces);
    context->startClock();
}

GameContext* GamePage::getContext() { return context.get(); }

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

    gameRenderer->resetHighlightedMove();
    gameRenderer->highlightLastMove(from, to);

    context->setApplyingNetworkMove(false);
}
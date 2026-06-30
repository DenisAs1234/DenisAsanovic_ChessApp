#include "GameEndChecker.h"
#include "enums.h"
#include "GameState.h"
#include "GamePageRenderer.h"
#include "PositionAnalyzer.h"
#include "GameContext.h"
#include "Square.h"
#include "Piece.h"

GameEndChecker::GameEndChecker(GameState* state, GamePageRenderer* gameRenderer, PositionAnalyzer* analyzer) :
	state(state), gameRenderer(gameRenderer), analyzer(analyzer) {}

void GameEndChecker::setContext(GameContext* context) { this->context = context; }
void GameEndChecker::setIsGameOver(bool isGameOver) { this->isGameOver = isGameOver; }

bool GameEndChecker::hasLegalMoves(PieceColor turnColor) {
	for (Square* square : state->getAllSquares()) {
		if (!square->isOccupied()) continue;

		Piece* piece = square->getPiece();
		if (piece->getColor() != turnColor) continue;

		piece->findLegalMoves();
		if (!piece->getLegalMoves().empty()) return true;
	}
	return false;
}

bool GameEndChecker::ifGameIsOver() {
	if (isGameOver) return true;

	if (is50MoveRuleReached()) return true;
	if (isRepetition()) return true;
	if (isMaterialInsufficient()) return true;

	PieceColor turnColor = state->getTurnColor();
	if (hasLegalMoves(turnColor)) return false;
	if (isCheckmate(turnColor)) return true;
	handleStalemate();
	return true;
}

bool GameEndChecker::isCheckmate(PieceColor colorWithNoMoves) {
	if (analyzer->isKingInCheck(colorWithNoMoves)) {
		PieceColor winner = colorWithNoMoves == PieceColor::White ? PieceColor::Black : PieceColor::White;
		endGame(colorStrings.at(winner) + " wins by checkmate");
		return true;
	}
	return false;
}

void GameEndChecker::handleStalemate() {
	endGame("Draw by stalemate");
}

void GameEndChecker::update50MoveCounter(Piece* movingPiece, Square* destination) {
	if (movingPiece->getType() == PieceType::Pawn || destination->isOccupied()) {
		fiftyMoveRuleCounter = 0;
		return;
	}
	fiftyMoveRuleCounter += 1;
}

bool GameEndChecker::is50MoveRuleReached() {
	if (fiftyMoveRuleCounter == 100) {
		endGame("Draw by 50-move rule");
		return true;
	}
	return false;
}

void GameEndChecker::updatePositionCounts() {
	positionCounts[state->getCurrentFen()] += 1;
}

bool GameEndChecker::isRepetition() {
	if (positionCounts[state->getCurrentFen()] == 3) {
		endGame("Draw by repetition");
		return true;
	}
	return false;
}

bool GameEndChecker::areSameColorBishops(Piece* whitePiece, Piece* blackPiece) {
	if (whitePiece->getType() != PieceType::Bishop || blackPiece->getType() != PieceType::Bishop) {
		return false;
	}
	if (whitePiece->getSquare()->getColor() != blackPiece->getSquare()->getColor()) {
		return false;
	}
	return true;
}

bool GameEndChecker::isMaterialInsufficient() {
	auto whitePieces = state->getWhitePieces();
	auto blackPieces = state->getBlackPieces();

	if (whitePieces.size() > 1 || blackPieces.size() > 1) return false;

	if (!whitePieces.empty() && whitePieces.at(0)->getValue() != 3) return false;
	if (!blackPieces.empty() && blackPieces.at(0)->getValue() != 3) return false;

	if (!whitePieces.empty() && !blackPieces.empty()) {
		if (!areSameColorBishops(whitePieces.at(0), blackPieces.at(0))) return false;
	}

	endGame("Draw by insufficient material");
	return true;
}

void GameEndChecker::endGame(QString outcome) {
	context->stopClock();
	gameRenderer->showGameOverWindow(outcome);
}
#include "GameEndChecker.h"
#include "enums.h"
#include "GameState.h"
#include "BoardRenderer.h"
#include "PositionAnalyzer.h"
#include "Square.h"
#include "Piece.h"

GameEndChecker::GameEndChecker(GameState* state, BoardRenderer* board, PositionAnalyzer* analyzer) :
	state(state), board(board), analyzer(analyzer) {
}

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

// nakon poteza: Checkmate, Stalemate, 50 move rule, Repetition, Insufficient material
// nakon neèeg drugog: Resignation, Timeout, Insufficient material vs Timeout, Agreement

void GameEndChecker::ifGameIsOver() {
	if (is50MoveRule()) return;
	if (isRepetition()) return;

	PieceColor turnColor = state->getTurnColor();
	if (hasLegalMoves(turnColor)) return;
	if (isCheckmate(turnColor)) return;
	handleStalemate();
}

bool GameEndChecker::isCheckmate(PieceColor colorWithNoMoves) {
	if (analyzer->isKingInCheck(colorWithNoMoves)) {
		PieceColor winner = colorWithNoMoves == PieceColor::White ? PieceColor::Black : PieceColor::White;
		board->showGameOverWindow(colorStrings.at(winner) + " wins by checkmate");
		return true;
	}
	return false;
}

void GameEndChecker::handleStalemate() {
	board->showGameOverWindow("Draw by stalemate");
}

void GameEndChecker::update50MoveCounter(Piece* movingPiece, Square* destination) {
	if (movingPiece->getType() == PieceType::Pawn || destination->isOccupied()) {
		fiftyMoveRuleCounter = 0;
		return;
	}
	fiftyMoveRuleCounter += 1;
}

bool GameEndChecker::is50MoveRule() {
	if (fiftyMoveRuleCounter == 100) {
		board->showGameOverWindow("Draw by 50-move rule");
		return true;
	}
	return false;
}

void GameEndChecker::updatePositionCounts() {
	positionCounts[state->getCurrentFen()] += 1;
}

bool GameEndChecker::isRepetition() {
	if (positionCounts[state->getCurrentFen()] == 3) {
		board->showGameOverWindow("Draw by repetition");
		return true;
	}
	return false;
}
#include "Rook.h"
#include "Square.h"
#include "GameContext.h"
#include "PositionAnalyzer.h"
#include "GameState.h"

Rook::Rook(PieceColor color, Square* square, int value, QString path, GameContext* context) :
	Piece(PieceType::Rook, color, square, value, path, context) {}

void Rook::findLegalMoves() {
	legalMoves.clear();
	findVisibleSquares();

	for (Square* newSquare : visibleSquares) {
		if (newSquare->isOccupied() && color == newSquare->getPiece()->getColor()) continue;
		if (context->getAnalyzer()->isMoveLegal(this, newSquare)) {
			legalMoves.push_back(newSquare);
		}
	}
}

void Rook::findVisibleSquares() {
	visibleSquares.clear();
	findMovesInDirections({ {1,0}, {0,1}, {-1,0}, {0,-1} });
}

bool Rook::getHasMoved() {
	return hasMoved;
}

void Rook::onMove() {
	if (hasMoved) return;
	hasMoved = true;

	GameState* state = context->getState();

	if (square) {
		state->removeCastlingRight(state->getCorrectCastlingChar(square->getFile(), color));
	}
}

void Rook::onCapture() {
	if (hasMoved) return;

	GameState* state = context->getState();
	state->removeCastlingRight(state->getCorrectCastlingChar(square->getFile(), color));
}
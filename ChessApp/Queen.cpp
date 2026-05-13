#include "Queen.h"
#include "Square.h"
#include "GameContext.h"
#include "PositionAnalyzer.h"

Queen::Queen(PieceColor color, Square* square, QString path, GameContext* context) :
	Piece(PieceType::Queen, color, square, path, context) {}

void Queen::findLegalMoves() {
	legalMoves.clear();
	findVisibleSquares();

	for (Square* newSquare : visibleSquares) {
		if (newSquare->isOccupied() && color == newSquare->getPiece()->getColor()) continue;
		if (context->getAnalyzer()->isMoveLegal(this, newSquare)) {
			legalMoves.push_back(newSquare);
		}
	}
}

void Queen::findVisibleSquares() {
	visibleSquares.clear();
	findMovesInDirections({ {1,1}, {1,-1}, {-1,1}, {-1,-1}, {1,0}, {0,1}, {-1,0}, {0,-1} });
}
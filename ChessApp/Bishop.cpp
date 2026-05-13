#include "Bishop.h"
#include "Square.h"
#include "GameContext.h"
#include "PositionAnalyzer.h"

Bishop::Bishop(PieceColor color, Square* square, QString path, GameContext* context) :
	Piece(PieceType::Bishop, color, square, path, context) {}

void Bishop::findLegalMoves() {
	legalMoves.clear();
	findVisibleSquares();

	for (Square* newSquare : visibleSquares) {
		if (newSquare->isOccupied() && color == newSquare->getPiece()->getColor()) continue;
		if (context->getAnalyzer()->isMoveLegal(this, newSquare)) {
			legalMoves.push_back(newSquare);
		}
	}
}

void Bishop::findVisibleSquares() {
	visibleSquares.clear();
	findMovesInDirections({ {1,1}, {1,-1}, {-1,1}, {-1,-1} });
}
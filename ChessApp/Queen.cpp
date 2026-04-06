#include "Queen.h"
#include "Square.h"

Queen::Queen(PieceColor color, Square* square, QString path, ChessBoard* board) :
	Piece(PieceType::Queen, color, square, path, board) {}

void Queen::findLegalMoves() {
	legalMoves.clear();
	findVisibleSquares();

	for (Square* newSquare : visibleSquares) {
		if (newSquare->isOccupied() && color == newSquare->getPiece()->getColor()) continue;
		if (isMoveLegal(newSquare)) {
			legalMoves.push_back(newSquare);
		}
	}
}

void Queen::findVisibleSquares() {
	visibleSquares.clear();
	findMovesInDirections({ {1,1}, {1,-1}, {-1,1}, {-1,-1}, {1,0}, {0,1}, {-1,0}, {0,-1} });
}
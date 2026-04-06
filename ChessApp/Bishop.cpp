#include "Bishop.h"
#include "Square.h"

Bishop::Bishop(PieceColor color, Square* square, QString path, ChessBoard* board) :
	Piece(PieceType::Bishop, color, square, path, board) {}

void Bishop::findLegalMoves() {
	legalMoves.clear();
	findVisibleSquares();

	for (Square* newSquare : visibleSquares) {
		if (newSquare->isOccupied() && color == newSquare->getPiece()->getColor()) continue;
		if (isMoveLegal(newSquare)) {
			legalMoves.push_back(newSquare);
		}
	}
}

void Bishop::findVisibleSquares() {
	visibleSquares.clear();
	findMovesInDirections({ {1,1}, {1,-1}, {-1,1}, {-1,-1} });
}
#include "Rook.h"
#include "Square.h"

Rook::Rook(PieceColor color, Square* square, QString path, ChessBoard* board) :
	Piece(PieceType::Rook, color, square, path, board) {}

void Rook::findLegalMoves() {
	legalMoves.clear();
	findVisibleSquares();

	for (Square* newSquare : visibleSquares) {
		if (newSquare->isOccupied() && color == newSquare->getPiece()->getColor()) continue;
		if (isMoveLegal(newSquare)) {
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
}
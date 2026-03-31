#include "Bishop.h"

Bishop::Bishop(PieceColor color, Square* square, QString path, ChessBoard* board) :
	Piece(PieceType::Bishop, color, square, path, board) {}

void Bishop::findLegalMoves() {
	legalMoves.clear();
	visibleSquares.clear();
	findMovesInDirections({ {1,1}, {1,-1}, {-1,1}, {-1,-1} });
}
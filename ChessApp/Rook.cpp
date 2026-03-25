#include "Rook.h"

Rook::Rook(PieceColor color, Square* square, QString path, ChessBoard* board) :
	Piece(PieceType::Rook, color, square, path, board) {}

void Rook::findLegalMoves() {
	findMovesInDirections({ {1,0}, {0,1}, {-1,0}, {0,-1} });
}
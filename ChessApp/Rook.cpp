#include "Rook.h"

Rook::Rook(PieceColor color, Square* square, QString path, ChessBoard* board) :
	Piece(PieceType::Rook, color, square, path, board) {
}

void Rook::showLegalMoves() {}
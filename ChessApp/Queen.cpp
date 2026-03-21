#include "Queen.h"

Queen::Queen(PieceColor color, Square* square, QString path, ChessBoard* board) :
	Piece(PieceType::Queen, color, square, path, board) {
}

void Queen::showLegalMoves() {}
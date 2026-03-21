#include "King.h"

King::King(PieceColor color, Square* square, QString path, ChessBoard* board) :
	Piece(PieceType::King, color, square, path, board) {
}

void King::showLegalMoves() {}
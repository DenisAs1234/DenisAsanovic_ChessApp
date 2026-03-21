#include "Knight.h"

Knight::Knight(PieceColor color, Square* square, QString path, ChessBoard* board) :
	Piece(PieceType::Knight, color, square, path, board) {
}

void Knight::showLegalMoves() {

}
#include "Pawn.h"
#include "ChessBoard.h"

Pawn::Pawn(PieceColor color, Square* square, QString path, ChessBoard* board) :
	Piece(PieceType::Pawn, color, square, path, board) {}

void Pawn::showLegalMoves() {
	int thisSquareIndex = (square->getRank() - 1) * 8 + square->getFile();
	if (color == PieceColor::White) {
		board->getAllSquares()[thisSquareIndex + 8]->highlight();
		board->getAllSquares()[thisSquareIndex + 16]->highlight();
	}
	else {
		board->getAllSquares()[thisSquareIndex - 8]->highlight();
		board->getAllSquares()[thisSquareIndex - 16]->highlight();
	}
}
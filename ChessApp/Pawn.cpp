#include "Pawn.h"
#include "ChessBoard.h"
#include "SquareIndex.h"

Pawn::Pawn(PieceColor color, Square* square, QString path, ChessBoard* board) :
	Piece(PieceType::Pawn, color, square, path, board) {}

void Pawn::findLegalMoves() {
	int forwardBy = 1;
	int index;
	while (forwardBy <= 2) {
		if (color == PieceColor::White) {
			index = getSquareIndex(square->getRank() + forwardBy, square->getFile());
		}
		else {
			index = getSquareIndex(square->getRank() - forwardBy, square->getFile());
		}

		Square* newSquare = board->getAllSquares()[index];
		if (newSquare->isOccupied()) break;
		else {
			legalMoves.push_back(newSquare);
		}

		if (!hasMoved) {
			forwardBy += 1;
		}
		else break;
	}
}

void Pawn::onMove() {
	hasMoved = true;
}
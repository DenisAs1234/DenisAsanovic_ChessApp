#include "Pawn.h"
#include "ChessBoard.h"
#include "SquareIndex.h"

Pawn::Pawn(PieceColor color, Square* square, QString path, ChessBoard* board) :
	Piece(PieceType::Pawn, color, square, path, board) {}

void Pawn::findLegalMoves() {
	int rank = square->getRank();
	int file = square->getFile();
	int forwardBy = 1;
	int index;

	while (forwardBy <= 2) {
		index = (color == PieceColor::White)
			? getSquareIndex(rank + forwardBy, file)
			: getSquareIndex(rank - forwardBy, file);

		Square* newSquare = board->getAllSquares()[index];

		if (newSquare->isOccupied()) break;
		legalMoves.push_back(newSquare);

		if (!hasMoved) {
			forwardBy += 1;
		}
		else break;
	}

	vector<pair<int, int>> diagonalCaptures = { {1,1},{1,-1} };
	for (auto& capture : diagonalCaptures) {
		index = (color == PieceColor::White)
			? getSquareIndex(rank + capture.first, file + capture.second)
			: getSquareIndex(rank - capture.first, file + capture.second);

		if (index == -1) continue;
		Square* newCapture = board->getAllSquares()[index];

		if (newCapture->isOccupied()) {
			if (newCapture->getPiece()->getColor() != this->color) {
				legalMoves.push_back(newCapture);
			}
		}
	}

	for (auto& enPassantMove : enPassantMoves) {
		legalMoves.push_back(enPassantMove);
	}
}

void Pawn::onMove() {
	if (hasMoved) return;
	hasMoved = true;

	int rank = square->getRank();

	if (rank == 4 || rank == 5) {
		checkForPawnsNextTo(this);
	}
}

void Pawn::checkForPawnsNextTo(Pawn* passingPawn) {
	int rank = passingPawn->getSquare()->getRank();
	int file = passingPawn->getSquare()->getFile();

	for (int i = -1; i <= 1; i += 2) {
		int index = getSquareIndex(rank, file + i);
		if (index == -1) continue;

		Square* nextToPassingPawn = board->getAllSquares()[index];

		if (!nextToPassingPawn->isOccupied()) continue;

		Piece* piece = nextToPassingPawn->getPiece();
		Pawn* pawn = dynamic_cast<Pawn*>(piece);

		if (pawn && pawn->getColor() != passingPawn->getColor()) {
			index = (passingPawn->getColor() == PieceColor::White)
				? getSquareIndex(rank - 1, file)
				: getSquareIndex(rank + 1, file);

			Square* behindPassingPawn = board->getAllSquares()[index];
			pawn->getEnPassantMoves().push_back(behindPassingPawn);
		}
	}
}

vector<Square*>& Pawn::getEnPassantMoves() {
	return enPassantMoves;
}
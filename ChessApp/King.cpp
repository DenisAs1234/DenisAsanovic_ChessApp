#include "King.h"
#include "Rook.h"
#include "ChessBoard.h"
#include "SquareIndex.h"

King::King(PieceColor color, Square* square, QString path, ChessBoard* board) :
	Piece(PieceType::King, color, square, path, board) {}

void King::findLegalMoves() {
	legalMoves.clear();

	findVisibleSquares();
	auto visibleSquares = getVisibleSquares();

	for (Square* newSquare : visibleSquares) {
		if (newSquare->isOccupied() && newSquare->getPiece()->getColor() == this->color) continue;

		if (newSquare->isSafe(color)) {
			legalMoves.push_back(newSquare);
		}
	}

	if (hasMoved) return;
	if (!square->isSafe(color)) return;

	int rank = square->getRank();
	int file = square->getFile();

	if (canCastle(CastlingType::Short)) {
		int index = getSquareIndex(rank, file + 2);
		Square* castlingSquare = board->getAllSquares()[index];
		legalMoves.push_back(castlingSquare);
	}
	if (canCastle(CastlingType::Long)) {
		int index = getSquareIndex(rank, file - 2);
		Square* castlingSquare = board->getAllSquares()[index];
		legalMoves.push_back(castlingSquare);
	}
}

void King::findVisibleSquares() {
	visibleSquares.clear();

	int rank = square->getRank();
	int file = square->getFile();

	vector<pair<int, int>> directions = { {1,1}, {1,-1}, {-1,1}, {-1,-1}, {1,0}, {0,1}, {-1,0}, {0,-1} };

	for (auto& dir : directions) {
		int newRank = rank + dir.first;
		int newFile = file + dir.second;

		int index = getSquareIndex(newRank, newFile);
		if (index == -1) continue;

		Square* newSquare = board->getAllSquares()[index];
		visibleSquares.push_back(newSquare);
	}
}

void King::onMove() {
	if (!hasMoved) {
		hasMoved = true;
	}

	if (color == PieceColor::White) {
		board->setWhiteKingPos(square);
		return;
	}
	board->setBlackKingPos(square);
}

bool King::canCastle(CastlingType castlingType) {
	int rank = (color == PieceColor::White) ? 1 : 8;
	int file = square->getFile();

	int step = (castlingType == CastlingType::Short) ? 1 : -1;
	file += step;
	auto allSquares = board->getAllSquares();

	while (file >= 0 && file <= 7) {
		int index = getSquareIndex(rank, file);
		Rook* rook = dynamic_cast<Rook*>(allSquares[index]->getPiece());

		if (allSquares[index]->isOccupied() && !rook) return false;
		if (!allSquares[index]->isSafe(color)) return false;
		if (!allSquares[index]->isOccupied()) {
			file += step;
			continue;
		}
		if (rook->getHasMoved()) return false;
		return true;
	}
	return false;
}
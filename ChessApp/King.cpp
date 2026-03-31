#include "King.h"
#include "ChessBoard.h"
#include "SquareIndex.h"

King::King(PieceColor color, Square* square, QString path, ChessBoard* board) :
	Piece(PieceType::King, color, square, path, board) {}

void King::findLegalMoves() {
	legalMoves.clear();

	findVisibleSquares();
	auto visibleSquares = getVisibleSquares();

	for(Square* newSquare : visibleSquares) {
		if (newSquare->isOccupied() && newSquare->getPiece()->getColor() == this->color) continue;
		
		if (newSquare->isSafe(color)) {
			legalMoves.push_back(newSquare);
		}
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
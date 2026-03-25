#include "King.h"
#include "ChessBoard.h"
#include "SquareIndex.h"

King::King(PieceColor color, Square* square, QString path, ChessBoard* board) :
	Piece(PieceType::King, color, square, path, board) {}

void King::findLegalMoves() {
	int rank = square->getRank();
	int file = square->getFile();

	vector<pair<int, int>> directions = { {1,1}, {1,-1}, {-1,1}, {-1,-1}, {1,0}, {0,1}, {-1,0}, {0,-1} };

	for (auto& dir : directions) {
		int newRank = rank + dir.first;
		int newFile = file + dir.second;

		int index = getSquareIndex(newRank, newFile);
		if (index != -1) {
			if (!board->getAllSquares()[index]->isOccupied()) {
				legalMoves.push_back(board->getAllSquares()[index]);
			}
		}
	}
}
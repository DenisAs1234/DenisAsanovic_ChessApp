#include "Piece.h"
#include "Square.h"
#include "ChessBoard.h"
#include "enums.h"
using namespace std;

Piece::Piece(PieceType type, PieceColor color, Square* square, QString path, ChessBoard* board) : 
	type(type), color(color), square(square), path(path), board(board) {}

PieceType Piece::getType() {
	return type;
}

Square* Piece::getSquare() {
	return square;
}

QString Piece::getPath() {
	return path;
}

vector<Square*> Piece::getLegalMoves() {
	return legalMoves;
}

void Piece::resetLegalMoves() {
	for (Square* legalMove : legalMoves) {
		legalMoves.pop_back();
	}
}

void Piece::findMovesInDirections(vector<pair<int, int>> directions) {
	int rank = square->getRank();
	int file = square->getFile();

	for (auto& dir : directions) {
		int newRank = rank + dir.first;
		int newFile = file + dir.second;

		while (true) {
			int index = getSquareIndex(newRank, newFile);

			if (index == -1) break;
			else if (board->getAllSquares()[index]->isOccupied()) break;

			legalMoves.push_back(board->getAllSquares()[index]);

			newRank += dir.first;
			newFile += dir.second;
		}
	}
}

void Piece::moveTo(Square* newSquare) {
	square->setPiece(nullptr);
	square = newSquare;
	newSquare->setPiece(this);
	setPos(newSquare->getX() + 5, newSquare->getY() + 7);
	onMove();
}
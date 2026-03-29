#include "Piece.h"
#include "Square.h"
#include "Pawn.h"
#include "ChessBoard.h"
#include "enums.h"

Piece::Piece(PieceType type, PieceColor color, Square* square, QString path, ChessBoard* board) : 
	type(type), color(color), square(square), path(path), board(board) {
}

PieceType Piece::getType() {
	return type;
}

PieceColor Piece::getColor() {
	return color;
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
	legalMoves.clear();
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

			Square* newSquare = board->getAllSquares()[index];
			if (newSquare->isOccupied()) {
				if (this->color == newSquare->getPiece()->getColor()) break;
				legalMoves.push_back(newSquare);
				break;
			}

			legalMoves.push_back(newSquare);

			newRank += dir.first;
			newFile += dir.second;
		}
	}
}

void Piece::moveTo(Square* destination) {
	square->setPiece(nullptr);

	Pawn* pawn = dynamic_cast<Pawn*>(this);
	bool isEnPassantMove = false;
	if (pawn) {
		auto& enPassantMoves = pawn->getEnPassantMoves();
		isEnPassantMove = find(enPassantMoves.begin(), enPassantMoves.end(), destination) 
			!= enPassantMoves.end();
	}

	if (destination->isOccupied()) {
		board->getScene()->removeItem(destination->getPiece());
	}

	if (isEnPassantMove) {
		int index = (color == PieceColor::White)
			? getSquareIndex(destination->getRank() - 1, destination->getFile())
			: getSquareIndex(destination->getRank() + 1, destination->getFile());

		Square* enPassantPos = board->getAllSquares()[index];
		board->getScene()->removeItem(enPassantPos->getPiece());
		enPassantPos->setPiece(nullptr);
		pawn->getEnPassantMoves().clear();
	}

	square = destination;
	destination->setPiece(this);
	setPos(destination->getX() + 5, destination->getY() + 7);
	board->clearEnPassants();
	onMove();
}
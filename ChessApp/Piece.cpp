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

vector<Square*> Piece::getVisibleSquares() {
	return visibleSquares;
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
			visibleSquares.push_back(newSquare);

			if (newSquare->isOccupied()) {
				visibleSquares.push_back(newSquare);
				break;
			}

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
		if (isEnPassantMove) {
			pawn->executeEnPassant(destination);
		}
	}

	if (destination->isOccupied()) {
		board->getScene()->removeItem(destination->getPiece());
	}

	square = destination;
	destination->setPiece(this);
	setPos(destination->getX() + 5, destination->getY() + 7);
	board->clearEnPassants();
	onMove();
}

bool Piece::isMoveLegal(Square* destination) {
	square->setPiece(nullptr);

	Square* originalSquare = square;
	square = destination;

	Piece* onDestination = destination->getPiece();
	destination->setPiece(this);

	bool isKingInCheck = board->isKingInCheck(color);

	square = originalSquare;
	square->setPiece(this);
	if (onDestination) {
		destination->setPiece(onDestination);
	}
	else {
		destination->setPiece(nullptr);
	}

	return isKingInCheck;
}
#include "Piece.h"
#include "Square.h"
#include "pieceTypes.h"
#include "GameContext.h"
#include "GameState.h"
#include "SpecialMoveHandler.h"
#include "BoardRenderer.h"
#include "GameEndChecker.h"
#include "enums.h"

Piece::Piece(PieceType type, PieceColor color, Square* square, QString path, GameContext* context) :
	type(type), color(color), square(square), path(path), context(context) {}

PieceType Piece::getType() {
	return type;
}

PieceColor Piece::getColor() {
	return color;
}

Square* Piece::getSquare() {
	return square;
}

void Piece::setSquare(Square* square) {
	this->square = square;
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

			Square* newSquare = context->getState()->getAllSquares()[index];
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
	auto specialMoves = context->getSpecialMoves();
	context->getGameEndings()->update50MoveCounter(this, destination);

	Pawn* pawn = dynamic_cast<Pawn*>(this);
	bool isPromotion = false;
	if (pawn) {
		specialMoves->checkIfEnPassant(pawn, destination);
		isPromotion = specialMoves->checkIfPromotion(pawn, destination);
	}

	King* king = dynamic_cast<King*>(this);
	if (king) {
		specialMoves->checkIfCastlingMove(king, destination);
	}

	if (destination->isOccupied()) {
		Piece* toBeCaptured = destination->getPiece();
		Rook* rook = dynamic_cast<Rook*>(toBeCaptured);
		if (rook) {
			rook->onCapture();
		}

		context->getBoard()->removePieceFromBoard(toBeCaptured);
	}

	if (isPromotion) {
		destination->setPiece(pawn->getPromotedTo());
	}
	else {
		square = destination;
		destination->setPiece(this);
		setPos(destination->getX() + 5, destination->getY() + 7);
	}

	specialMoves->clearEnPassants();
	context->getState()->resetEnPassantSquare();
	onMove();
}
/*
bool Piece::isMoveLegal(Square* destination) {
	square->setPiece(nullptr);
	
	Square* originalSquare = square;
	square = destination;

	King* king = dynamic_cast<King*>(this);
	if (king) {
		if (color == PieceColor::White) { state->setWhiteKingPos(destination); }
		else { state->setBlackKingPos(destination); }
	}
	
	Piece* onDestination = destination->getPiece();
	destination->setPiece(this);

	bool isKingInCheck = logic->isKingInCheck(color);

	square = originalSquare;
	square->setPiece(this);
	if (onDestination) {
		destination->setPiece(onDestination);
	}
	else {
		destination->setPiece(nullptr);
	}

	if (king) {
		if (color == PieceColor::White) { state->setWhiteKingPos(originalSquare); }
		else { state->setBlackKingPos(originalSquare); }
	}

	return isKingInCheck ? false : true;
}*/
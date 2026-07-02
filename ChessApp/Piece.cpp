#include "Piece.h"
#include "Square.h"
#include "pieceTypes.h"
#include "GameContext.h"
#include "GameState.h"
#include "SpecialMoveHandler.h"
#include "GamePageRenderer.h"
#include "GameEndChecker.h"
#include "enums.h"

Piece::Piece(PieceType type, PieceColor color, Square* square, int value, QString path, GameContext* context) 
	: type(type), color(color), square(square), value(value), path(path), context(context) {}

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

int Piece::getValue() {
	return value;
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

Square* Piece::getDestination() {
	return destination;
}

void Piece::resetDestination() {
	destination = nullptr;
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

void Piece::moveTo(Square* clickedSquare) {
	auto specialMoves = context->getSpecialMoves();
	context->getGameEndings()->update50MoveCounter(this, clickedSquare);

	King* king = dynamic_cast<King*>(this);
	Square* castlingDestination = nullptr;
	if (king) {
		castlingDestination = specialMoves->checkIfCastlingMove(king, clickedSquare);
	}
	if (king && castlingDestination) {
		this->destination = castlingDestination;

		specialMoves->executeCastling();

		specialMoves->clearEnPassants();
		context->getState()->resetEnPassantSquare();

		onMove();
		return;
	}

	square->setPiece(nullptr);

	if (clickedSquare->isOccupied()) {
		if (context->getVariant() == ChessVariant::Atomic) {
			specialMoves->executeAtomicCapture(clickedSquare, this);
		}

		Piece* toBeCaptured = clickedSquare->getPiece();
		
		Rook* rook = dynamic_cast<Rook*>(toBeCaptured);
		if (rook) {
			rook->onCapture();
		}

		context->capturePiece(toBeCaptured);
	}

	Pawn* pawn = dynamic_cast<Pawn*>(this);
	bool isPromotion = false;
	if (pawn) {
		specialMoves->checkIfEnPassant(pawn, clickedSquare);
		isPromotion = specialMoves->checkIfPromotion(pawn, clickedSquare);
	}

	if (isPromotion) {
		clickedSquare->setPiece(pawn->getPromotedTo());
	}

	else if (this->square) {
		square = clickedSquare;
		clickedSquare->setPiece(this);
		setPos(clickedSquare->getX() + 5, clickedSquare->getY() + 7);
	}

	specialMoves->clearEnPassants();
	context->getState()->resetEnPassantSquare();
	onMove();
}
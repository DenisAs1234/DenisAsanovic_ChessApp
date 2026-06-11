#include "King.h"
#include "GameContext.h"
#include "GameState.h"
#include "PositionAnalyzer.h"
#include "SpecialMoveHandler.h"
#include "Square.h"
#include "SquareIndex.h"

King::King(PieceColor color, Square* square, int value, QString path, GameContext* context) :
	Piece(PieceType::King, color, square, value, path, context) {}

void King::findLegalMoves() {
	legalMoves.clear();
	castlingMoves.clear();

	findVisibleSquares();
	auto visibleSquares = getVisibleSquares();

	for (Square* newSquare : visibleSquares) {
		if (context->getVariant() == ChessVariant::Atomic && newSquare->isOccupied()) continue;

		if (newSquare->isOccupied() && newSquare->getPiece()->getColor() == this->color) continue;

		if (context->getAnalyzer()->isMoveLegal(this, newSquare)) {
			legalMoves.push_back(newSquare);
		}
	}

	if (hasMoved) return;
	if (context->getAnalyzer()->isKingInCheck(color)) return;

	int rank = square->getRank();
	int file = square->getFile();

	if (context->getSpecialMoves()->canCastle(CastlingType::Short)) {
		int castlingRookFile = context->getSpecialMoves()->getInitialKingsideRookFile();
		int index = getSquareIndex(rank, castlingRookFile);
		Square* castlingRookSquare = context->getState()->getAllSquares()[index];
		legalMoves.push_back(castlingRookSquare);
	}

	if (context->getSpecialMoves()->canCastle(CastlingType::Long)) {
		int castlingRookFile = context->getSpecialMoves()->getInitialQueensideRookFile();
		int index = getSquareIndex(rank, castlingRookFile);
		Square* castlingRookSquare = context->getState()->getAllSquares()[index];
		legalMoves.push_back(castlingRookSquare);
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

		Square* newSquare = context->getState()->getAllSquares()[index];
		visibleSquares.push_back(newSquare);
	}
}

void King::onMove() {
	if (!hasMoved) {
		hasMoved = true;
	}

	GameState* state = context->getState();

	if (color == PieceColor::White) {
		state->setWhiteKingPos(square);
		state->removeCastlingRight('K');
		state->removeCastlingRight('Q');
		return;
	}
	state->setBlackKingPos(square);
	state->removeCastlingRight('k');
	state->removeCastlingRight('q');
}

vector<pair<Square*, Rook*>>& King::getCastlingMoves() {
	return castlingMoves;
}
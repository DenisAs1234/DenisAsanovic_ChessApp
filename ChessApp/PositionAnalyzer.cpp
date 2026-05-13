#include "PositionAnalyzer.h"
#include "King.h"
#include "Square.h"
#include "GameState.h"

PositionAnalyzer::PositionAnalyzer(GameState* state, BoardRenderer* board) :
	state(state), board(board) {
}

bool PositionAnalyzer::isMoveLegal(Piece* movingPiece, Square* destination) {
	movingPiece->getSquare()->setPiece(nullptr);

	Square* originalSquare = movingPiece->getSquare();
	movingPiece->setSquare(destination);

	King* king = dynamic_cast<King*>(movingPiece);
	if (king) {
		if (king->getColor() == PieceColor::White) { state->setWhiteKingPos(destination); }
		else { state->setBlackKingPos(destination); }
	}

	Piece* onDestination = destination->getPiece();
	destination->setPiece(movingPiece);

	bool isMoveLegal = !isKingInCheck(movingPiece->getColor());

	movingPiece->setSquare(originalSquare);
	originalSquare->setPiece(movingPiece);
	if (onDestination) {
		destination->setPiece(onDestination);
	}
	else {
		destination->setPiece(nullptr);
	}

	if (king) {
		if (king->getColor() == PieceColor::White) { state->setWhiteKingPos(originalSquare); }
		else { state->setBlackKingPos(originalSquare); }
	}

	return isMoveLegal;
}

bool PositionAnalyzer::isKingInCheck(PieceColor turnColor) {
	if (turnColor == PieceColor::White) {
		return !isSquareSafe(state->getWhiteKingPos());
	}
	return !isSquareSafe(state->getBlackKingPos());
}

bool PositionAnalyzer::isSquareSafe(Square* kingPos) {
	PieceColor attackerColor = (state->getTurnColor() == PieceColor::White)
		? PieceColor::Black : PieceColor::White;
	auto allSquares = state->getAllSquares();

	for (Square* square : allSquares) {
		if (!square->isOccupied()) continue;

		Piece* piece = square->getPiece();
		if (piece->getColor() != attackerColor) continue;

		piece->findVisibleSquares();
		auto attackedSquares = piece->getVisibleSquares();

		if (find(attackedSquares.begin(), attackedSquares.end(), kingPos) != attackedSquares.end()) {
			return false;
		}
	}
	return true;
}
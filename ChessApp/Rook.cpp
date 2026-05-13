#include "Rook.h"
#include "Square.h"
#include "GameContext.h"
#include "PositionAnalyzer.h"
#include "GameState.h"

Rook::Rook(PieceColor color, Square* square, QString path, GameContext* context) :
	Piece(PieceType::Rook, color, square, path, context) {}

void Rook::findLegalMoves() {
	legalMoves.clear();
	findVisibleSquares();

	for (Square* newSquare : visibleSquares) {
		if (newSquare->isOccupied() && color == newSquare->getPiece()->getColor()) continue;
		if (context->getAnalyzer()->isMoveLegal(this, newSquare)) {
			legalMoves.push_back(newSquare);
		}
	}
}

void Rook::findVisibleSquares() {
	visibleSquares.clear();
	findMovesInDirections({ {1,0}, {0,1}, {-1,0}, {0,-1} });
}

bool Rook::getHasMoved() {
	return hasMoved;
}

void Rook::onMove() {
	if (hasMoved) return;
	hasMoved = true;
	GameState* state = context->getState();
	state->removeCastlingRight(state->getCorrectCastlingChar(square->getFile(), color));
}

void Rook::onCapture() {
	if (hasMoved) return;
	GameState* state = context->getState();
	state->removeCastlingRight(state->getCorrectCastlingChar(square->getFile(), color));
}
/*
char Rook::getCorrectCastlingChar() {
	int kingFile = board->getWhiteKingPos()->getFile();
	if (square->getFile() < kingFile) {
		return color == PieceColor::White ? 'Q' : 'q';
	}
	return color == PieceColor::White ? 'K' : 'k';
}*/
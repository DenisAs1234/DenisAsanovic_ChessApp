#include "PieceFactory.h"
#include "GameContext.h"
#include "GameState.h"

PieceFactory::PieceFactory(GameContext* context) : context(context) {};

Piece* PieceFactory::createPiece(PieceType type, PieceColor color, Square* square, QString path) {
	Piece* piece;
	if (type == PieceType::Knight) {
		piece = new Knight(color, square, 3, path, context);
	}
	else if (type == PieceType::Bishop) {
		piece = new Bishop(color, square, 3, path, context);
	}
	else if (type == PieceType::Rook) {
		piece = new Rook(color, square, 5, path, context);
	}
	else if (type == PieceType::Queen) {
		piece = new Queen(color, square, 9, path, context);
	}
	else {
		piece = new King(color, square, 0, path, context);
	}
	context->getState()->addPiece(piece);
	return piece;
}
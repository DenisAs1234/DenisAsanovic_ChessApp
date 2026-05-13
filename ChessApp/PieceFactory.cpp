#include "PieceFactory.h"

PieceFactory::PieceFactory(GameContext* context) : context(context) {};

Piece* PieceFactory::createPiece(PieceType type, PieceColor color, Square* square, QString path) {
	if (type == PieceType::Knight) {
		return new Knight(color, square, path, context);
	}
	else if (type == PieceType::Bishop) {
		return new Bishop(color, square, path, context);
	}
	else if (type == PieceType::Rook) {
		return new Rook(color, square, path, context);
	}
	else if (type == PieceType::Queen) {
		return new Queen(color, square, path, context);
	}
	else {
		return new King(color, square, path, context);
	}
}
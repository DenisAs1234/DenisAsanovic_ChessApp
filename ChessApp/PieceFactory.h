#pragma once

#include "Piece.h"
#include "pieceTypes.h"

class PieceFactory {
	GameContext* context;
public:
	PieceFactory(GameContext* context);
	Piece* createPiece(PieceType type, PieceColor color, Square* square, QString path);
};
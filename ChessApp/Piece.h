#pragma once

#include "enums.h"
#include "Square.h"

class Piece {
	PieceType type;
	PieceColor color;
	Square* square;
public:
	Piece(PieceType type, PieceColor color, Square* square);
	Square* getSquare();
};
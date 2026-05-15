#pragma once

#include "Piece.h"
#include "enums.h"

class Knight : public Piece {
public:
	Knight(PieceColor color, Square* square, int value, QString path, GameContext* context);
	void findLegalMoves() override;
	void findVisibleSquares() override;
};
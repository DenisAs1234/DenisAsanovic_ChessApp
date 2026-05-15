#pragma once

#include "Piece.h"
#include "enums.h"

class Bishop : public Piece {
public:
	Bishop(PieceColor color, Square* square, int value, QString path, GameContext* context);
	void findLegalMoves() override;
	void findVisibleSquares() override;
};

#pragma once

#include "Piece.h"
#include "enums.h"

class Bishop : public Piece {
public:
	Bishop(PieceColor color, Square* square, QString path, ChessBoard* board);
	void findLegalMoves() override;
	void findVisibleSquares() override;
};

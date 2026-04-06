#pragma once

#include "Piece.h"
#include "enums.h"

class Knight : public Piece {
public:
	Knight(PieceColor color, Square* square, QString path, ChessBoard* board);
	void findLegalMoves() override;
	void findVisibleSquares() override;
};
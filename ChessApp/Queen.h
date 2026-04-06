#pragma once

#include "Piece.h"
#include "enums.h"

class Queen : public Piece {
public:
	Queen(PieceColor color, Square* square, QString path, ChessBoard* board);
	void findLegalMoves() override;
	void findVisibleSquares() override;
};

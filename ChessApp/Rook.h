#pragma once

#include "Piece.h"
#include "enums.h"

class Rook : public Piece {
public:
	Rook(PieceColor color, Square* square, QString path, ChessBoard* board);
	void showLegalMoves() override;
};

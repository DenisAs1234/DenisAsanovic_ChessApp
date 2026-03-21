#pragma once

#include "Piece.h"
#include "enums.h"

class King : public Piece {
public:
	King(PieceColor color, Square* square, QString path, ChessBoard* board);
	void showLegalMoves() override;
};

#pragma once

#include "Piece.h"
#include "enums.h"

class Pawn : public Piece {
public:
	Pawn(PieceColor color, Square* square, QString path, ChessBoard* board);
	void showLegalMoves() override;
};
#pragma once

#include "Piece.h"
#include "enums.h"

class Pawn : public Piece {
	bool hasMoved = false;
public:
	Pawn(PieceColor color, Square* square, QString path, ChessBoard* board);
	void findLegalMoves() override;
	void onMove() override;
};
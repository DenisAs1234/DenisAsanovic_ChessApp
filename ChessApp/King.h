#pragma once

#include "Piece.h"
#include "enums.h"

class King : public Piece {
	bool hasMoved = false;
public:
	King(PieceColor color, Square* square, QString path, ChessBoard* board);
	void findLegalMoves() override;
	void findVisibleSquares() override;
	void onMove() override;
	bool canCastle(CastlingType castlingType);
};

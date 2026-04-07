#pragma once

#include "Piece.h"
#include "enums.h"

class Rook : public Piece {
	bool hasMoved = false;
public:
	Rook(PieceColor color, Square* square, QString path, ChessBoard* board);
	void findLegalMoves() override;
	void findVisibleSquares() override;
	bool getHasMoved();
	void onMove() override;
};

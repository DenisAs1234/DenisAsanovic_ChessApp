#pragma once

#include "Piece.h"
#include "enums.h"

class Rook : public Piece {
	bool hasMoved = false;
public:
	Rook(PieceColor color, Square* square, QString path, GameContext* context);
	void findLegalMoves() override;
	void findVisibleSquares() override;
	bool getHasMoved();
	void onMove() override;
	void onCapture();
	//char getCorrectCastlingChar();
};

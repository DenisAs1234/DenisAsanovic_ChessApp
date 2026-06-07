#pragma once

#include "Piece.h"
#include "enums.h"

class Rook : public Piece {
	bool hasMoved = false;
	//bool capturePending = false;
public:
	Rook(PieceColor color, Square* square, int value, QString path, GameContext* context);
	void findLegalMoves() override;
	void findVisibleSquares() override;

	bool getHasMoved();
	/*
	bool isCapturePending();
	void setCapturePending(bool value);*/

	void onMove() override;
	void onCapture();
};

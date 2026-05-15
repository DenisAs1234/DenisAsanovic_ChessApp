#pragma once

#include "Piece.h"
#include "enums.h"

class Rook;

class King : public Piece {
	bool hasMoved = false;
	vector<pair<Square*, Rook*>> castlingMoves;
public:
	King(PieceColor color, Square* square, int value, QString path, GameContext* context);
	void findLegalMoves() override;
	void findVisibleSquares() override;
	void onMove() override;
	vector<pair<Square*, Rook*>>& getCastlingMoves();
	/*
	bool canCastle(CastlingType castlingType);
	void checkIfCastlingMove(Square* destination);
	void executeCastling(Square* destination);*/
};

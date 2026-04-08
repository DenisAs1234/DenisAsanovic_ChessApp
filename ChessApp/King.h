#pragma once

#include "Piece.h"
#include "enums.h"

class Rook;

class King : public Piece {
	bool hasMoved = false;
	vector<pair<Square*, Rook*>> castlingMoves;
public:
	King(PieceColor color, Square* square, QString path, ChessBoard* board);
	void findLegalMoves() override;
	void findVisibleSquares() override;
	void onMove() override;

	bool canCastle(CastlingType castlingType);
	void checkIfCastlingMove(Square* destination);
	void executeCastling(Square* destination);
};

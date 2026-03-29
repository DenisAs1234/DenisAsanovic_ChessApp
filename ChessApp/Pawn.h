#pragma once

#include "Piece.h"
#include "enums.h"

class Pawn : public Piece {
	bool hasMoved = false;
	vector<Square*> enPassantMoves;
public:
	Pawn(PieceColor color, Square* square, QString path, ChessBoard* board);
	void findLegalMoves() override;
	void onMove() override;
	void checkForPawnsNextTo(Pawn* passingPawn);
	vector<Square*>& getEnPassantMoves();
};
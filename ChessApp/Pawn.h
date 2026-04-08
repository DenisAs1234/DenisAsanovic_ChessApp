#pragma once

#include "Piece.h"
#include "enums.h"

class Pawn : public Piece {
	bool hasMoved = false;
	vector<Square*> enPassantMoves;
public:
	Pawn(PieceColor color, Square* square, QString path, ChessBoard* board);
	void findLegalMoves() override;
	void findVisibleSquares() override;
	void getCaptures(int rank, int file);
	void onMove() override;

	void checkForPawnsNextTo(Pawn* passingPawn);
	vector<Square*>& getEnPassantMoves();

	void checkIfEnPassant(Square* destination);
	void executeEnPassant(Square* destination);
	bool isEnPassantLegal(Square* destination);
};
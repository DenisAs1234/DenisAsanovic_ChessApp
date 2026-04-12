#pragma once

#include "Piece.h"
#include "enums.h"

class PromotionType;

class Pawn : public Piece {
	bool hasMoved = false;
	vector<Square*> enPassantMoves;
	Piece* promotedTo;
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

	bool checkIfPromotion(Square* destination);
	void createPromotionPiece(PieceType type, Square* destination);
	void drawPromotionSelector(Square* destination);
	Piece* getPromotedTo();
};
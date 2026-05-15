#pragma once

#include "Piece.h"
#include "enums.h"

class PromotionType;

class Pawn : public Piece {
	bool hasMoved = false;
	Square* enPassantMove = nullptr;
	Piece* promotedTo;
public:
	Pawn(PieceColor color, Square* square, int value, QString path, GameContext* context);
	void findLegalMoves() override;
	void findVisibleSquares() override;
	void getCaptures(int rank, int file);
	void onMove() override;

	Square* getEnPassantMove();
	void setEnPassantMove(Square* enPassant);
	/*
	void checkForPawnsNextTo(Pawn* passingPawn);

	void checkIfEnPassant(Square* destination);
	void executeEnPassant(Square* destination);
	bool isEnPassantLegal(Square* destination);

	bool checkIfPromotion(Square* destination);
	void createPromotionPiece(PieceType type, Square* destination);
	void drawPromotionSelector(Square* destination);*/
	Piece* getPromotedTo();
	void setPromotedTo(Piece* piece);
};
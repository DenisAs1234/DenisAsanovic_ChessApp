#pragma once

#include "enums.h"

class GameContext;
class GameState;
class BoardRenderer;
class PositionAnalyzer;
class Pawn;
class King;
class Square;
class Piece;
class PieceFactory;

class SpecialMoveHandler {
	GameContext* context;
	GameState* state;
	BoardRenderer* board;
	PositionAnalyzer* analyzer;
	PieceFactory* factory;

	int castlingRookFrom = -1;
	int castlingRookTo = -1;

	Pawn* promotingPawn;
	int promotionPiece = -1;
	int pendingPromotionFrom = -1;
	int pendingPromotionTo = -1;
	bool promotionPending = false;

public:
	SpecialMoveHandler(GameState* state, BoardRenderer* board, PositionAnalyzer* analyzer);

	void setContext(GameContext* context);
	void setFactory(PieceFactory* factory);

	void checkForPawnsNextTo(Pawn* passingPawn);
	void checkIfEnPassant(Pawn* movingPawn, Square* destination);
	void executeEnPassant(Pawn* movingPawn, Square* destination);
	bool isEnPassantLegal(Pawn* movingPawn, Square* destination);
	void clearEnPassants();

	bool isPromotionPending();
	int getPendingPromotionFrom();
	int getPendingPromotionTo();
	void setPendingPromotionFrom(int fromIndex);
	void setPendingPromotionTo(int toIndex);
	int getPromotionPiece();

	bool checkIfPromotion(Pawn* promotingPawn, Square* destination);
	void executePromotion(Pawn* promotingPawn, PieceType type, Square* destination);
	void executePromotionFromNetwork(PieceColor color, PieceType type, Square* destination);

	int getCastlingRookFrom();
	int getCastlingRookTo();

	bool canCastle(King* king, CastlingType castlingType);
	void checkIfCastlingMove(King* king, Square* destination);
	void executeCastling(King* king, Square* destination);

	vector<Piece*> findAdjacentPieces(Square* captureSquare);
	void executeAtomicCapture(Square* captureSquare, Piece* capturingPiece);

	void clearSpecialMoveData();
};
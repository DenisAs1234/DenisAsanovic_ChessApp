#pragma once

#include "enums.h"
#include "CastlingData.h"

class GameContext;
class GameState;
class GamePageRenderer;
class PositionAnalyzer;
class Pawn;
class King;
class Square;
class Piece;
class PieceFactory;

class SpecialMoveHandler {
	GameContext* context;
	GameState* state;
	GamePageRenderer* gameRenderer;
	PositionAnalyzer* analyzer;
	PieceFactory* factory;

	int initialKingFile;
	int initialQueensideRookFile;
	int initialKingsideRookFile;

	CastlingData castlingData;
	int castlingRookFrom = -1;
	int castlingRookTo = -1;

	Pawn* promotingPawn;
	int promotionPiece = -1;
	int pendingPromotionFrom = -1;
	int pendingPromotionTo = -1;
	bool promotionPending = false;

public:
	SpecialMoveHandler(GameState* state, GamePageRenderer* gameRenderer, PositionAnalyzer* analyzer);

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

	void setInitialKingFile(int file);
	void setInitialKingsideRookFile(int file);
	void setInitialQueensideRookFile(int file);
	int getInitialKingsideRookFile();
	int getInitialQueensideRookFile();

	int getCastlingRookFrom();
	int getCastlingRookTo();

	bool canCastle(CastlingType castlingType);
	bool isRookUnobstructed(int currentFile, int destinationFile);

	Square* checkIfCastlingMove(King* king, Square* clickedSquare);
	void prepareCastling(King* king, Square* kingDestination, Square* rookStart, Square* rookDestination);
	void executeCastling();
	//void executeCastling(King* king, Square* kingDestination);
	/*void executeCastling(
		King* king,
		Square* kingDestination,
		Square* rookStart,
		Square* rookDestination);*/

	vector<Piece*> findAdjacentPieces(Square* captureSquare);
	void executeAtomicCapture(Square* captureSquare, Piece* capturingPiece);

	void clearSpecialMoveData();
};
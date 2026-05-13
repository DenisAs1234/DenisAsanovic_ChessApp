#pragma once

#include "enums.h"

class GameState;
class BoardRenderer;
class PositionAnalyzer;
class Pawn;
class King;
class Square;
class PieceFactory;

class SpecialMoveHandler {
	GameState* state;
	BoardRenderer* board;
	PositionAnalyzer* analyzer;
	PieceFactory* factory;
public:
	SpecialMoveHandler(GameState* state, BoardRenderer* board, PositionAnalyzer* analyzer);

	void setFactory(PieceFactory* factory);

	void checkForPawnsNextTo(Pawn* passingPawn);
	void checkIfEnPassant(Pawn* movingPawn, Square* destination);
	void executeEnPassant(Pawn* movingPawn, Square* destination);
	bool isEnPassantLegal(Pawn* movingPawn, Square* destination);
	void clearEnPassants();

	bool checkIfPromotion(Pawn* promotingPawn, Square* destination);
	void executePromotion(Pawn* promotingPawn, PieceType type, Square* destination);

	bool canCastle(King* king, CastlingType castlingType);
	void checkIfCastlingMove(King* king, Square* destination);
	void executeCastling(King* king, Square* destination);
};
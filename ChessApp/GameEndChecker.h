#pragma once

#include "enums.h"

class GameState;
class BoardRenderer;
class PositionAnalyzer;

class GameEndChecker {
	GameState* state;
	BoardRenderer* board;
	PositionAnalyzer* analyzer;
public:
	GameEndChecker(GameState* state, BoardRenderer* board, PositionAnalyzer* analyzer);
	bool hasLegalMoves(PieceColor turnColor);
	void ifGameIsOver();
	bool isCheckmate(PieceColor colorWithNoMoves);
	void handleStalemate();
};
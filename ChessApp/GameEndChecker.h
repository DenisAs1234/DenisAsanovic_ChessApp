#pragma once

#include "enums.h"
#include<qhash.h>

class GameState;
class BoardRenderer;
class PositionAnalyzer;
class Piece;
class Square;

class GameEndChecker {
	GameState* state;
	BoardRenderer* board;
	PositionAnalyzer* analyzer;
	int fiftyMoveRuleCounter = 0;
	QHash<QString, int> positionCounts;
public:
	GameEndChecker(GameState* state, BoardRenderer* board, PositionAnalyzer* analyzer);

	bool hasLegalMoves(PieceColor turnColor);
	void ifGameIsOver();

	bool isCheckmate(PieceColor colorWithNoMoves);
	void handleStalemate();

	void update50MoveCounter(Piece* movingPiece, Square* destination);
	bool is50MoveRule();

	void updatePositionCounts();
	bool isRepetition();
};
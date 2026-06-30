#pragma once

#include "enums.h"
#include <qhash.h>

class GameContext;
class GameState;
class GamePageRenderer;
class PositionAnalyzer;
class Piece;
class Square;

class GameEndChecker {
	GameState* state;
	GamePageRenderer* gameRenderer;
	PositionAnalyzer* analyzer;
	int fiftyMoveRuleCounter = 0;
	QHash<QString, int> positionCounts;
	GameContext* context;
	bool isGameOver = false;

public:
	GameEndChecker(GameState* state, GamePageRenderer* gameRenderer, PositionAnalyzer* analyzer);

	void setContext(GameContext* context);
	void setIsGameOver(bool isGameOver);

	bool hasLegalMoves(PieceColor turnColor);
	bool ifGameIsOver();

	bool isCheckmate(PieceColor colorWithNoMoves);
	void handleStalemate();

	void update50MoveCounter(Piece* movingPiece, Square* destination);
	bool is50MoveRuleReached();

	void updatePositionCounts();
	bool isRepetition();

	bool areSameColorBishops(Piece* whitePiece, Piece* blackPiece);
	bool isMaterialInsufficient();

	void endGame(QString outcome);
};
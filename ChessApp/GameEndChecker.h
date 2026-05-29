#pragma once

#include "enums.h"
#include<qhash.h>

class GameContext;
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
	GameContext* context;
public:
	GameEndChecker(GameState* state, BoardRenderer* board, PositionAnalyzer* analyzer);

	void setContext(GameContext* context);

	bool hasLegalMoves(PieceColor turnColor);
	void ifGameIsOver();

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
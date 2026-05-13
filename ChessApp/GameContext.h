#pragma once

#include "enums.h"

class GameState;
class BoardRenderer;
class PositionAnalyzer;
class SpecialMoveHandler;
class GameEndChecker;
class Square;
class PieceFactory;

class GameContext {
	Square* selectedSquare;
	GameState* state;
	BoardRenderer* board;
	PositionAnalyzer* analyzer;
	SpecialMoveHandler* specialMoves;
	GameEndChecker* gameEndings;
	PieceFactory* factory;
public:
	GameContext(GameState* state, BoardRenderer* board, PositionAnalyzer* analyzer,
		SpecialMoveHandler* specialMoves, GameEndChecker* gameEndings);

	void setFactory(PieceFactory* factory);

	Square* getSelectedSquare();
	GameState* getState();
	BoardRenderer* getBoard();
	PositionAnalyzer* getAnalyzer();
	SpecialMoveHandler* getSpecialMoves();
	GameEndChecker* getGameEndings();

	void setupStartingPosition();
	//bool hasLegalMoves();

	void selectSquare(Square* square);
	void resetSelectedSquare();
	void handleSquareClick(Square* square);
};
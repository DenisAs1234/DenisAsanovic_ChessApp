#pragma once

#include "enums.h"
#include "Player.h"

class GameState;
class BoardRenderer;
class PositionAnalyzer;
class SpecialMoveHandler;
class GameEndChecker;
class Square;
class PieceFactory;

class GameContext {
	Player whitePlayer;
	Player blackPlayer;
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

	Player getWhitePlayer();
	Player getBlackPlayer();

	GameState* getState();
	BoardRenderer* getBoard();

	PositionAnalyzer* getAnalyzer();
	SpecialMoveHandler* getSpecialMoves();
	GameEndChecker* getGameEndings();

	void setupStartingPosition();

	Square* getSelectedSquare();
	void selectSquare(Square* square);
	void resetSelectedSquare();
	void handleSquareClick(Square* square);

	void offerDraw(Player player);
	void acceptDraw();
	void resign(Player player);
};
#pragma once

#include "enums.h"
#include "Player.h"
#include <QTimer>
#include <QElapsedTimer>
#include <QObject>

class GameState;
class BoardRenderer;
class PositionAnalyzer;
class SpecialMoveHandler;
class GameEndChecker;
class Square;
class PieceFactory;

class GameContext : public QObject {
	Q_OBJECT
	Player whitePlayer;
	Player blackPlayer;
	Square* selectedSquare;
	GameState* state;
	BoardRenderer* board;
	PositionAnalyzer* analyzer;
	SpecialMoveHandler* specialMoves;
	GameEndChecker* gameEndings;
	PieceFactory* factory;
	bool drawOfferActive = false;
	QTimer* clockTimer;
	QElapsedTimer elapsedTimer;
public:
	GameContext(GameState* state, BoardRenderer* board, PositionAnalyzer* analyzer,
		SpecialMoveHandler* specialMoves, GameEndChecker* gameEndings);

	void setFactory(PieceFactory* factory);

	Player getWhitePlayer();
	Player getBlackPlayer();
	Player& getTurnPlayer();

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
	void resign(Player player);

	void updateClock();
};
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
	PieceColor localPlayerColor;

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

	bool applyingNetworkMove = false;

public:
	GameContext(GameState* state, BoardRenderer* board, PositionAnalyzer* analyzer,
		SpecialMoveHandler* specialMoves, GameEndChecker* gameEndings);

	void setFactory(PieceFactory* factory);

	void setLocalPlayerColor(PieceColor color);
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

	void updateGameStateAfterMove();

	void offerDraw(Player player);
	void resign(Player player);

	void startClock();
	void updateClock();
	void stopClock();

	void setApplyingNetworkMove(bool value);
	bool isApplyingNetworkMove();

signals:
	void movePlayed(int fromIndex, int toIndex);
};
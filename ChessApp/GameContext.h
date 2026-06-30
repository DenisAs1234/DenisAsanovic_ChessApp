#pragma once

#include "enums.h"
#include "Player.h"
#include "TimeControl.h"
#include <QTimer>
#include <QElapsedTimer>
#include <QObject>

class GameState;
class GamePageRenderer;
class PositionAnalyzer;
class SpecialMoveHandler;
class GameEndChecker;
class Square;
class Piece;
class PieceFactory;

class GameContext : public QObject {
	Q_OBJECT
	Player whitePlayer;
	Player blackPlayer;
	PieceColor localPlayerColor;

	ChessVariant variant;
	TimeControl timeControl;

	Square* selectedSquare;

	GameState* state;
	GamePageRenderer* gameRenderer;
	PositionAnalyzer* analyzer;
	SpecialMoveHandler* specialMoves;
	GameEndChecker* gameEndings;

	PieceFactory* factory;

	bool drawOfferActive = false;
	bool drawButtonActive = true;

	QTimer* clockTimer;
	QElapsedTimer elapsedTimer;

	bool applyingNetworkMove = false;

public:
	GameContext(GameState* state, GamePageRenderer* gameRenderer, PositionAnalyzer* analyzer,
		SpecialMoveHandler* specialMoves, GameEndChecker* gameEndings);

	void setFactory(PieceFactory* factory);

	ChessVariant getVariant();
	void setVariant(ChessVariant variant);
	void setTimeControl(TimeControl timeControl);

	PieceColor getLocalPlayerColor();
	void setLocalPlayerColor(PieceColor color);
	Player getWhitePlayer();
	Player getBlackPlayer();
	Player& getTurnPlayer();

	GameState* getState();
	GamePageRenderer* getGameRenderer();
	PositionAnalyzer* getAnalyzer();
	SpecialMoveHandler* getSpecialMoves();
	GameEndChecker* getGameEndings();

	void setupStartingPosition(QString orderOfPieces);

	Square* getSelectedSquare();
	void selectSquare(Square* square);
	void resetSelectedSquare();
	void handleSquareClick(Square* square);

	void capturePiece(Piece* piece);

	bool updateGameStateAfterMove();
	void finishPromotionMove();

	void offerDraw(Player oferrer);
	void receiveDrawOffer(PieceColor oferrer);
	void receiveDrawAccepted();
	void clearDrawOffer();

	void resign(Player player);
	void receiveResignation(PieceColor loser);

	void backToLobby();

	void startClock();
	void updateClock();
	void stopClock();
	void addIncrement();

	void setApplyingNetworkMove(bool value);
	bool isApplyingNetworkMove();

signals:
	void movePlayed(int fromIndex, 
					int toIndex, 
					int castlingRookFrom, 
					int castlingRookTo,
					int promotionPiece);

	void drawOffered(PieceColor offerer);
	void drawAccepted();
	void playerResigned(PieceColor loser);
	void gameFinished();
	void returnToLobbyRequested();
};
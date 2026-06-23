#pragma once

#include<QGraphicsScene>
#include<QGraphicsRectItem>
#include<vector>
#include "enums.h"
using namespace std;

class Piece;
class Square;
class PromotionOption;
class Pawn;
class GameContext;
class Player;
class GameButton;

class GamePageRenderer : public QGraphicsRectItem {
	QGraphicsScene* scene;
	bool boardActive = true;

	Square* lastMoveFrom = nullptr;
	Square* lastMoveTo = nullptr;

	QGraphicsTextItem* localNickname;
	QGraphicsTextItem* opponentNickname;

	QGraphicsTextItem* whiteClock;
	QGraphicsTextItem* blackClock;
	QString lastDisplayedTime;

	vector<QGraphicsRectItem*> promotionMenu;
	bool promotionMenuActive = false;

	QGraphicsTextItem* drawOfferMsg = nullptr;

	QGraphicsItemGroup* gameOverWindow = nullptr;
	GameButton* closeButton = nullptr;
	GameButton* backToLobbyButton = nullptr;

	GameContext* context;

public:
	GamePageRenderer(QGraphicsScene* scene);

	QGraphicsScene* getScene();
	void setContext(GameContext* context);

	void drawBoard(PieceColor localPlayerColor);
	void drawPiece(Piece* piece);
	void removeFromBoard(Piece* piece);

	void setPlayerNames(QString myName, QString opponentName);
	void drawNicknames();
	void drawButtons();

	void showDrawOfferMessage(PieceColor offerer);
	void removeDrawOfferMessage();

	void resetColor(Square* square);
	void highlightSelected(Square* square);
	void highlightLegalMove(Square* square);
	void highlightLastMove(Square* startingSquare, Square* destination);

	void resetColorOfLegalMoves(vector<Square*> legalMoves);
	void resetHighlightedMove();

	void drawPromotionMenu(Pawn* promotingPawn, Square* destination);
	vector<QGraphicsRectItem*>& getPromotionMenu();
	bool getPromotionMenuActive();
	void setPromotionMenuActive(bool isActive);
	void removePromotionMenu();

	void drawClocks();
	void updateClockDisplay(Player& player);

	bool getBoardActive();

	void showGameOverWindow(QString outcome);
	void removeGameOverWindow();
};
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

class BoardRenderer : public QGraphicsRectItem {
	QGraphicsScene* scene;
	bool boardActive = true;

	Square* lastMoveFrom = nullptr;
	Square* lastMoveTo = nullptr;

	QGraphicsTextItem* whiteClock;
	QGraphicsTextItem* blackClock;
	QString lastDisplayedTime;

	vector<QGraphicsRectItem*> promotionMenu;
	bool promotionMenuActive = false;

	QGraphicsTextItem* drawOfferMsg = nullptr;

	GameContext* context;

public:
	BoardRenderer(QGraphicsScene* scene);

	QGraphicsScene* getScene();
	void setContext(GameContext* context);

	void drawBoard(PieceColor localPlayerColor);
	void drawPiece(Piece* piece);
	void removeFromBoard(Piece* piece);

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
};
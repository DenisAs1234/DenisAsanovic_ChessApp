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

class BoardRenderer : public QGraphicsRectItem {
	QGraphicsScene* scene;
	vector<QGraphicsRectItem*> promotionMenu;
	bool boardActive = true;
	bool promotionMenuActive = false;
	GameContext* context;
public:
	BoardRenderer(QGraphicsScene* scene);

	QGraphicsScene* getScene();
	void setContext(GameContext* context);

	void drawBoard();
	void drawPiece(Piece* piece);
	void removeFromBoard(Piece* piece);
	void drawButtons();

	void resetColor(Square* square);
	void highlightSelected(Square* square);
	void highlightMove(Square* square);
	void resetColorOfLegalMoves(vector<Square*> legalMoves);

	void drawPromotionMenu(Pawn* promotingPawn, Square* destination);
	vector<QGraphicsRectItem*>& getPromotionMenu();
	bool getPromotionMenuActive();
	void setPromotionMenuActive(bool isActive);
	void removePromotionMenu();

	bool getBoardActive();
	void showGameOverWindow(QString outcome);
};
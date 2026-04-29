#pragma once

#include "Square.h"
#include "Piece.h"
#include<QGraphicsScene>
#include<QGraphicsPixmapItem>

class PromotionType;

class ChessBoard : public QGraphicsRectItem {
	QGraphicsScene* scene;
	vector<Square*> allSquares;
	Square* selectedSquare;
	PieceColor turnColor = PieceColor::White;
	Square* whiteKingPos;
	Square* blackKingPos;
	vector<QGraphicsRectItem*> promotionMenu;
	bool promotionMenuActive = false;
public:
	ChessBoard(QGraphicsScene* scene);

	void drawBoard();
	void drawPiece(Piece* piece);

	void setStartingPosition();
	void selectSquare(Square* square);

	QGraphicsScene* getScene();
	vector<Square*> getAllSquares();
	Square* getSelectedSquare();
	PieceColor getTurnColor();

	Square* getWhiteKingPos();
	Square* getBlackKingPos();
	void setWhiteKingPos(Square* square);
	void setBlackKingPos(Square* square);

	vector<QGraphicsRectItem*>& getPromotionMenu();
	bool getPromotionMenuActive();
	void setPromotionMenuActive(bool isActive);
	void removePromotionMenu();

	bool isKingInCheck(PieceColor turnColor);

	void resetSelectedSquare();
	void resetColorOfLegalMoves(vector<Square*> legalMoves);

	void clearEnPassants();
	void switchTurn();
};
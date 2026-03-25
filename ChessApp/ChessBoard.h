#pragma once
#include "Piece.h"
#include "Square.h"
#include<QGraphicsScene>
#include<QGraphicsPixmapItem>
using namespace std;

class ChessBoard : public QGraphicsRectItem {
	QGraphicsScene* scene;
	vector<Square*> allSquares;
	Square* selectedSquare;
public:
	ChessBoard(QGraphicsScene* scene);
	void drawBoard();
	void drawPiece(Piece* piece);
	void setStartingPosition();
	void selectSquare(Square* square);
	QGraphicsScene* getScene();
	vector<Square*> getAllSquares();
	Square* getSelectedSquare();
	void resetSelectedSquare();
	void resetColorOfLegalMoves(vector<Square*> legalMoves);
};
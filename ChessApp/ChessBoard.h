#pragma once
#include "Square.h"
#include<QGraphicsScene>
#include<QGraphicsPixmapItem>
using namespace std;

class ChessBoard : public QGraphicsRectItem {
	QGraphicsScene* scene;
	vector<Square*> allSquares;
public:
	ChessBoard(QGraphicsScene* scene);
	void drawBoard();
	void drawPiece(QString path, qreal x, qreal y);
	void setStartingPosition();
	QGraphicsScene* getScene();
};
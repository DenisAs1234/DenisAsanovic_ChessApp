#pragma once
#include<QGraphicsScene>

class ChessBoard {
	QGraphicsScene* scene;

public:
	ChessBoard(QGraphicsScene* scene);
	void drawChessBoard();
	QGraphicsScene* getScene();
};
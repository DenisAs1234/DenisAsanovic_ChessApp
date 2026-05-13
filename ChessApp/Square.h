#pragma once

#include "enums.h"
#include<QGraphicsRectItem>
#include<QGraphicsSceneMouseEvent>

class Piece;
class GameContext;

class Square : public QGraphicsRectItem {
	File file;
	int rank;
	SquareColor color;
	qreal xPos;
	qreal yPos;
	Piece* piece;
	GameContext* context;
public:
	Square(File file, int rank, SquareColor color, qreal xPos, qreal yPos, GameContext* context,
		QGraphicsItem* parent = nullptr);

	int getFile();
	int getRank();
	qreal getX();
	qreal getY();
	SquareColor getColor();

	Piece* getPiece();
	void setPiece(Piece* piece);
	/*
	void resetColor();
	void highlightSelected();
	void highlightMove();*/

	bool isOccupied();
	//bool isSafe();

	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
};

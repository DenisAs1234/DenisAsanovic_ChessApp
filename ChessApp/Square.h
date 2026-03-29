#pragma once

#include "enums.h"
#include<QGraphicsRectItem>
#include<QGraphicsSceneMouseEvent>

class Piece;
class ChessBoard;

class Square : public QGraphicsRectItem {
	File file;
	int rank;
	SquareColor color;
	qreal xPos;
	qreal yPos;
	Piece* piece;
	ChessBoard* board;
public:
	Square(File file, int rank, SquareColor color, qreal xPos, qreal yPos,
		ChessBoard* board, QGraphicsItem* parent = nullptr);

	int getFile();
	int getRank();
	qreal getX();
	qreal getY();

	Piece* getPiece();
	void setPiece(Piece* piece);

	void resetColor();
	void highlightSelected();
	void highlightMove();

	bool isOccupied();

	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
};

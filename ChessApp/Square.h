#pragma once

#include "enums.h"
#include<QGraphicsRectItem>

class Square : public QGraphicsRectItem {
	File file;
	int rank;
	SquareColor color;
	qreal xPos;
	qreal yPos;

public:
	Square(File file, int rank, SquareColor color, qreal xPos, qreal yPos, QGraphicsItem* parent = nullptr);
};

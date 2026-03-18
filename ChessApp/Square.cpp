#include "Square.h"
#include<QGraphicsRectItem>
#include<QBrush>
#include<QPen>

Square::Square(File file, int rank, SquareColor color, qreal xPos, qreal yPos, QGraphicsItem* parent) :
	QGraphicsRectItem(parent), file(file), rank(rank), color(color), xPos(xPos), yPos(yPos) {

	QColor darkSquare(194, 106, 62);
	QColor lightSquare(247, 183, 151);
	setRect(0, 0, 90, 90);
	setBrush(QBrush(color == SquareColor::dark ? darkSquare : lightSquare));
	setPen(Qt::NoPen);
	setPos(xPos, yPos);
}

qreal Square::getX() {
	return pos().x();
}

qreal Square::getY() {
	return pos().y();
}
#include "Square.h"
#include "Piece.h"
#include "GameContext.h"
#include "PositionAnalyzer.h"
#include<QGraphicsRectItem>
#include<QBrush>
#include<QPen>

Square::Square(File file, int rank, SquareColor color, qreal xPos, qreal yPos, GameContext* context, 
	QGraphicsItem* parent) : QGraphicsRectItem(parent), file(file), rank(rank), color(color), 
	xPos(xPos), yPos(yPos), context(context) {

	QColor darkSquare(194, 106, 62);
	QColor lightSquare(247, 183, 151);

	setRect(0, 0, 90, 90);
	setBrush(QBrush(color == SquareColor::dark ? darkSquare : lightSquare));
	setPen(Qt::NoPen);
	setPos(xPos, yPos);

	setAcceptedMouseButtons(Qt::LeftButton);
}

int Square::getFile() { return static_cast<int>(file); }

int Square::getRank() { return rank; }

qreal Square::getX() { return pos().x(); }

qreal Square::getY() { return pos().y(); }

SquareColor Square::getColor() { return color; }

Piece* Square::getPiece() {
    return piece;
}

void Square::setPiece(Piece* piece) {
    this->piece = piece;
}

bool Square::isOccupied() {
    return this->getPiece();
}
/*
bool Square::isSafe() {
    return context->getAnalyzer()->isSquareSafe(this);
}*/

void Square::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    context->handleSquareClick(this);
}
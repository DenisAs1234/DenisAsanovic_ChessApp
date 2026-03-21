#include "Square.h"
#include "Piece.h"
#include "ChessBoard.h"
#include<QGraphicsRectItem>
#include<QBrush>
#include<QPen>

Square::Square(File file, int rank, SquareColor color, qreal xPos, qreal yPos, ChessBoard* board, 
	QGraphicsItem* parent) :
	QGraphicsRectItem(parent), file(file), rank(rank), color(color), xPos(xPos), yPos(yPos), board(board) {

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

void Square::mousePressEvent(QGraphicsSceneMouseEvent* event) {
	board->selectSquare(this);
	QGraphicsRectItem::mousePressEvent(event);
}

Piece* Square::getPiece() {
	return piece;
}

void Square::setPiece(Piece* piece) {
	this->piece = piece;
}

void Square::resetColor() {
	QColor darkSquare(194, 106, 62);
	QColor lightSquare(247, 183, 151);
	setBrush(QBrush(color == SquareColor::dark ? darkSquare : lightSquare));
}

void Square::highlight() {
	setBrush(QBrush(color == SquareColor::dark ? QColor(252, 186, 3) : QColor(250, 209, 5)));
}
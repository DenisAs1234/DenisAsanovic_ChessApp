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
	Square* selectedSquare = board->getSelectedSquare();

	if (!selectedSquare) {
		board->selectSquare(this);
		return;
	}

	Piece* piece = selectedSquare->getPiece();
	vector<Square*> legalMoves = piece->getLegalMoves();
	bool isLegalMove = find(legalMoves.begin(), legalMoves.end(), this) != legalMoves.end();

	if (isLegalMove) {
		piece->moveTo(this);
		board->resetSelectedSquare();
		board->resetColorOfLegalMoves(legalMoves);
	}
	else {
		board->selectSquare(this);
	}

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

void Square::highlightSelected() {
	setBrush(QBrush(color == SquareColor::dark ? QColor(252, 186, 3) : QColor(250, 209, 5)));
}

void Square::highlightMove() {
	setBrush(QBrush(color == SquareColor::dark ? QColor(30, 156, 52) : QColor(52, 235, 85)));
}

bool Square::isOccupied() {
	return this->getPiece() != nullptr;
}
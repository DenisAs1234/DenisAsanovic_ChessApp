#include "Square.h"
#include "Piece.h"
#include "King.h"
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
    if (!this->isOccupied()) {
        setBrush(QBrush(color == SquareColor::dark ? QColor(30, 156, 52) : QColor(52, 235, 85)));
        return;
    }
    setBrush(QBrush(color == SquareColor::dark ? QColor(39, 117, 242) : QColor(66, 139, 255)));
}

bool Square::isOccupied() {
    return this->getPiece();
}

bool Square::isSafe(PieceColor turnColor) {
    PieceColor attackerColor = turnColor == PieceColor::White ? PieceColor::Black : PieceColor::White;
    auto allSquares = board->getAllSquares();

    for (Square* square : allSquares) {
        if (!square->isOccupied()) continue;

        Piece* piece = square->getPiece();
        if (piece->getColor() != attackerColor) continue;

        piece->findVisibleSquares();
        auto attackedSquares = piece->getVisibleSquares();

        if (find(attackedSquares.begin(), attackedSquares.end(), this) != attackedSquares.end()) {
            return false;
        }
    }
    return true;
}

void Square::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (board->getPromotionMenuActive()) return;

    Square* selectedSquare = board->getSelectedSquare();

    if (!selectedSquare) {
        if (this->isOccupied() && this->getPiece()->getColor() != board->getTurnColor()) return;
        board->selectSquare(this);
        return;
    }

    Piece* selectedPiece = selectedSquare->getPiece();

    auto legalMoves = selectedPiece->getLegalMoves();
    if (find(legalMoves.begin(), legalMoves.end(), this) != legalMoves.end()) {
        selectedPiece->moveTo(this);

        board->resetSelectedSquare();
        board->resetColorOfLegalMoves(legalMoves);

        board->switchTurn();
        board->hasLegalMoves(board->getTurnColor());
        return;
    }

    board->resetSelectedSquare();
    board->resetColorOfLegalMoves(legalMoves);

    if (this->isOccupied()) {
        if (this->getPiece()->getColor() != board->getTurnColor()) return;
        board->selectSquare(this);
        return;
    }
}
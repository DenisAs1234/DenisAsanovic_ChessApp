#pragma once

#include "enums.h"
#include<QGraphicsSceneMouseEvent>
#include<QGraphicsPixmapItem>

class Square;
class ChessBoard;

class Piece : public QGraphicsPixmapItem {
protected:
	PieceType type;
	PieceColor color;
	Square* square;
	QString path;
	ChessBoard* board;
public:
	Piece(PieceType type, PieceColor color, Square* square, QString path, ChessBoard* board);
	Square* getSquare();
	QString getPath();
	virtual void showLegalMoves() = 0;
};
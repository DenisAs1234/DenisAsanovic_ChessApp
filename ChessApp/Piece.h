#pragma once

#include "enums.h"
#include "SquareIndex.h"
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
	vector<Square*> legalMoves;
public:
	Piece(PieceType type, PieceColor color, Square* square, QString path, ChessBoard* board);
	PieceType getType();
	Square* getSquare();
	QString getPath();
	vector<Square*> getLegalMoves();
	void resetLegalMoves();
	virtual void findLegalMoves() = 0;
	void findMovesInDirections(vector<pair<int, int>> directions);
	void moveTo(Square* square);
	virtual void onMove() {};
};
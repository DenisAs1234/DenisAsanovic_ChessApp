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
	vector<Square*> visibleSquares;
public:
	Piece(PieceType type, PieceColor color, Square* square, QString path, ChessBoard* board);

	PieceType getType();
	PieceColor getColor();
	Square* getSquare();
	QString getPath();

	vector<Square*> getVisibleSquares();
	vector<Square*> getLegalMoves();

	virtual void findLegalMoves() = 0;
	virtual void findVisibleSquares() = 0;
	void findMovesInDirections(vector<pair<int, int>> directions);

	void moveTo(Square* square);
	virtual void onMove() {};
	bool isMoveLegal(Square* square);
};
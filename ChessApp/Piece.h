#pragma once

#include "enums.h"
#include "SquareIndex.h"
#include<QGraphicsSceneMouseEvent>
#include<QGraphicsPixmapItem>

class Square;
class GameContext;

class Piece : public QGraphicsPixmapItem {
protected:
	PieceType type;
	PieceColor color;
	Square* square;
	QString path;
	GameContext* context;
	vector<Square*> legalMoves;
	vector<Square*> visibleSquares;
public:
	Piece(PieceType type, PieceColor color, Square* square, QString path, GameContext* context);

	PieceType getType();
	PieceColor getColor();
	Square* getSquare();
	void setSquare(Square* square);
	QString getPath();

	vector<Square*> getLegalMoves();
	vector<Square*> getVisibleSquares();

	virtual void findLegalMoves() = 0;
	virtual void findVisibleSquares() = 0;
	void findMovesInDirections(vector<pair<int, int>> directions);

	void moveTo(Square* square);
	virtual void onMove() {};
	//bool isMoveLegal(Square* square);
};
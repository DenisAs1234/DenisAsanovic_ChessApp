#pragma once

#include "enums.h"
#include "PieceFactory.h"
#include<QGraphicsRectItem>
#include<QGraphicsPixmapItem>
#include<QGraphicsSceneMouseEvent>

class Pawn;
class ChessBoard;

class PromotionType : public QGraphicsPixmapItem {
	PieceType type;
	QGraphicsRectItem* rect;
	Pawn* pawn;
	Square* destination;
	ChessBoard* board;
public:
	PromotionType(PieceType type, Pawn* pawn, Square* destination, ChessBoard* board);
	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
};
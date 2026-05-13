#pragma once

#include "enums.h"
#include<QGraphicsRectItem>
#include<QGraphicsPixmapItem>
#include<QGraphicsSceneMouseEvent>

class Pawn;
class Square;
class GameContext;

class PromotionOption : public QGraphicsPixmapItem {
	PieceType type;
	QGraphicsRectItem* rect;
	Pawn* pawn;
	Square* destination;
	GameContext* context;
public:
	PromotionOption(PieceType type, Pawn* pawn, Square* destination, GameContext* context);
	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
};
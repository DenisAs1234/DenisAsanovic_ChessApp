#pragma once

#include<QGraphicsRectItem>
#include "Player.h"

class GameContext;

class GameButton : public QGraphicsRectItem {
	ButtonAction action;
	QString text;
	Player player;
	GameContext* context;
	qreal x;
	qreal y;
public:
	GameButton(ButtonAction action, QString text, Player player, GameContext* context, qreal x, qreal y);
	void applyStyle(ButtonAction action);
	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
};
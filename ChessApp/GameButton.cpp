#include "GameButton.h"
#include "GameContext.h"
#include<QBrush>
#include<QPen>
#include<QFont>

GameButton::GameButton(ButtonAction action, QString text, Player player,
	GameContext* context, qreal x, qreal y)
	: action(action), player(player), context(context) {
	setRect(x, y, 200, 50);

	applyStyle(action);

	auto* label = new QGraphicsTextItem(text, this);
	label->setDefaultTextColor(Qt::black);

	QFont font("Arial", 16, QFont::Bold);
	label->setFont(font);

	QRectF r = rect();
	label->setPos(r.x() + (r.width() - label->boundingRect().width()) / 2,
		r.y() + (r.height() - label->boundingRect().height()) / 2);
}

void GameButton::applyStyle(ButtonAction action) {
	QColor fill;
	QColor border;

	if (action == ButtonAction::OfferDraw /* || action == ButtonAction::AcceptDraw */) {
		fill = QColor(180, 220, 255);
		border = QColor(0, 120, 255);
	}
	else if (action == ButtonAction::Resign) {
		fill = QColor(255, 200, 200);
		border = QColor(220, 50, 50);
	}

	setBrush(QBrush(fill));
	setPen(QPen(border, 2));

	setRect(rect());
}

void GameButton::mousePressEvent(QGraphicsSceneMouseEvent* event) {
	switch (action) {

	case ButtonAction::OfferDraw: 
		context->offerDraw(player);
		break;
	/*
	case ButtonAction::AcceptDraw: 
		context->acceptDraw();
		break;
		*/
	case ButtonAction::Resign:
		context->resign(player);
		break;
	}
}
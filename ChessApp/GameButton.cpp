#include "GameButton.h"
#include "GameContext.h"
#include "GamePageRenderer.h"
#include<QBrush>
#include<QPen>
#include<QFont>

GameButton::GameButton(ButtonAction action, QString text, Player player,
	GameContext* context, qreal x, qreal y)
	: action(action), player(player), context(context) {

	auto* label = new QGraphicsTextItem(text, this);

	if (action == ButtonAction::CloseGameOverWindow) {
		setRect(x, y, 30, 30);
		label->setDefaultTextColor(Qt::white);
	}
	else {
		setRect(x, y, 200, 50);
		label->setDefaultTextColor(Qt::black);
	}

	applyStyle(action);

	QFont font("Arial", 16, QFont::Bold);
	label->setFont(font);

	QRectF r = rect();
	label->setPos(r.x() + (r.width() - label->boundingRect().width()) / 2,
		r.y() + (r.height() - label->boundingRect().height()) / 2);
}

void GameButton::applyStyle(ButtonAction action) {
	QColor fill;
	QColor border;

	switch (action) {

	case ButtonAction::OfferDraw:
		fill = QColor(180, 220, 255);
		border = QColor(0, 120, 255);
		break;

	case ButtonAction::Resign:
		fill = QColor(255, 200, 200);
		border = QColor(220, 50, 50);
		break;

	case ButtonAction::CloseGameOverWindow:
		fill = QColor(240, 31, 31);
		border = QColor(0, 0, 0);
		break;

	case ButtonAction::BackToLobby:
		fill = QColor(192, 194, 188);
		border = QColor(109, 110, 107);
		break;
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

	case ButtonAction::Resign:
		context->resign(player);
		break;

	case ButtonAction::CloseGameOverWindow:
		context->getGameRenderer()->removeGameOverWindow();
		break;

	case ButtonAction::BackToLobby:
		context->backToLobby();
		break;
	}
}
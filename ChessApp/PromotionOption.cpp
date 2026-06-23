#include "PromotionOption.h"
#include "Pawn.h"
#include "GameContext.h"
#include "SpecialMoveHandler.h"
#include "GamePageRenderer.h"

PromotionOption::PromotionOption(PieceType type, Pawn* pawn, Square* destination, GameContext* context) :
	type(type), pawn(pawn), destination(destination), context(context) {}

void PromotionOption::mousePressEvent(QGraphicsSceneMouseEvent* event) {
	context->getSpecialMoves()->executePromotion(pawn, type, destination);
	context->getGameRenderer()->setPromotionMenuActive(false);
	context->getGameRenderer()->removePromotionMenu();
}
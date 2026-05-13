#include "PromotionOption.h"
#include "Pawn.h"
#include "GameContext.h"
#include "SpecialMoveHandler.h"
#include "BoardRenderer.h"

PromotionOption::PromotionOption(PieceType type, Pawn* pawn, Square* destination, GameContext* context) :
	type(type), pawn(pawn), destination(destination), context(context) {}

void PromotionOption::mousePressEvent(QGraphicsSceneMouseEvent* event) {
	context->getSpecialMoves()->executePromotion(pawn, type, destination);
	context->getBoard()->setPromotionMenuActive(false);
	context->getBoard()->removePromotionMenu();
}
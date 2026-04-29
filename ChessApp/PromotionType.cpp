#include "PromotionType.h"
#include "Pawn.h"
#include "ChessBoard.h"

PromotionType::PromotionType(PieceType type, Pawn* pawn, Square* destination, ChessBoard* board)
    : type(type), pawn(pawn), destination(destination), board(board) {
}

void PromotionType::mousePressEvent(QGraphicsSceneMouseEvent* event) {
	pawn->createPromotionPiece(type, destination);
	board->setPromotionMenuActive(false);
	board->removePromotionMenu();
}
#pragma once

#include "enums.h"

class GameState;
class BoardRenderer;
class Square;
class Piece;

class PositionAnalyzer {
	GameState* state;
	BoardRenderer* board;
public:
	PositionAnalyzer(GameState* state, BoardRenderer* board);
	bool isMoveLegal(Piece* movingPiece, Square* destination);
	bool isKingInCheck(PieceColor turnColor);
	bool isSquareSafe(Square* square);
};
#pragma once

#include "enums.h"

class GameState;
class GamePageRenderer;
class Square;
class Piece;

class PositionAnalyzer {
	GameState* state;
	GamePageRenderer* gameRenderer;
public:
	PositionAnalyzer(GameState* state, GamePageRenderer* gameRenderer);
	bool isMoveLegal(Piece* movingPiece, Square* destination);
	bool isKingInCheck(PieceColor turnColor);
	bool isSquareSafe(Square* square);
};
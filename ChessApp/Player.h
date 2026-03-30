#pragma once
#include "enums.h"

class Player {
	PieceColor color;
	bool isYourTurn = false;
public:
	Player(PieceColor color);
	void setIsYourTurn(bool isYourTurn);
};
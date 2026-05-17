#pragma once

#include "enums.h"

class Player {
	PieceColor color;
	QString nickname;
public:
	Player(PieceColor color, QString nickname);

	PieceColor getColor();
	QString getNickname();
};
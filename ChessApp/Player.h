#pragma once

#include "enums.h"

class Player {
	PieceColor color;
	QString nickname;
	qint64 remainingTime;
public:
	Player(PieceColor color, QString nickname, qint64 remainingTime);

	PieceColor getColor();
	QString getNickname();

	void setRemainingTime(qint64 remainingTime);
	qint64 getRemainingTime();
};
#include "Player.h"

Player::Player(PieceColor color, QString nickname) : 
	color(color), nickname(nickname) {};

PieceColor Player::getColor() { return color; }

QString Player::getNickname() { return nickname; }

void Player::setRemainingTime(qint64 remainingTime) { this->remainingTime = remainingTime; }

qint64 Player::getRemainingTime() { return remainingTime; }
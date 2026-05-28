#include "Player.h"

Player::Player(PieceColor color, QString nickname, qint64 remainingTime) : 
	color(color), nickname(nickname), remainingTime(remainingTime) {};

PieceColor Player::getColor() { return color; }

QString Player::getNickname() { return nickname; }

void Player::setRemainingTime(qint64 remainingTime) { this->remainingTime = remainingTime; }

qint64 Player::getRemainingTime() { return remainingTime; }
#include "Player.h"

Player::Player(PieceColor color, QString nickname) : color(color), nickname(nickname) {};

PieceColor Player::getColor() { return color; }

QString Player::getNickname() { return nickname; }
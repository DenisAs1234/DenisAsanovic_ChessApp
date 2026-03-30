#include "Player.h"

Player::Player(PieceColor color) : color(color) {};

void Player::setIsYourTurn(bool isYourTurn) { this->isYourTurn = isYourTurn; }
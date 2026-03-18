#include "Piece.h"
#include "Square.h"
#include "enums.h"

using namespace std;

Piece::Piece(PieceType type, PieceColor color, Square* square) : 
	type(type), color(color), square(square) {}

Square* Piece::getSquare() {
	return square;
}
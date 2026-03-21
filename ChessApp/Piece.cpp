#include "Piece.h"
#include "Square.h"
#include "enums.h"
using namespace std;

Piece::Piece(PieceType type, PieceColor color, Square* square, QString path, ChessBoard* board) : 
	type(type), color(color), square(square), path(path), board(board) {}

Square* Piece::getSquare() {
	return square;
}

QString Piece::getPath() {
	return path;
}
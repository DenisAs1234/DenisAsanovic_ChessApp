#pragma once

#include "Piece.h"
#include "pieceTypes.h"

inline Piece* createPiece(PieceType type, PieceColor color, Square* square, QString path, ChessBoard* board) 
{
	if (type == PieceType::Knight) {
		return new Knight(color, square, path, board);
	}
	else if (type == PieceType::Bishop) {
		return new Bishop(color, square, path, board);
	}
	else if (type == PieceType::Rook) {
		return new Rook(color, square, path, board);
	}
	else if (type == PieceType::Queen) {
		return new Queen(color, square, path, board);
	}
	else {
		return new King(color, square, path, board);
	}
}
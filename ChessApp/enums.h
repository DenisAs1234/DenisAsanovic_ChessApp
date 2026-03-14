#pragma once

enum class File {
	A, B, C, D, E, F, G, H
};

enum class PieceType {
	Pawn, Knight, Bishop, Rook, Queen, King
};

enum class PieceColor {
	White, Black
};

enum class SquareColor {
	light, dark
};

enum class ChessVariant {
	Standard, Chess960, Atomic
};
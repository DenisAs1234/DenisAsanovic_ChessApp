#pragma once
#include<map>
#include<QString>
using namespace std;

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

inline const map<PieceType, QString> pieceStrings = {
    {PieceType::Pawn,   "Pawn"},
    {PieceType::Knight, "Knight"},
    {PieceType::Bishop, "Bishop"},
    {PieceType::Rook,   "Rook"},
    {PieceType::Queen,  "Queen"},
    {PieceType::King,   "King"}
};

inline const map<int, PieceColor> colorsByRank = {
    {1, PieceColor::White},
    {2, PieceColor::White},
    {7, PieceColor::Black},
    {8, PieceColor::Black}
};

inline const map<PieceColor, QString> colorStrings = {
    {PieceColor::White, "White"},
    {PieceColor::Black, "Black"}
};
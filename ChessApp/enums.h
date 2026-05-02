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

enum class CastlingType {
    Short, Long
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

inline const map<pair<PieceColor, PieceType>, char> fenCharacters = {
    {{PieceColor::White, PieceType::Pawn}, 'P'},
    {{PieceColor::White, PieceType::Knight}, 'N'},
    {{PieceColor::White, PieceType::Bishop}, 'B'},
    {{PieceColor::White, PieceType::Rook}, 'R'},
    {{PieceColor::White, PieceType::Queen}, 'Q'},
    {{PieceColor::White, PieceType::King}, 'K'},
    {{PieceColor::Black, PieceType::Pawn}, 'p'},
    {{PieceColor::Black, PieceType::Knight}, 'n'},
    {{PieceColor::Black, PieceType::Bishop}, 'b'},
    {{PieceColor::Black, PieceType::Rook}, 'r'},
    {{PieceColor::Black, PieceType::Queen}, 'q'},
    {{PieceColor::Black, PieceType::King}, 'k'},
};
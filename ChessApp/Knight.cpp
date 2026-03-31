#include "Knight.h"
#include "ChessBoard.h"
#include "SquareIndex.h"

Knight::Knight(PieceColor color, Square* square, QString path, ChessBoard* board) :
	Piece(PieceType::Knight, color, square, path, board) {}

void Knight::findLegalMoves() {
    legalMoves.clear();
    visibleSquares.clear();

    int rank = square->getRank();
    int file = square->getFile();

    vector<pair<int, int>> possibleMoves = {
        {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
        {1, 2}, {1, -2}, {-1, 2}, {-1, -2}
    };

    for (auto& move : possibleMoves) {
        int index = getSquareIndex(rank + move.first, file + move.second);

        if (index == -1) continue;
        Square* newSquare = board->getAllSquares()[index];
        visibleSquares.push_back(newSquare);

        if (!newSquare->isOccupied() || newSquare->getPiece()->getColor() != this->color) {
            legalMoves.push_back(board->getAllSquares()[index]);
        }
    }
}
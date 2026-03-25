#include "Knight.h"
#include "ChessBoard.h"
#include "SquareIndex.h"

Knight::Knight(PieceColor color, Square* square, QString path, ChessBoard* board) :
	Piece(PieceType::Knight, color, square, path, board) {}

void Knight::findLegalMoves() {
    int rank = square->getRank();
    int file = square->getFile();

    vector<pair<int, int>> possibleMoves = {
        {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
        {1, 2}, {1, -2}, {-1, 2}, {-1, -2}
    };

    for (auto& move : possibleMoves) {
        int newRank = rank + move.first;
        int newFile = file + move.second;

        int index = getSquareIndex(newRank, newFile);

        if (index != -1) {
            if(!board->getAllSquares()[index]->isOccupied())
                legalMoves.push_back(board->getAllSquares()[index]);
        }
    }
}
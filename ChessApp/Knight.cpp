#include "Knight.h"
#include "GameContext.h"
#include "PositionAnalyzer.h"
#include "GameState.h"
#include "Square.h"
#include "SquareIndex.h"

Knight::Knight(PieceColor color, Square* square, QString path, GameContext* context) :
	Piece(PieceType::Knight, color, square, path, context) {}

void Knight::findLegalMoves() {
    legalMoves.clear();
    findVisibleSquares();

    for(Square* newSquare : visibleSquares) {
        if (newSquare->isOccupied() && newSquare->getPiece()->getColor() == this->color) continue;
        
        if (context->getAnalyzer()->isMoveLegal(this, newSquare)) {
            legalMoves.push_back(newSquare);
        }
    }
}

void Knight::findVisibleSquares() {
    visibleSquares.clear();

    int rank = square->getRank();
    int file = square->getFile();

    vector<pair<int, int>> possibleMoves = {
        {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
        {1, 2}, {1, -2}, {-1, 2}, {-1, -2}
    };

    GameState* state = context->getState();
    for (auto& move : possibleMoves) {
        int index = getSquareIndex(rank + move.first, file + move.second);

        if (index == -1) continue;
        Square* newSquare = state->getAllSquares()[index];
        visibleSquares.push_back(newSquare);
    }
}
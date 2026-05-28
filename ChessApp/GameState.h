#pragma once

#include<vector>
#include "enums.h"
using namespace std;

class Square;
class Piece;

class GameState {
	vector<Square*> allSquares;
	int moveCount = 1;
	PieceColor turnColor = PieceColor::White;
	vector<Piece*> whitePieces;
	vector<Piece*> blackPieces;
	Square* whiteKingPos;
	Square* blackKingPos;
	QString castlingRights = "KQkq";
	QString enPassantSquare = "-";
	QString currentFen;
public:
	GameState();

	vector<Square*>& getAllSquares();
	void addSquare(int index, Square* square);

	void addPiece(Piece* piece);
	void removePiece(Piece* piece);
	vector<Piece*>& getWhitePieces();
	vector<Piece*>& getBlackPieces();

	Square* getWhiteKingPos();
	Square* getBlackKingPos();
	void setWhiteKingPos(Square* square);
	void setBlackKingPos(Square* square);

	QString getCastlingRights();
	void removeCastlingRight(char toRemove);
	char getCorrectCastlingChar(int rookFile, PieceColor rookColor);

	PieceColor getTurnColor();
	void switchTurn();

	int getMoveCount();
	void updateMoveCount();

	void addEnPassantSquare(Square* square);
	void resetEnPassantSquare();

	void generateFen();
	QString getCurrentFen();
};
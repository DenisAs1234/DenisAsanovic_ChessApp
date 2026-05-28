#include "GameState.h"
#include "Square.h"
#include "Piece.h"

GameState::GameState() : allSquares(64) {};

vector<Square*>& GameState::getAllSquares() { return allSquares; }

vector<Piece*>& GameState::getWhitePieces() { return whitePieces; }
vector<Piece*>& GameState::getBlackPieces() { return blackPieces; }

Square* GameState::getWhiteKingPos() { return whiteKingPos; }
Square* GameState::getBlackKingPos() { return blackKingPos; }

void GameState::setWhiteKingPos(Square* square) { whiteKingPos = square; }
void GameState::setBlackKingPos(Square* square) { blackKingPos = square; }

int GameState::getMoveCount() { return moveCount; }

PieceColor GameState::getTurnColor() { return turnColor; }
QString GameState::getCastlingRights() { return castlingRights; }

void GameState::addSquare(int index, Square* square) {
	allSquares[index] = square;
}

void GameState::addPiece(Piece* piece) {
	if (piece->getType() == PieceType::King) return;

	if (piece->getColor() == PieceColor::White) {
		whitePieces.push_back(piece);
		return;
	}
	blackPieces.push_back(piece);
}

void GameState::removePiece(Piece* piece) {
	if (piece->getColor() == PieceColor::White) {
		whitePieces.erase(find(whitePieces.begin(), whitePieces.end(), piece));
		return;
	}
	blackPieces.erase(find(blackPieces.begin(), blackPieces.end(), piece));
}

void GameState::removeCastlingRight(char toRemove) {
	castlingRights.remove(toRemove);
	if (castlingRights.isEmpty()) {
		castlingRights = "-";
	}
}

char GameState::getCorrectCastlingChar(int rookFile, PieceColor rookColor) {
	int kingFile = whiteKingPos->getFile();
	if (rookFile < kingFile) {
		return rookColor == PieceColor::White ? 'Q' : 'q';
	}
	return rookColor == PieceColor::White ? 'K' : 'k';
}

void GameState::addEnPassantSquare(Square* square) {
	if (enPassantSquare != "-") return;

	File file = static_cast<File>(square->getFile());
	enPassantSquare = QChar('a' + static_cast<int>(file)) + QString::number(square->getRank());
}

void GameState::resetEnPassantSquare() {
	enPassantSquare = "-";
}

void GameState::switchTurn() {
	turnColor = (turnColor == PieceColor::White) ? PieceColor::Black : PieceColor::White;
}

void GameState::updateMoveCount() {
	if (turnColor == PieceColor::White) {
		moveCount += 1;
	}
}

void GameState::generateFen() {
	QString fen = "";
	int file = 0;
	int consecutiveEmpty = 0;

	for (auto square : allSquares) {
		if (file == 8) {
			if (consecutiveEmpty > 0) {
				fen += QString::number(consecutiveEmpty);
			}
			fen += '/';
			file = 0;
			consecutiveEmpty = 0;
		}

		if (!square->isOccupied()) {
			consecutiveEmpty++;
			file++;
			continue;
		}

		if (consecutiveEmpty > 0) {
			fen += QString::number(consecutiveEmpty);
			consecutiveEmpty = 0;
		}

		Piece* piece = square->getPiece();
		pair<PieceColor, PieceType> pieceKind = { piece->getColor(), piece->getType() };

		fen += fenCharacters.at(pieceKind);
		file++;
	}

	if (consecutiveEmpty > 0) {
		fen += QString::number(consecutiveEmpty);
	}

	fen += (turnColor == PieceColor::White) ? " w " : " b ";
	fen += castlingRights + ' ' + enPassantSquare;

	qDebug() << fen;
	currentFen = fen;
}

QString GameState::getCurrentFen() {
	return currentFen;
}
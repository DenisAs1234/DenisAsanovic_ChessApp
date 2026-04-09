#include "Pawn.h"
#include "PieceFactory.h"
#include "ChessBoard.h"
#include "SquareIndex.h"

Pawn::Pawn(PieceColor color, Square* square, QString path, ChessBoard* board) :
	Piece(PieceType::Pawn, color, square, path, board) {}

void Pawn::findLegalMoves() {
	legalMoves.clear();

	int rank = square->getRank();
	int file = square->getFile();
	int forwardBy = 1;
	int index;

	while (forwardBy <= 2) {
		index = (color == PieceColor::White)
			? getSquareIndex(rank + forwardBy, file)
			: getSquareIndex(rank - forwardBy, file);

		Square* newSquare = board->getAllSquares()[index];

		if (newSquare->isOccupied()) break;

		if (isMoveLegal(newSquare)) {
			legalMoves.push_back(newSquare);
		}

		if (!hasMoved) {
			forwardBy += 1;
		}
		else break;
	}

	getCaptures(rank, file);

	for (auto& enPassantMove : enPassantMoves) {
		if (isEnPassantLegal(enPassantMove)) {
			legalMoves.push_back(enPassantMove);
		}
	}
}

void Pawn::getCaptures(int rank, int file) {
	findVisibleSquares();

	for (Square* newCapture : visibleSquares) {
		if (!newCapture->isOccupied()) continue;
		if (newCapture->getPiece()->getColor() == this->color) continue;

		if (isMoveLegal(newCapture)) {
			legalMoves.push_back(newCapture);
		}
	}
}

void Pawn::findVisibleSquares() {
	visibleSquares.clear();

	int rank = square->getRank();
	int file = square->getFile();
	vector<pair<int, int>> diagonalCaptures = { {1,1},{1,-1} };
	int index;

	for (auto& capture : diagonalCaptures) {
		index = (color == PieceColor::White)
			? getSquareIndex(rank + capture.first, file + capture.second)
			: getSquareIndex(rank - capture.first, file + capture.second);

		if (index == -1) continue;
		Square* newCapture = board->getAllSquares()[index];
		visibleSquares.push_back(newCapture);
	}
}

void Pawn::onMove() {
	if (hasMoved) return;
	hasMoved = true;

	int rank = square->getRank();

	if (rank == 4 || rank == 5) {
		checkForPawnsNextTo(this);
	}
}

void Pawn::checkForPawnsNextTo(Pawn* passingPawn) {
	int rank = passingPawn->getSquare()->getRank();
	int file = passingPawn->getSquare()->getFile();

	for (int i = -1; i <= 1; i += 2) {
		int index = getSquareIndex(rank, file + i);
		if (index == -1) continue;

		Square* nextToPassingPawn = board->getAllSquares()[index];

		if (!nextToPassingPawn->isOccupied()) continue;

		Piece* piece = nextToPassingPawn->getPiece();
		Pawn* pawn = dynamic_cast<Pawn*>(piece);

		if (pawn && pawn->getColor() != passingPawn->getColor()) {
			index = (passingPawn->getColor() == PieceColor::White)
				? getSquareIndex(rank - 1, file)
				: getSquareIndex(rank + 1, file);

			Square* behindPassingPawn = board->getAllSquares()[index];
			pawn->getEnPassantMoves().push_back(behindPassingPawn);
		}
	}
}

vector<Square*>& Pawn::getEnPassantMoves() {
	return enPassantMoves;
}

void Pawn::checkIfEnPassant(Square* destination) {
	bool isEnPassantMove = find(enPassantMoves.begin(), enPassantMoves.end(), destination)
		!= enPassantMoves.end();
	if (isEnPassantMove) {
		executeEnPassant(destination);
	}
}

void Pawn::executeEnPassant(Square* destination) {
	int index = (color == PieceColor::White)
		? getSquareIndex(destination->getRank() - 1, destination->getFile())
		: getSquareIndex(destination->getRank() + 1, destination->getFile());

	Square* enPassantPos = board->getAllSquares()[index];
	board->getScene()->removeItem(enPassantPos->getPiece());
	enPassantPos->setPiece(nullptr);
	enPassantMoves.clear();
}

bool Pawn::isEnPassantLegal(Square* destination) {
	int index = (color == PieceColor::White)
		? getSquareIndex(destination->getRank() - 1, destination->getFile())
		: getSquareIndex(destination->getRank() + 1, destination->getFile());

	Square* enPassantPos = board->getAllSquares()[index]; //polje na kojem se nalazi pješak u prolazu
	Piece* onEnPassantPos = enPassantPos->getPiece(); //pješak u prolazu
	enPassantPos->setPiece(nullptr); //micanje pješaka u prolazu s ploèe

	Square* originalSquare = square; //spremanje originalnog polja ovog pješaka (this) koji uzima
	square->setPiece(nullptr); //micanje ovog pješaka s originalnog polja
	square = destination; //polje ovog pješaka postaje njegova destinacija (iza pješaka u prolazu)
	destination->setPiece(this); //postavljanje ovog pješaka na destinaciju

	bool isKingInCheck = board->isKingInCheck(color);

	square = originalSquare; //polje ovog pješaka postaje originalno polje
	enPassantPos->setPiece(onEnPassantPos); //vraæanje pješaka u prolazu na njegovo originalno polje
	destination->setPiece(nullptr); //micanje ovog pješaka s destinacije
	square->setPiece(this); //postavljanje ovog pješaka na originalno polje

	return isKingInCheck ? false : true;
}

bool Pawn::checkIfPromotion(Square* destination) {
	int rank = destination->getRank();
	if (rank == 1 || rank == 8) {
		return true;
	}
	return false;
}

Piece* Pawn::getPromotionPiece(Square* destination) {
	board->getScene()->removeItem(this);

	QString path = ":/assets/" + colorStrings.at(color) + "Queen.png";
	Piece* piece = createPiece(PieceType::Queen, color, destination, path, board);

	board->drawPiece(piece);
	return piece;
}
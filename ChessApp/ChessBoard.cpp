#include "ChessBoard.h"
#include "pieceTypes.h"
#include "PieceFactory.h"
#include "enums.h"
#include<QGraphicsRectItem>
#include<vector>

ChessBoard::ChessBoard(QGraphicsScene* scene)
	: scene(scene), allSquares(64) {}

QGraphicsScene* ChessBoard::getScene() { return scene; }

vector<Square*> ChessBoard::getAllSquares() { return allSquares; }

Square* ChessBoard::getSelectedSquare() { return selectedSquare; }

PieceColor ChessBoard::getTurnColor() { return turnColor; }

void ChessBoard::drawBoard() {
	SquareColor color = SquareColor::dark;
	int xPos = 0;
	int yPos = 630;

	for (int rank = 1; rank <= 8; rank++) {
		for (int file = 0; file < 8; file++) {
			Square* square = new Square(static_cast<File>(file), rank, color, xPos, yPos, this);
			scene->addItem(square);
			allSquares[(rank - 1) * 8 + file] = square;

			xPos += 90;
			if (file < 7) {
				color = (color == SquareColor::dark) ? SquareColor::light : SquareColor::dark;
			}
		}
		xPos = 0;
		yPos -= 90;
	}
}

void ChessBoard::drawPiece(Piece* piece) {
	QPixmap pix(piece->getPath());
	Square* square = piece->getSquare();

	piece->setPixmap(pix);
	piece->setZValue(1);
	piece->setScale(80.0 / pix.width());
	piece->setPos(square->getX() + 5, square->getY() + 7);

	scene->addItem(piece);
}

void ChessBoard::setStartingPosition() {
	vector<PieceType> startingSetup = { PieceType::Rook, PieceType::Knight, PieceType::Bishop,
		PieceType::Queen, PieceType::King, PieceType::Bishop, PieceType::Knight, PieceType::Rook };
	int file;

	for (int rank = 1; rank <= 8; rank++) {
		file = 0;
		if (rank == 1 || rank == 8) {
			for (PieceType type : startingSetup) {
				PieceColor color = colorsByRank.at(rank);
				QString path = ":/assets/" + colorStrings.at(color) + pieceStrings.at(type) + ".png";
				Square* square = allSquares[(rank - 1) * 8 + file];
				Piece* piece = createPiece(type, color, square, path, this);
				drawPiece(piece);
				square->setPiece(piece);
				file++;
			}
		}
		if (rank == 2 || rank == 7) {
			PieceColor color = colorsByRank.at(rank);
			QString path = ":/assets/" + colorStrings.at(color) + "Pawn.png";

			for (file = 0; file < 8; file++) {
				Square* square = allSquares[(rank - 1) * 8 + file];
				Piece* piece = new Pawn(color, square, path, this);
				drawPiece(piece);
				square->setPiece(piece);
			}
		}
	}

}

void ChessBoard::selectSquare(Square* square) {
	if (selectedSquare) {
		vector<Square*> legalMoves = selectedSquare->getPiece()->getLegalMoves();
		resetSelectedSquare();
		resetColorOfLegalMoves(legalMoves);
	}

	if (square->isOccupied()) {
		selectedSquare = square;

		Piece* piece = square->getPiece();
		piece->resetLegalMoves();
	    piece->findLegalMoves();
		vector<Square*> legalMoves = piece->getLegalMoves();

		selectedSquare->highlightSelected();
		for (Square* legalMove : legalMoves) {
			legalMove->highlightMove();
		}
	}
}
/*
Square* ChessBoard::getSquareAt(QPointF pos) {
	for (Square* square : allSquares) {
		if (square->contains(square->mapFromScene(pos))) {
			return square;
		}
	}
	return nullptr;
}*/

void ChessBoard::resetSelectedSquare() {
	selectedSquare->resetColor();
	selectedSquare = nullptr;
}

void ChessBoard::resetColorOfLegalMoves(vector<Square*> legalMoves) {
	for (Square* legalMove : legalMoves) {
		legalMove->resetColor();
	}
}

void ChessBoard::clearEnPassants() {
	int startOf4thRank = getSquareIndex(4, 0);
	int endOf5thRank = getSquareIndex(5, 7);

	for (int i = startOf4thRank; i <= endOf5thRank; i++) {
		Pawn* pawn = dynamic_cast<Pawn*>(allSquares[i]->getPiece());
		if (pawn) {
			pawn->getEnPassantMoves().clear();
		}
	}
}

void ChessBoard::switchTurn() {
	turnColor = (turnColor == PieceColor::White) ? PieceColor::Black : PieceColor::White;
}
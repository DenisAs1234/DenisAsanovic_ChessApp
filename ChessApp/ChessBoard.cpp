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
	if (selectedSquare != nullptr) {
		vector<Square*> legalMoves = selectedSquare->getPiece()->getLegalMoves();
		resetSelectedSquare();
		resetColorOfLegalMoves(legalMoves);
	}
	if (square->getPiece() != nullptr) {
		selectedSquare = square;
		square->getPiece()->resetLegalMoves();
	    square->getPiece()->findLegalMoves();
		vector<Square*> legalMoves = square->getPiece()->getLegalMoves();
		selectedSquare->highlightSelected();
		for (Square* legalMove : legalMoves) {
			legalMove->highlightMove();
		}
	}
}

void ChessBoard::resetSelectedSquare() {
	selectedSquare->resetColor();
	selectedSquare = nullptr;
}

void ChessBoard::resetColorOfLegalMoves(vector<Square*> legalMoves) {
	for (Square* legalMove : legalMoves) {
		legalMove->resetColor();
	}
}
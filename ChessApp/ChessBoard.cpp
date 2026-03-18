#include "ChessBoard.h"
#include "Square.h"
#include "Piece.h"
#include "enums.h"
#include<QGraphicsRectItem>
#include<vector>
using namespace std;

ChessBoard::ChessBoard(QGraphicsScene* scene)
	: scene(scene), allSquares(64) {}

QGraphicsScene* ChessBoard::getScene() { return scene; }

void ChessBoard::drawBoard() {
	SquareColor color = SquareColor::dark;
	int xPos = 0;
	int yPos = 630;

	for (int rank = 1; rank <= 8; rank++) {
		for (int file = 0; file < 8; file++) {
			Square* square = new Square(static_cast<File>(file), rank, color, xPos, yPos);
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

void ChessBoard::drawPiece(QString path, qreal x, qreal y) {
	QPixmap pix(path);
	QGraphicsPixmapItem* piecePicture = new QGraphicsPixmapItem(pix);
	piecePicture->setPos(x, y);
	piecePicture->setZValue(1);
	piecePicture->setScale(90.0 / pix.width());
	scene->addItem(piecePicture);
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
				Piece* piece = new Piece(type, color, allSquares[(rank - 1) * 8 + file]);
				Square* square = piece->getSquare();
				drawPiece(":/assets/" + colorStrings.at(color) + pieceStrings.at(type) + ".png",
					square->getX(), square->getY());
				file++;
			}
		}
		if (rank == 2 || rank == 7) {
			for (file = 0; file < 8; file++) {
				PieceColor color = colorsByRank.at(rank);
				Piece* piece = new Piece(PieceType::Pawn, color, allSquares[(rank - 1) * 8 + file]);
				Square* square = piece->getSquare();
				drawPiece(":/assets/" + colorStrings.at(color) + "Pawn.png",
					square->getX(), square->getY());
			}
		}
	}
}
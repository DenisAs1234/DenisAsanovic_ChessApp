#include "ChessBoard.h"
#include "Square.h"
#include<QGraphicsRectItem>

ChessBoard::ChessBoard(QGraphicsScene* scene) : scene(scene) {}

QGraphicsScene* ChessBoard::getScene() { return scene; }

void ChessBoard::drawChessBoard() {
	SquareColor color = SquareColor::dark;
	int xPos = 0;
	int yPos = 700;
	for (int rank = 1; rank <= 8; rank++) {
		for (int file = 0; file < 8; file++) {
			Square* square = new Square(static_cast<File>(file), rank, color, xPos, yPos);
			scene->addItem(square);
			xPos += 100;
			if (file < 7) {
				color = (color == SquareColor::dark) ? SquareColor::light : SquareColor::dark;
			}
		}
		xPos = 0;
		yPos -= 100;
	}
}
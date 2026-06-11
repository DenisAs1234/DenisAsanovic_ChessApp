#include "Chess960Generator.h"
#include <QRandomGenerator>
#include <QDebug>

QString Chess960Generator::generateStartingPosition() {
	QVector<int> darkSquares = { 1,3,5,7 };
	generateOnePiece('B', darkSquares);

	QVector<int> lightSquares = { 0,2,4,6 };
	generateOnePiece('B', lightSquares);

	generateOnePiece('Q', freePositions);
	generateOnePiece('N', freePositions);
	generateOnePiece('N', freePositions);

	orderOfPieces[freePositions.at(0)] = 'R';
	orderOfPieces[freePositions.at(1)] = 'K';
	orderOfPieces[freePositions.at(2)] = 'R';

	qDebug() << "CHESS960 POSITION " << orderOfPieces;
	return orderOfPieces;
}

void Chess960Generator::generateOnePiece(QChar pieceChar, QVector<int> positions) {
	int index = QRandomGenerator::global()->bounded(positions.size());
	int file = positions[index];

	orderOfPieces[file] = pieceChar;
	freePositions.removeOne(file);
}
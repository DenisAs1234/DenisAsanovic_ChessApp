#pragma once

#include <QString>
#include <QVector>

class Chess960Generator {
	QString orderOfPieces = QString(8, '.');
	QVector<int> freePositions = { 0,1,2,3,4,5,6,7 };
public:
	QString generateStartingPosition();
	void generateOnePiece(QChar pieceChar, QVector<int> positions);
};
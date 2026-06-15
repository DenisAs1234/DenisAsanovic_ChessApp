#pragma once

#include <QTcpSocket>
#include <QString>

struct GameSession {
    QTcpSocket* white;
    QTcpSocket* black;

    QString variant;
    QString timeControl;
    QString startingPosition;
};
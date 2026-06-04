#pragma once

#include <QString>
#include <QTcpSocket>

struct PlayerRequest
{
    QString nickname;
    QString variant;
    QString timeControl;
    QString skill;

    QTcpSocket* socket;
};

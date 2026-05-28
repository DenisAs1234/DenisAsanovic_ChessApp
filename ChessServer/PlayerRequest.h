#pragma once

#include <QString>
#include <QTcpSocket>

struct PlayerRequest
{
    QString nickname;
    QString variant;
    QString timeControl;
    int skill;

    QTcpSocket* socket;
};

#pragma once

#include <QObject>
#include <QTcpSocket>
#include "Enums.h"

class NetworkManager : public QObject
{
    Q_OBJECT

public:
    NetworkManager(QObject* parent = nullptr);

    void connectToServer(QString ip);

    void sendCreateGame(QString nickname, QString variant, QString timeControl, QString skill);
    void sendJoinGame(QString nickname, QString variant, QString timeControl, QString skill);

    void sendMove(int from, int to, int rookFrom, int rookTo, int promotionPiece);

    void sendDrawOffer(PieceColor offerer);
    void sendDrawAccepted();

    void sendResignation(PieceColor loser);

    void sendGameOver();

signals:
    void connected();
    void lobbyCleared();
    void lobbyEntryReceived(QString nickname, QString variant, QString timeControl, QString skill);

    void matchFound(QString nickname, QString color, QString variant, 
        QString timeControl, QString startingPosition);

    void moveReceived(int from, int to, int rookFrom, int rookTo, int promotionPiece);

    void drawOfferReceived(PieceColor offerer);
    void drawAcceptedReceived();

    void resignationReceived(PieceColor loser);

private:
    QTcpSocket* socket;
    void processMessage(QString msg);
};
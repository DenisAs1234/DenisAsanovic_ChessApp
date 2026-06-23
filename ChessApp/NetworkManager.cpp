#include "NetworkManager.h"

#include <QDebug>

NetworkManager::NetworkManager(QObject* parent) : QObject(parent)
{
    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::readyRead,
        this, [this]()
        {
            processMessage(socket->readAll());
        });
}

void NetworkManager::connectToServer()
{
    socket->connectToHost("127.0.0.1", 12345);

    connect(socket, &QTcpSocket::connected,
        this, []()
        {
            qDebug() << "Connected to ChessServer!";
        });
}

void NetworkManager::sendCreateGame(QString nickname, QString variant, QString timeControl, QString skill)
{
    QString request =
        "CREATE_GAME|" +
        nickname + "|" +
        variant + "|" +
        timeControl + "|" +
        skill;

    socket->write(request.toUtf8());
}

void NetworkManager::sendJoinGame(QString nickname, QString variant, QString timeControl, QString skill)
{
    QString request =
        "JOIN_GAME|" +
        nickname + "|" +
        variant + "|" +
        timeControl + "|" +
        skill;

    socket->write(request.toUtf8());
}

void NetworkManager::sendMove(int from, int to, int rookFrom, int rookTo, int promotionPiece)
{
    QString msg =
        "MOVE|" +
        QString::number(from) + "|" +
        QString::number(to) + "|" +
        QString::number(rookFrom) + "|" +
        QString::number(rookTo) + "|" +
        QString::number(promotionPiece);

    socket->write(msg.toUtf8());
}

void NetworkManager::sendDrawOffer(PieceColor offerer) {
    socket->write(
        QString("DRAW_OFFER|%1\n")
        .arg(static_cast<int>(offerer))
        .toUtf8());
}

void NetworkManager::sendDrawAccepted() {
    socket->write("DRAW_ACCEPTED\n");
}

void NetworkManager::sendResignation(PieceColor loser) {
    socket->write(
        QString("RESIGN|%1\n")
        .arg(static_cast<int>(loser))
        .toUtf8());
}

void NetworkManager::processMessage(QString msg) {
    QStringList lines = msg.split('\n');

    for (const QString& line : lines)
    {
        if (line.startsWith("LOBBY_UPDATE")) {
            emit lobbyCleared();
            continue;
        }

        if (line.startsWith("MATCH_FOUND")) {
            QStringList parts = line.split('|');
            emit matchFound(parts[1], parts[2], parts[3], parts[4]);
            continue;
        }

        if (line.startsWith("MOVE|")) {
            QStringList parts = line.split('|');

            emit moveReceived(
                parts[1].toInt(),
                parts[2].toInt(),
                parts[3].toInt(),
                parts[4].toInt(),
                parts[5].toInt());

            continue;
        }

        if (line.startsWith("DRAW_OFFER|")) {
            QStringList parts = line.split('|');
            emit drawOfferReceived(static_cast<PieceColor>(parts[1].toInt()));
            continue;
        }

        if (line.startsWith("DRAW_ACCEPTED")) {
            emit drawAcceptedReceived();
            continue;
        }

        if (line.startsWith("RESIGN|")) {
            QStringList parts = line.split('|');
            emit resignationReceived(static_cast<PieceColor>(parts[1].toInt()));
            continue;
        }

        QStringList parts = line.split('|');

        if (parts.size() == 4)
        {
            emit lobbyEntryReceived(
                parts[0],
                parts[1],
                parts[2],
                parts[3]);
        }
    }
}
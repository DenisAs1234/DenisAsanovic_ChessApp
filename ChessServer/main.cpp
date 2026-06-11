#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QVector>
#include "PlayerRequest.h"
#include <QRandomGenerator>
#include "Chess960Generator.h"

auto isCompatible = [](const PlayerRequest& a, const PlayerRequest& b) {
    return a.variant == b.variant &&
        a.timeControl == b.timeControl &&
        a.skill == b.skill;
    };

auto sendLobbyUpdate = 
[](const QVector<PlayerRequest>& waitingPlayers, 
   const QVector<QTcpSocket*>& clients)
{
    QString msg = "LOBBY_UPDATE\n";

    for (const auto& player : waitingPlayers) {
        msg +=
            player.nickname + "|" +
            player.variant + "|" +
            player.timeControl + "|" +
            player.skill + "\n";
    }

    for (QTcpSocket* client : clients) {
        client->write(msg.toUtf8());
    }
};

auto removeFromWaitingPlayers =
[](QTcpSocket* client,
   QVector<PlayerRequest>& waitingPlayers)
{
    for (int i = 0;
        i < waitingPlayers.size();
        i++)
    {
        if (waitingPlayers[i].socket == client) {
                waitingPlayers.remove(i);
                break;
        }
    }
};

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    QTcpServer server;

    bool success = server.listen(QHostAddress::Any, 12345);

    if (!success) {
        qDebug() << "Server failed to start!";
        return -1;
    }

    qDebug() << "ChessServer listening on port 12345";

    QVector<PlayerRequest> waitingPlayers;
    QVector<QTcpSocket*> connectedClients;

    QObject::connect(&server, &QTcpServer::newConnection,
        [&]()
        {
            QTcpSocket* client = server.nextPendingConnection();
            connectedClients.push_back(client);
            sendLobbyUpdate(waitingPlayers, connectedClients);
            qDebug() << "Player connected.";

            QObject::connect(client, &QTcpSocket::disconnected,
                [&]()
                {
                    removeFromWaitingPlayers(client, waitingPlayers);
                    connectedClients.removeOne(client);
                    sendLobbyUpdate(waitingPlayers, connectedClients);
                });

            QObject::connect(client, &QTcpSocket::readyRead, [client, &waitingPlayers, &connectedClients]()
            {
                QString data = client->readAll();
                qDebug() << "Received:" << data;

                if (data.startsWith("MOVE|") ||
                    data.startsWith("DRAW_OFFER|") ||
                    data.startsWith("DRAW_ACCEPTED") ||
                    data.startsWith("RESIGN|")) {
                    qDebug() << "Broadcasting:" << data;

                    for (QTcpSocket* other : connectedClients) {
                        if (other != client)
                            other->write(data.toUtf8());
                    }
                    return;
                }

                QStringList parts = data.split('|');
                if (parts.size() != 4)
                    return;

                PlayerRequest request;
                request.nickname = parts[0];
                request.variant = parts[1];
                request.timeControl = parts[2];
                request.skill = parts[3];
                request.socket = client;

                waitingPlayers.push_back(request);
                sendLobbyUpdate(waitingPlayers, connectedClients);

                for (int i = 0; i < waitingPlayers.size() - 1; i++)
                {
                    PlayerRequest& other = waitingPlayers[i];

                    if (isCompatible(request, other))
                    {
                        qDebug() << "MATCH FOUND:" << request.nickname << "vs" << other.nickname;

                        bool randomColor = QRandomGenerator::global()->bounded(2);

                        QString requestColor = randomColor ? "White" : "Black";
                        QString otherColor = randomColor ? "Black" : "White";

                        QString startingPosition = "RNBQKBNR";
                        
                        if (request.variant == "Chess960") {
                            auto chess960Generator = new Chess960Generator();
                            startingPosition = chess960Generator->generateStartingPosition();
                        }
                        
                        request.socket->write(
                            ("MATCH_FOUND|" +
                                requestColor + "|" +
                                request.variant + "|" +
                                request.timeControl + "|" + 
                                startingPosition + "\n").toUtf8());

                        other.socket->write(
                            ("MATCH_FOUND|" +
                                otherColor + "|" +
                                request.variant + "|" +
                                request.timeControl + "|" +
                                startingPosition + "\n").toUtf8());

                        waitingPlayers.remove(i);
                        waitingPlayers.pop_back();
                        sendLobbyUpdate(waitingPlayers, connectedClients);

                        break;
                    }
                }
                
                qDebug() << request.nickname << "added to queue.";
            });
        });

    return app.exec();
}
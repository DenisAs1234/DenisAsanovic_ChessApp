#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QVector>
#include "PlayerRequest.h"

auto isCompatible = [](const PlayerRequest& a, const PlayerRequest& b)
    {
        return a.variant == b.variant &&
            a.timeControl == b.timeControl &&
            abs(a.skill - b.skill) <= 200;
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

    QObject::connect(&server, &QTcpServer::newConnection,
        [&]()
        {
            QTcpSocket* client =
                server.nextPendingConnection();

            qDebug() << "Player connected.";

            QObject::connect(client,
                &QTcpSocket::readyRead,
                [client, &waitingPlayers]()
                {
                    QString data =
                        client->readAll();

                    qDebug() << "Received:" << data;

                    QStringList parts =
                        data.split('|');

                    if (parts.size() != 4)
                        return;

                    PlayerRequest request;

                    request.nickname = parts[0];
                    request.variant = parts[1];
                    request.timeControl = parts[2];
                    request.skill = parts[3].toInt();
                    request.socket = client;

                    waitingPlayers.push_back(request);

                    for (int i = 0; i < waitingPlayers.size() - 1; i++)
                    {
                        PlayerRequest& other = waitingPlayers[i];

                        if (isCompatible(request, other))
                        {
                            qDebug() << "MATCH FOUND:"
                                << request.nickname
                                << "vs"
                                << other.nickname;

                            request.socket->write("MATCH_FOUND|White\n");
                            other.socket->write("MATCH_FOUND|Black\n");

                            waitingPlayers.remove(i);
                            waitingPlayers.pop_back();

                            break;
                        }
                    }

                    qDebug()
                        << request.nickname
                        << "added to queue.";
                });
        });

    return app.exec();
}
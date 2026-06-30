#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QVector>
#include "PlayerRequest.h"
#include <QRandomGenerator>
#include "Chess960Generator.h"
#include "GameSession.h"

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

auto removeFromWaitingPlayers = [](QTcpSocket* client, QVector<PlayerRequest>& waitingPlayers)
{
    for (int i = 0; i < waitingPlayers.size(); i++)
    {
        if (waitingPlayers[i].socket == client) {
                waitingPlayers.remove(i);
                break;
        }
    }
};

auto findSession = [](QTcpSocket* socket, QVector<GameSession>& activeGames) -> GameSession*
{
    for (auto& game : activeGames) {

        if (game.white == socket)
            return &game;

        if (game.black == socket)
            return &game;
    }

    return nullptr;
};

auto isPlayerInGame = [](QTcpSocket* client, const QVector<GameSession>& activeGames)
    {
        for (const auto& game : activeGames) {
            if (game.white == client ||
                game.black == client)
                return true;
        }

        return false;
    };

auto isPlayerWaiting = [](QTcpSocket* client, const QVector<PlayerRequest>& waitingPlayers)
    {
        for (const auto& player : waitingPlayers) {
            if (player.socket == client)
                return true;
        }

        return false;
    };

void removeFinishedGame(QTcpSocket* client, QVector<GameSession>& activeGames)
{
    for (int i = 0; i < activeGames.size(); ++i)
    {
        if (activeGames[i].white == client ||
            activeGames[i].black == client)
        {
            activeGames.removeAt(i);
            return;
        }
    }
}

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
    QVector<GameSession> activeGames;

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

            QObject::connect(client, &QTcpSocket::readyRead,
                [client, &waitingPlayers, &connectedClients, &activeGames]()
                {
                    QString data = client->readAll();
                    qDebug() << "Received:" << data;

                    QStringList messages = data.split('\n', Qt::SkipEmptyParts);

                    for (const QString& message : messages)
                    {
                        if (message.startsWith("MOVE|") ||
                            message.startsWith("DRAW_OFFER|") ||
                            message.startsWith("DRAW_ACCEPTED") ||
                            message.startsWith("RESIGN|"))
                        {
                            GameSession* session = findSession(client, activeGames);

                            if (!session)
                                continue;

                            QTcpSocket* opponent = (session->white == client)
                                ? session->black
                                : session->white;

                            if (opponent)
                                opponent->write((message + "\n").toUtf8());

                            continue;
                        }

                        if (message.startsWith("GAME_OVER"))
                        {
                            removeFinishedGame(client, activeGames);
                            continue;
                        }

                        QStringList parts = message.split('|');

                        PlayerRequest request;

                        QString command = parts[0];
                        request.nickname = parts[1];
                        request.variant = parts[2];
                        request.timeControl = parts[3];
                        request.skill = parts[4];
                        request.socket = client;

                        if (command == "CREATE_GAME") {
                            if (isPlayerInGame(client, activeGames))
                                return;

                            if (isPlayerWaiting(client, waitingPlayers))
                                return;

                            waitingPlayers.push_back(request);
                            sendLobbyUpdate(waitingPlayers, connectedClients);
                        }

                        if (command == "JOIN_GAME") {
                            if (isPlayerInGame(client, activeGames))
                                return;

                            removeFromWaitingPlayers(client, waitingPlayers);
                            waitingPlayers.push_back(request);
                            sendLobbyUpdate(waitingPlayers, connectedClients);
                        }

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
                                    Chess960Generator chess960Generator;
                                    startingPosition = chess960Generator.generateStartingPosition();
                                }

                                GameSession session;

                                session.white = (requestColor == "White")
                                    ? request.socket
                                    : other.socket;

                                session.black = (requestColor == "Black")
                                    ? request.socket
                                    : other.socket;

                                session.variant = request.variant;
                                session.timeControl = request.timeControl;
                                session.startingPosition = startingPosition;

                                activeGames.push_back(session);

                                request.socket->write(
                                    ("MATCH_FOUND|" +
                                        requestColor + "|" +
                                        request.variant + "|" +
                                        request.timeControl + "|" +
                                        startingPosition + "|" +
                                        other.nickname + "\n").toUtf8());

                                other.socket->write(
                                    ("MATCH_FOUND|" +
                                        otherColor + "|" +
                                        request.variant + "|" +
                                        request.timeControl + "|" +
                                        startingPosition + "|" +
                                        request.nickname + "\n").toUtf8());

                                waitingPlayers.remove(i);
                                waitingPlayers.pop_back();
                                sendLobbyUpdate(waitingPlayers, connectedClients);

                                break;
                            }
                        }
                    }
                });
        });

    return app.exec();
}
#pragma once

#include <QMainWindow>
#include "NetworkManager.h"

class QStackedWidget;
class QWidget;
class QTcpSocket;
class QPushButton;
class QLineEdit;
class QComboBox;
class QVBoxLayout;
class GamePage;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    NetworkManager* network;

    QStackedWidget* stackedWidget;

    QWidget* menuPage;
    QWidget* lobbyPage;
    GamePage* gamePage;

    QTcpSocket* socket;

    QString nickname;
    QLineEdit* nicknameEdit;
    QComboBox* skillBox;
    QComboBox* timeBox;
    QComboBox* variantBox;

    QWidget* lobbyFrame;
    QVBoxLayout* gamesLayout;
    QPushButton* backToMenuButton;

    QPushButton* createGameButton;
    QPushButton* browseGamesButton;

    void setupMenuUI();
    void createMenuWidgets();
    void setupMenuStyles();
    void setupMenuLayout();

    void setupLobbyUI();
    void addLobbyEntry(QString nickname, QString variant, QString timeControl, QString skill);
    void clearLobby();
};
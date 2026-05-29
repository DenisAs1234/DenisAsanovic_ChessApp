#pragma once

#include <QMainWindow>

class QStackedWidget;
class QWidget;
class QTcpSocket;
class QPushButton;
class QLineEdit;
class QComboBox;
class GamePage;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    QStackedWidget* stackedWidget;

    QWidget* menuPage;
    QWidget* lobbyPage;
    GamePage* gamePage;

    QTcpSocket* socket;

    QLineEdit* nicknameEdit;
    QComboBox* skillBox;
    QComboBox* timeBox;
    QComboBox* variantBox;

    QPushButton* createGameButton;
    QPushButton* joinGameButton;

    void setupMenuUI();
    void createMenuWidgets();
    void setupMenuStyles();
    void setupMenuLayout();
};
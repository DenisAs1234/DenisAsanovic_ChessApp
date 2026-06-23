#include "GamePageRenderer.h"
#include "Piece.h"
#include "Square.h"
#include "Pawn.h"
#include "PromotionOption.h"
#include "GameContext.h"
#include "GameState.h"
#include "SquareIndex.h"
#include "GameButton.h"
#include "Player.h"

GamePageRenderer::GamePageRenderer(QGraphicsScene* scene) : scene(scene) {};

QGraphicsScene* GamePageRenderer::getScene() { return scene; }
void GamePageRenderer::setContext(GameContext* context) { this->context = context; }
vector<QGraphicsRectItem*>& GamePageRenderer::getPromotionMenu() { return promotionMenu; }
bool GamePageRenderer::getPromotionMenuActive() { return promotionMenuActive; }
void GamePageRenderer::setPromotionMenuActive(bool isActive) { promotionMenuActive = isActive; }

void GamePageRenderer::drawBoard(PieceColor localPlayerColor) {
	SquareColor color = SquareColor::dark;

	qreal xPos = 0;
	qreal yPos = 630;
	
	bool localIsWhite = localPlayerColor == PieceColor::White;
	int step = localIsWhite ? 1 : -1;
	int start = localIsWhite ? 1 : 8;
	int end = start + (step * 8);

	for (int rank = start; rank != end; rank += step) {
		for (int file = start; file != end; file += step) {
			Square* square = new Square(static_cast<File>(file - 1), rank, color, xPos, yPos, context);
			scene->addItem(square);
			context->getState()->addSquare(getSquareIndex(rank, file - 1), square);

			xPos += 90;
			if (file != end - step) {
				color = (color == SquareColor::dark) ? SquareColor::light : SquareColor::dark;
			}
		}
		xPos = 0;
		yPos -= 90;
	}	
}

void GamePageRenderer::drawPiece(Piece* piece) {
	QPixmap pix(piece->getPath());
	Square* square = piece->getSquare();

	piece->setPixmap(pix);
	piece->setZValue(1);
	piece->setScale(80.0 / pix.width());
	piece->setPos(square->getX() + 5, square->getY() + 7);

	scene->addItem(piece);
}

void GamePageRenderer::drawButtons() {
	Player localPlayer = context->getLocalPlayerColor() == PieceColor::White
		? context->getWhitePlayer()
		: context->getBlackPlayer();

	GameButton* draw =
		new GameButton(ButtonAction::OfferDraw, "Offer draw", localPlayer, context, 780, 550);

	scene->addItem(draw);

	GameButton* resign =
		new GameButton(ButtonAction::Resign, "Resign", localPlayer, context, 780, 620);

	scene->addItem(resign);

	backToLobbyButton =
		new GameButton(ButtonAction::BackToLobby, "Back to lobby", localPlayer, context, -250, 0);

	scene->addItem(backToLobbyButton);
	backToLobbyButton->setVisible(false);
	backToLobbyButton->setEnabled(false);
}

void GamePageRenderer::setPlayerNames(QString myName, QString opponentName) {
	localNickname = new QGraphicsTextItem();
	opponentNickname = new QGraphicsTextItem();

	qDebug() << "my name: " << myName;
	qDebug() << "opponent name: " << opponentName;

	localNickname->setPlainText(myName);
	opponentNickname->setPlainText(opponentName);
}

void GamePageRenderer::drawNicknames() {
	QFont nicknameFont("Arial", 20, QFont::Bold);

	localNickname->setFont(nicknameFont);
	opponentNickname->setFont(nicknameFont);

	localNickname->setDefaultTextColor(Qt::black);
	opponentNickname->setDefaultTextColor(Qt::black);

	PieceColor localColor = context->getLocalPlayerColor();

	qreal localNicknameY = 480;
	qreal opponentNicknameY = 200;

	localNickname->setPos(780, localNicknameY);
	opponentNickname->setPos(780, opponentNicknameY);

	scene->addItem(localNickname);
	scene->addItem(opponentNickname);
}

void GamePageRenderer::drawClocks() {
	QFont clockFont("Arial", 20, QFont::Bold);

	whiteClock = new QGraphicsTextItem("0:00");
	blackClock = new QGraphicsTextItem("0:00");

	whiteClock->setFont(clockFont);
	blackClock->setFont(clockFont);

	whiteClock->setDefaultTextColor(Qt::black);
	blackClock->setDefaultTextColor(Qt::black);

	PieceColor localColor = context->getLocalPlayerColor();

	qreal localClockY = 440;
	qreal opponentClockY = 240;

	if (localColor == PieceColor::White) {
		whiteClock->setPos(780, localClockY);
		blackClock->setPos(780, opponentClockY);
	}
	else {
		blackClock->setPos(780, localClockY);
		whiteClock->setPos(780, opponentClockY);
	}

	scene->addItem(whiteClock);
	scene->addItem(blackClock);
}

void GamePageRenderer::updateClockDisplay(Player& player) {
	qint64 ms = player.getRemainingTime();
	int seconds = ms / 1000;

	QString text = QString("%1:%2")
		.arg(seconds / 60, 1, 10)
		.arg(seconds % 60, 2, 10, QChar('0'));

	if (player.getRemainingTime() < 20000) {
		text += QString(".%1").arg(ms / 100 % 10);
	}

	lastDisplayedTime = text;

	if (player.getColor() == PieceColor::White) {
		whiteClock->setPlainText(text);
		return;
	}
	blackClock->setPlainText(text);
}

void GamePageRenderer::removeFromBoard(Piece* piece) {
	scene->removeItem(piece);
}

void GamePageRenderer::resetColor(Square* square) {
	QColor darkSquare(194, 106, 62);
	QColor lightSquare(247, 183, 151);
	square->setBrush(QBrush(square->getColor() == SquareColor::dark ? darkSquare : lightSquare));
}

void GamePageRenderer::highlightSelected(Square* square) {
	square->setBrush(QBrush(square->getColor() == SquareColor::dark 
		? QColor(252, 186, 3) 
		: QColor(250, 209, 5)));
}

void GamePageRenderer::highlightLegalMove(Square* square) {
	if (!square->isOccupied()) {
		square->setBrush(QBrush(square->getColor() == SquareColor::dark 
			? QColor(30, 156, 52) 
			: QColor(52, 235, 85)));
		return;
	}
	square->setBrush(QBrush(square->getColor() == SquareColor::dark 
		? QColor(39, 117, 242) 
		: QColor(66, 139, 255)));
}

void GamePageRenderer::highlightLastMove(Square* startingSquare, Square* destination) {
	startingSquare->setBrush(
		QBrush(startingSquare->getColor() == SquareColor::dark
			? QColor(252, 186, 3)
			: QColor(250, 209, 5)));

	destination->setBrush(
		QBrush(destination->getColor() == SquareColor::dark
			? QColor(252, 186, 3)
			: QColor(250, 209, 5)));

	lastMoveFrom = startingSquare;
	lastMoveTo = destination;
}

void GamePageRenderer::resetColorOfLegalMoves(vector<Square*> legalMoves) {
	for (Square* legalMove : legalMoves) {
		resetColor(legalMove);
	}
}

void GamePageRenderer::resetHighlightedMove() {
	if (lastMoveFrom)
		resetColor(lastMoveFrom);

	if (lastMoveTo)
		resetColor(lastMoveTo);
}

void GamePageRenderer::drawPromotionMenu(Pawn* promotingPawn, Square* destination) {
	setPromotionMenuActive(true);

	vector<PieceType> promotionTypes = { PieceType::Queen, PieceType::Rook,
		PieceType::Bishop, PieceType::Knight };
	int yPos = 180;
	QColor lightGreen(5, 252, 104);
	QColor darkGreen(4, 181, 75);
	PieceColor pawnColor = promotingPawn->getColor();

	for (auto type : promotionTypes) {
		auto rect = new QGraphicsRectItem(-100, yPos, 90, 90);
		rect->setBrush(type == PieceType::Queen || type == PieceType::Bishop ? lightGreen : darkGreen);
		rect->setPen(Qt::NoPen);
		scene->addItem(rect);
		promotionMenu.push_back(rect);

		auto promotionOption = new PromotionOption(type, promotingPawn, destination, context);
		QString path = ":/assets/" + colorStrings.at(pawnColor) + pieceStrings.at(type) + ".png";

		QPixmap pix(path);
		promotionOption->setPixmap(pix);
		promotionOption->setParentItem(rect);
		promotionOption->setScale(80.0 / pix.width());
		promotionOption->setPos(-95, yPos + 7);

		yPos += 90;
	}
}

void GamePageRenderer::removePromotionMenu() {
	for (auto promotionType : promotionMenu) {
		scene->removeItem(promotionType);
		delete promotionType;
	}
	scene->setSceneRect(scene->itemsBoundingRect());
	promotionMenu.clear();
}

bool GamePageRenderer::getBoardActive() { return boardActive; }

void GamePageRenderer::showGameOverWindow(QString outcome) {
	boardActive = false;

	backToLobbyButton->setVisible(true);
	backToLobbyButton->setEnabled(true);

	gameOverWindow = new QGraphicsItemGroup();
	gameOverWindow->setZValue(10);
	scene->addItem(gameOverWindow);

	bool isDraw = outcome.contains("Draw", Qt::CaseInsensitive);

	QColor backgroundColor = isDraw ? QColor(70, 120, 220) : QColor(60, 170, 90);

	int width = 420;
	int height = 220;

	int x = 150;
	int y = 250;

	// Zaobljeni pravokutnik
	QPainterPath path;
	path.addRoundedRect(x, y, width, height, 25, 25);

	QGraphicsPathItem* window = new QGraphicsPathItem(path);
	window->setBrush(backgroundColor);
	window->setPen(QPen(Qt::black, 3));
	window->setZValue(2);

	gameOverWindow->addToGroup(window);

	// GAME OVER naslov
	QGraphicsTextItem* title = new QGraphicsTextItem("GAME OVER");

	QFont titleFont;
	titleFont.setPointSize(24);
	titleFont.setBold(true);

	title->setFont(titleFont);
	title->setDefaultTextColor(Qt::white);
	title->setZValue(3);

	QRectF titleRect = title->boundingRect();

	title->setPos(x + (width - titleRect.width()) / 2, y + 40);

	gameOverWindow->addToGroup(title);

	// Ishod partije
	QGraphicsTextItem* result = new QGraphicsTextItem(outcome);

	QFont resultFont;
	resultFont.setPointSize(20);
	resultFont.setBold(true);

	result->setFont(resultFont);
	result->setDefaultTextColor(Qt::white);
	result->setZValue(3);

	QRectF resultRect = result->boundingRect();
	result->setPos(x + (width - resultRect.width()) / 2, y + 120);

	gameOverWindow->addToGroup(result);

	// Botun za zatvaranje prozora
	closeButton = new GameButton(
		ButtonAction::CloseGameOverWindow, "X", context->getWhitePlayer(), context, x + width - 50, y + 10);
	closeButton->setZValue(11);

	scene->addItem(closeButton);
}

void GamePageRenderer::removeGameOverWindow() {
	if (!gameOverWindow)
		return;

	scene->removeItem(gameOverWindow);
	delete gameOverWindow;
	gameOverWindow = nullptr;

	scene->removeItem(closeButton);
	delete closeButton;
	closeButton = nullptr;
}

void GamePageRenderer::showDrawOfferMessage(PieceColor offerer) {
	int moveCount = context->getState()->getMoveCount();
	QString message = QString("%1 offered a draw.")
		.arg(colorStrings.at(offerer));

	drawOfferMsg = new QGraphicsTextItem(message);
	QFont drawOfferFont;

	drawOfferMsg->setDefaultTextColor(Qt::black);
	drawOfferFont.setPointSize(14);
	drawOfferMsg->setFont(drawOfferFont);
	drawOfferMsg->setPos(780, 335);
	
	scene->addItem(drawOfferMsg);
}

void GamePageRenderer::removeDrawOfferMessage() {
	if (drawOfferMsg) {
		scene->removeItem(drawOfferMsg);
		delete drawOfferMsg;
		drawOfferMsg = nullptr;
	}
}
#include "BoardRenderer.h"
#include "Piece.h"
#include "Square.h"
#include "Pawn.h"
#include "PromotionOption.h"
#include "GameContext.h"
#include "GameState.h"
#include "SquareIndex.h"
#include "GameButton.h"
#include "Player.h"

BoardRenderer::BoardRenderer(QGraphicsScene* scene) : scene(scene) {};

QGraphicsScene* BoardRenderer::getScene() { return scene; }
void BoardRenderer::setContext(GameContext* context) { this->context = context; }
vector<QGraphicsRectItem*>& BoardRenderer::getPromotionMenu() { return promotionMenu; }
bool BoardRenderer::getPromotionMenuActive() { return promotionMenuActive; }
void BoardRenderer::setPromotionMenuActive(bool isActive) { promotionMenuActive = isActive; }

void BoardRenderer::drawBoard() {
	SquareColor color = SquareColor::dark;
	qreal xPos = 0;
	qreal yPos = 630;

	for (int rank = 1; rank <= 8; rank++) {
		for (int file = 0; file < 8; file++) {
			Square* square = new Square(static_cast<File>(file), rank, color, xPos, yPos, context);
			scene->addItem(square);
			context->getState()->addSquare(getSquareIndex(rank, file), square);

			xPos += 90;
			if (file < 7) {
				color = (color == SquareColor::dark) ? SquareColor::light : SquareColor::dark;
			}
		}
		xPos = 0;
		yPos -= 90;
	}
}

void BoardRenderer::drawPiece(Piece* piece) {
	QPixmap pix(piece->getPath());
	Square* square = piece->getSquare();

	piece->setPixmap(pix);
	piece->setZValue(1);
	piece->setScale(80.0 / pix.width());
	piece->setPos(square->getX() + 5, square->getY() + 7);

	scene->addItem(piece);
}

void BoardRenderer::removeFromBoard(Piece* piece) {
	scene->removeItem(piece);
}

void BoardRenderer::resetColor(Square* square) {
	QColor darkSquare(194, 106, 62);
	QColor lightSquare(247, 183, 151);
	square->setBrush(QBrush(square->getColor() == SquareColor::dark ? darkSquare : lightSquare));
}

void BoardRenderer::highlightSelected(Square* square) {
	square->setBrush(QBrush(square->getColor() == SquareColor::dark 
		? QColor(252, 186, 3) 
		: QColor(250, 209, 5)));
}

void BoardRenderer::highlightMove(Square* square) {
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

void BoardRenderer::resetColorOfLegalMoves(vector<Square*> legalMoves) {
	for (Square* legalMove : legalMoves) {
		resetColor(legalMove);
	}
}

void BoardRenderer::drawPromotionMenu(Pawn* promotingPawn, Square* destination) {
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

void BoardRenderer::removePromotionMenu() {
	for (auto promotionType : promotionMenu) {
		scene->removeItem(promotionType);
		delete promotionType;
	}
	scene->setSceneRect(scene->itemsBoundingRect());
	promotionMenu.clear();
}

bool BoardRenderer::getBoardActive() { return boardActive; }

void BoardRenderer::showGameOverWindow(QString outcome) {
	boardActive = false;

	bool isDraw = outcome.contains("Draw", Qt::CaseInsensitive);

	QColor backgroundColor = isDraw ? QColor(70, 120, 220) : QColor(60, 170, 90);

	int width = 420;
	int height = 220;

	int x = (scene->width() - width) / 2;
	int y = (scene->height() - height) / 2;

	// Zaobljeni pravokutnik
	QPainterPath path;
	path.addRoundedRect(x, y, width, height, 25, 25);

	QGraphicsPathItem* window = new QGraphicsPathItem(path);
	window->setBrush(backgroundColor);
	window->setPen(QPen(Qt::black, 3));
	window->setZValue(2);

	scene->addItem(window);

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

	scene->addItem(title);

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

	scene->addItem(result);
}

void BoardRenderer::drawButtons() {
	GameButton* whiteDraw =
		new GameButton(ButtonAction::OfferDraw,
			"Offer draw",
			context->getWhitePlayer(),
			context,
			780, 500);

	scene->addItem(whiteDraw);

	GameButton* whiteResign =
		new GameButton(ButtonAction::Resign,
			"White Resign",
			context->getWhitePlayer(),
			context,
			780, 570);

	scene->addItem(whiteResign);

	GameButton* blackDraw =
		new GameButton(ButtonAction::OfferDraw,
			"Offer draw",
			context->getBlackPlayer(),
			context,
			780, 100);

	scene->addItem(blackDraw);

	GameButton* blackResign =
		new GameButton(ButtonAction::Resign,
			"Black Resign",
			context->getBlackPlayer(),
			context,
			780, 170);

	scene->addItem(blackResign);
}

void BoardRenderer::showDrawOfferMessage(PieceColor offerer) {
	int moveCount = context->getState()->getMoveCount();
	QString message = QString("Move %1: %2 offered a draw.")
		.arg(QString::number(moveCount))
		.arg(colorStrings.at(offerer));

	drawOfferMsg = new QGraphicsTextItem(message);
	QFont drawOfferFont;

	drawOfferMsg->setDefaultTextColor(Qt::black);
	drawOfferFont.setPointSize(14);
	drawOfferMsg->setFont(drawOfferFont);
	drawOfferMsg->setPos(780, 335);
	
	scene->addItem(drawOfferMsg);
}

void BoardRenderer::removeDrawOfferMessage() {
	if (drawOfferMsg) {
		scene->removeItem(drawOfferMsg);
		delete drawOfferMsg;
		drawOfferMsg = nullptr;
	}
}

void BoardRenderer::drawClocks() {
	QFont clockFont("Arial", 20, QFont::Bold);

	whiteClock = new QGraphicsTextItem("0:25");
	blackClock = new QGraphicsTextItem("0:25");

	whiteClock->setFont(clockFont);
	blackClock->setFont(clockFont);

	whiteClock->setDefaultTextColor(Qt::black);
	blackClock->setDefaultTextColor(Qt::black);

	whiteClock->setPos(780, 440);
	blackClock->setPos(780, 240);

	scene->addItem(whiteClock);
	scene->addItem(blackClock);
}

void BoardRenderer::updateClockDisplay(Player& player) {
	qint64 ms = player.getRemainingTime();
	int seconds = ms / 1000;

	QString text = QString("%1:%2")
		.arg(seconds / 60, 1, 10)
		.arg(seconds % 60, 2, 10, QChar('0'));
	
	if (player.getRemainingTime() < 20000) {
		text += QString(".%1").arg(ms / 100 % 10);
	}

	if (text == lastDisplayedTime) return;

	lastDisplayedTime = text;

	if (player.getColor() == PieceColor::White) {
		whiteClock->setPlainText(text);
		return;
	}
	blackClock->setPlainText(text);
}
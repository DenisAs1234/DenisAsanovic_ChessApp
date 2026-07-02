#include "GameContext.h"
#include "GameState.h"
#include "GamePageRenderer.h"
#include "PositionAnalyzer.h"
#include "SpecialMoveHandler.h"
#include "GameEndChecker.h"
#include "Piece.h"
#include "Square.h"
#include "PieceFactory.h"
#include <QBrush>

GameContext::GameContext(GameState* state, GamePageRenderer* gameRenderer, PositionAnalyzer* analyzer,
	SpecialMoveHandler* specialMoves, GameEndChecker* gameEndings) :
	whitePlayer(PieceColor::White, "White"), blackPlayer(PieceColor::Black, "Black"),
	state(state), gameRenderer(gameRenderer), analyzer(analyzer), specialMoves(specialMoves), 
	gameEndings(gameEndings) {

	clockTimer = new QTimer(this);
	connect(clockTimer, &QTimer::timeout, this, &GameContext::updateClock);
	elapsedTimer.start();
};

Player GameContext::getWhitePlayer() { return whitePlayer; }
Player GameContext::getBlackPlayer() { return blackPlayer; }

PieceColor GameContext::getLocalPlayerColor() { return localPlayerColor; }
void GameContext::setLocalPlayerColor(PieceColor color) { localPlayerColor = color; }

ChessVariant GameContext::getVariant() { return variant; }
void GameContext::setVariant(ChessVariant variant) { this->variant = variant; }

void GameContext::setTimeControl(TimeControl time) { 
	this->timeControl = time; 

	whitePlayer.setRemainingTime(time.initial);
	blackPlayer.setRemainingTime(time.initial);

	gameRenderer->updateClockDisplay(whitePlayer);
	gameRenderer->updateClockDisplay(blackPlayer);
}

Player& GameContext::getTurnPlayer() {
	return (state->getTurnColor() == PieceColor::White)
		? whitePlayer
		: blackPlayer;
}

Square* GameContext::getSelectedSquare() { return selectedSquare; }

GameState* GameContext::getState() { return state; }
GamePageRenderer* GameContext::getGameRenderer() { return gameRenderer; }

PositionAnalyzer* GameContext::getAnalyzer() { return analyzer; }
SpecialMoveHandler* GameContext::getSpecialMoves() { return specialMoves; }
GameEndChecker* GameContext::getGameEndings() { return gameEndings; }

void GameContext::setFactory(PieceFactory* factory) { this->factory = factory; }

void GameContext::setupStartingPosition(QString orderOfPieces) {
	int kingFile = orderOfPieces.indexOf('K');
	specialMoves->setInitialKingFile(orderOfPieces.indexOf('K'));
	qDebug() << "initial king file " << orderOfPieces.indexOf('K');

	specialMoves->setInitialQueensideRookFile(orderOfPieces.indexOf('R'));
	qDebug() << "initial queenside rook file " << specialMoves->getInitialQueensideRookFile();

	specialMoves->setInitialKingsideRookFile(orderOfPieces.indexOf('R', kingFile + 1));
	qDebug() << "initial kingside rook file " << 
		specialMoves->getInitialKingsideRookFile();

	int file;

	auto allSquares = state->getAllSquares();

	for (int rank = 1; rank <= 8; rank++) {
		file = 0;
		if (rank == 1 || rank == 8) {
			for (QChar pieceChar : orderOfPieces) {
				PieceColor color = colorsByRank.at(rank);
				PieceType type = pieceTypes.at(pieceChar);
				QString pieceName = pieceStrings.at(type);

				QString path = ":/assets/" + colorStrings.at(color) + pieceName + ".png";

				Square* square = allSquares[(rank - 1) * 8 + file];
				Piece* piece = factory->createPiece(type, color, square, path);

				gameRenderer->drawPiece(piece);
				square->setPiece(piece);

				if (type == PieceType::King) {
					if (rank == 1) 
						state->setWhiteKingPos(square);
					else 
						state->setBlackKingPos(square);
					specialMoves->setInitialKingFile(file);
				}
				file++;
			}
		}
		if (rank == 2 || rank == 7) {
			PieceColor color = colorsByRank.at(rank);
			QString path = ":/assets/" + colorStrings.at(color) + "Pawn.png";

			for (file = 0; file < 8; file++) {
				Square* square = allSquares[(rank - 1) * 8 + file];
				Piece* piece = new Pawn(color, square, 1, path, this);
				gameRenderer->drawPiece(piece);
				square->setPiece(piece);
				state->addPiece(piece);
			}
		}
	}
	state->updateCurrentFen();
	gameEndings->updatePositionCounts();
	state->getAllSquares();
}

void GameContext::selectSquare(Square* square) {
	if (selectedSquare) {
		auto legalMoves = selectedSquare->getPiece()->getLegalMoves();
		resetSelectedSquare();
		gameRenderer->resetColorOfLegalMoves(legalMoves);
	}

	if (square->isOccupied()) {
		selectedSquare = square;

		Piece* piece = square->getPiece();
		piece->findLegalMoves();
		auto legalMoves = piece->getLegalMoves();

		gameRenderer->highlightSelected(selectedSquare);
		for (Square* legalMove : legalMoves) {
			gameRenderer->highlightLegalMove(legalMove);
		}
	}
}

void GameContext::resetSelectedSquare() {
	gameRenderer->resetColor(selectedSquare);
	selectedSquare = nullptr;
}

bool GameContext::updateGameStateAfterMove() {
	addIncrement();
	state->switchTurn();
	state->updateCurrentFen();
	state->updateMoveCount();
	gameEndings->updatePositionCounts();
	return gameEndings->ifGameIsOver();
}

void GameContext::handleSquareClick(Square* clickedSquare) {
	if (!gameRenderer->getBoardActive()) return;
	if (state->getTurnColor() != localPlayerColor) return;
	if (gameRenderer->getPromotionMenuActive()) return;

	if (!selectedSquare) {
		if (clickedSquare->isOccupied() && clickedSquare->getPiece()->getColor() != state->getTurnColor()) 
			return;
		selectSquare(clickedSquare);
		return;
	}

	Piece* selectedPiece = selectedSquare->getPiece();

	auto legalMoves = selectedPiece->getLegalMoves();
	if (find(legalMoves.begin(), legalMoves.end(), clickedSquare) != legalMoves.end()) {
		Square* originalSquare = selectedPiece->getSquare();
		int fromIndex = originalSquare->getIndex();

		selectedPiece->moveTo(clickedSquare);

		auto destination = selectedPiece->getDestination();
		int toIndex = destination ? destination->getIndex() : clickedSquare->getIndex();

		if (specialMoves->isPromotionPending()) {
			specialMoves->setPendingPromotionFrom(fromIndex);
			specialMoves->setPendingPromotionTo(toIndex);

			resetSelectedSquare();
			gameRenderer->resetColorOfLegalMoves(legalMoves);
			return;
		}

		bool isGameOver = updateGameStateAfterMove();

		clearDrawOffer();

		resetSelectedSquare();
		gameRenderer->resetColorOfLegalMoves(legalMoves);

		gameRenderer->resetHighlightedMove();
		gameRenderer->highlightLastMove(originalSquare, state->getAllSquares()[toIndex]);

		if (!applyingNetworkMove) {
			emit movePlayed(
				fromIndex,
				toIndex,
				specialMoves->getCastlingRookFrom(),
				specialMoves->getCastlingRookTo(),
				specialMoves->getPromotionPiece());

			specialMoves->clearSpecialMoveData();

			if (isGameOver)
				emit gameFinished();
		}

		selectedPiece->resetDestination();

		drawOfferActive = false;
		gameRenderer->removeDrawOfferMessage();

		return;
	}
	
	resetSelectedSquare();
	gameRenderer->resetColorOfLegalMoves(legalMoves);

	if (clickedSquare->isOccupied()) {
		if (clickedSquare->getPiece()->getColor() != state->getTurnColor()) return;
		selectSquare(clickedSquare);
	}
}

void GameContext::capturePiece(Piece* piece) {
	gameRenderer->removeFromBoard(piece);
	piece->getSquare()->setPiece(nullptr);
	piece->setSquare(nullptr);
	state->removePiece(piece);
}

void GameContext::finishPromotionMove() {
	updateGameStateAfterMove();

	Square* from = state->getAllSquares()[specialMoves->getPendingPromotionFrom()];
	Square* to = state->getAllSquares()[specialMoves->getPendingPromotionTo()];

	gameRenderer->resetHighlightedMove();
	gameRenderer->highlightLastMove(from, to);

	emit movePlayed(
		specialMoves->getPendingPromotionFrom(),
		specialMoves->getPendingPromotionTo(),
		specialMoves->getCastlingRookFrom(),
		specialMoves->getCastlingRookTo(),
		specialMoves->getPromotionPiece());

	specialMoves->clearSpecialMoveData();
}

void GameContext::offerDraw(Player offerer) {
	if (!drawButtonActive || !gameRenderer->getBoardActive()) return;

	if (!drawOfferActive) {
		drawOfferActive = true;
		gameRenderer->showDrawOfferMessage(offerer.getColor());

		emit drawOffered(offerer.getColor());
		drawButtonActive = false;
		return;
	}

	emit drawAccepted();
	gameEndings->endGame("Draw by agreement");
	emit gameFinished();
}

void GameContext::receiveDrawOffer(PieceColor offerer) {
	drawOfferActive = true;
	gameRenderer->showDrawOfferMessage(offerer);
}

void GameContext::receiveDrawAccepted() {
	gameEndings->endGame("Draw by agreement");
}

void GameContext::clearDrawOffer() {
	drawOfferActive = false;
	drawButtonActive = true;
	gameRenderer->removeDrawOfferMessage();
}

void GameContext::resign(Player loser) {
	if (!gameRenderer->getBoardActive()) return;

	PieceColor winner = (loser.getColor() == PieceColor::White)
		? PieceColor::Black
		: PieceColor::White;

	gameEndings->endGame(colorStrings.at(winner) + " wins by resignation");
	emit playerResigned(loser.getColor());
	emit gameFinished();
}

void GameContext::receiveResignation(PieceColor loser) {
	PieceColor winner = (loser == PieceColor::White)
		? PieceColor::Black
		: PieceColor::White;

	gameEndings->endGame(colorStrings.at(winner) + " wins by resignation");
}

void GameContext::backToLobby() {
	emit returnToLobbyRequested();
}

void GameContext::startClock() {
	elapsedTimer.restart();
	clockTimer->start(50);
}

void GameContext::updateClock() {
	if (!gameRenderer->getBoardActive()) return;
	
	qint64 elapsedMs = elapsedTimer.restart();
	Player& turnPlayer = getTurnPlayer();

	turnPlayer.setRemainingTime(turnPlayer.getRemainingTime() - elapsedMs);

	if (turnPlayer.getRemainingTime() <= 0) {
		auto winner = turnPlayer.getColor() == PieceColor::White ?
			PieceColor::Black : PieceColor::White;
		gameEndings->endGame(colorStrings.at(winner) + " wins by timeout");
	}

	gameRenderer->updateClockDisplay(turnPlayer);
}

void GameContext::stopClock() {
	clockTimer->stop();
}

void GameContext::addIncrement() {
	if (timeControl.increment == 0)
		return;

	Player& playerWhoMoved = state->getTurnColor() == PieceColor::White
		? whitePlayer
		: blackPlayer;

	playerWhoMoved.setRemainingTime(playerWhoMoved.getRemainingTime() + timeControl.increment);
	gameRenderer->updateClockDisplay(playerWhoMoved);
}

void GameContext::setApplyingNetworkMove(bool value) { applyingNetworkMove = value; }

bool GameContext::isApplyingNetworkMove() { return applyingNetworkMove; }
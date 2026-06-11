#include "GameContext.h"
#include "GameState.h"
#include "BoardRenderer.h"
#include "PositionAnalyzer.h"
#include "SpecialMoveHandler.h"
#include "GameEndChecker.h"
#include "Piece.h"
#include "Square.h"
#include "PieceFactory.h"
#include <QBrush>

GameContext::GameContext(GameState* state, BoardRenderer* board, PositionAnalyzer* analyzer,
	SpecialMoveHandler* specialMoves, GameEndChecker* gameEndings) :
	whitePlayer(PieceColor::White, "White"), blackPlayer(PieceColor::Black, "Black"),
	state(state), board(board), analyzer(analyzer), specialMoves(specialMoves), gameEndings(gameEndings) {

	clockTimer = new QTimer();
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

	board->updateClockDisplay(whitePlayer);
	board->updateClockDisplay(blackPlayer);
}

Player& GameContext::getTurnPlayer() {
	return (state->getTurnColor() == PieceColor::White)
		? whitePlayer
		: blackPlayer;
}

Square* GameContext::getSelectedSquare() { return selectedSquare; }

GameState* GameContext::getState() { return state; }
BoardRenderer* GameContext::getBoard() { return board; }

PositionAnalyzer* GameContext::getAnalyzer() { return analyzer; }
SpecialMoveHandler* GameContext::getSpecialMoves() { return specialMoves; }
GameEndChecker* GameContext::getGameEndings() { return gameEndings; }

void GameContext::setFactory(PieceFactory* factory) { this->factory = factory; }

void GameContext::setupStartingPosition(QString orderOfPieces) {
	specialMoves->setInitialKingFile(orderOfPieces.indexOf('K'));

	specialMoves->setInitialQueensideRookFile(orderOfPieces.indexOf('R'));
	int kingsideRookFile = specialMoves->getInitialKingsideRookFile();

	specialMoves->setInitialKingsideRookFile(orderOfPieces.indexOf('R', kingsideRookFile + 1));

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

				board->drawPiece(piece);
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
				board->drawPiece(piece);
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
		board->resetColorOfLegalMoves(legalMoves);
	}

	if (square->isOccupied()) {
		selectedSquare = square;

		Piece* piece = square->getPiece();
		piece->findLegalMoves();
		auto legalMoves = piece->getLegalMoves();

		board->highlightSelected(selectedSquare);
		for (Square* legalMove : legalMoves) {
			board->highlightLegalMove(legalMove);
		}
	}
}

void GameContext::resetSelectedSquare() {
	board->resetColor(selectedSquare);
	selectedSquare = nullptr;
}

void GameContext::updateGameStateAfterMove() {
	addIncrement();
	state->switchTurn();
	state->updateCurrentFen();
	state->updateMoveCount();
	gameEndings->updatePositionCounts();
	gameEndings->ifGameIsOver();
}

void GameContext::handleSquareClick(Square* clickedSquare) {
	if (!board->getBoardActive()) return;
	if (state->getTurnColor() != localPlayerColor) return;
	if (board->getPromotionMenuActive()) return;

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
			board->resetColorOfLegalMoves(legalMoves);
			return;
		}

		updateGameStateAfterMove();

		clearDrawOffer();

		resetSelectedSquare();
		board->resetColorOfLegalMoves(legalMoves);

		board->resetHighlightedMove();
		board->highlightLastMove(originalSquare, state->getAllSquares()[toIndex]);

		if (!applyingNetworkMove) {
			emit movePlayed(
				fromIndex,
				toIndex,
				specialMoves->getCastlingRookFrom(),
				specialMoves->getCastlingRookTo(),
				specialMoves->getPromotionPiece());

			specialMoves->clearSpecialMoveData();
		}

		selectedPiece->resetDestination();

		drawOfferActive = false;
		board->removeDrawOfferMessage();

		return;
	}
	
	resetSelectedSquare();
	board->resetColorOfLegalMoves(legalMoves);

	if (clickedSquare->isOccupied()) {
		if (clickedSquare->getPiece()->getColor() != state->getTurnColor()) return;
		selectSquare(clickedSquare);
	}
}

void GameContext::capturePiece(Piece* piece) {
	board->removeFromBoard(piece);
	piece->getSquare()->setPiece(nullptr);
	piece->setSquare(nullptr);
	state->removePiece(piece);
}

void GameContext::finishPromotionMove() {
	updateGameStateAfterMove();

	Square* from = state->getAllSquares()[specialMoves->getPendingPromotionFrom()];
	Square* to = state->getAllSquares()[specialMoves->getPendingPromotionTo()];

	board->resetHighlightedMove();
	board->highlightLastMove(from, to);

	emit movePlayed(
		specialMoves->getPendingPromotionFrom(),
		specialMoves->getPendingPromotionTo(),
		specialMoves->getCastlingRookFrom(),
		specialMoves->getCastlingRookTo(),
		specialMoves->getPromotionPiece());

	specialMoves->clearSpecialMoveData();
}

void GameContext::offerDraw(Player offerer)
{
	if (!drawOfferActive) {
		drawOfferActive = true;
		board->showDrawOfferMessage(offerer.getColor());

		emit drawOffered(offerer.getColor());
		return;
	}

	gameEndings->endGame("Draw by agreement");
	emit drawAccepted();
}

void GameContext::receiveDrawOffer(PieceColor offerer) {
	drawOfferActive = true;
	board->showDrawOfferMessage(offerer);
}

void GameContext::receiveDrawAccepted() {
	gameEndings->endGame("Draw by agreement");
}

void GameContext::clearDrawOffer() {
	drawOfferActive = false;
	board->removeDrawOfferMessage();
}

void GameContext::resign(Player loser) {
	PieceColor winner = (loser.getColor() == PieceColor::White)
		? PieceColor::Black
		: PieceColor::White;

	gameEndings->endGame(colorStrings.at(winner) + " wins by resignation");
	emit playerResigned(loser.getColor());
}

void GameContext::receiveResignation(PieceColor loser) {
	PieceColor winner = (loser == PieceColor::White)
		? PieceColor::Black
		: PieceColor::White;

	gameEndings->endGame(colorStrings.at(winner) + " wins by resignation");
}

void GameContext::startClock() {
	elapsedTimer.restart();
	clockTimer->start(50);
}

void GameContext::updateClock() {
	if (!board->getBoardActive()) return;
	
	qint64 elapsedMs = elapsedTimer.restart();
	Player& turnPlayer = getTurnPlayer();

	turnPlayer.setRemainingTime(turnPlayer.getRemainingTime() - elapsedMs);

	if (turnPlayer.getRemainingTime() <= 0) {
		auto winner = turnPlayer.getColor() == PieceColor::White ?
			PieceColor::Black : PieceColor::White;
		gameEndings->endGame(colorStrings.at(winner) + " wins by timeout");
	}

	board->updateClockDisplay(turnPlayer);
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
	board->updateClockDisplay(playerWhoMoved);
}

void GameContext::setApplyingNetworkMove(bool value) { applyingNetworkMove = value; }

bool GameContext::isApplyingNetworkMove() { return applyingNetworkMove; }
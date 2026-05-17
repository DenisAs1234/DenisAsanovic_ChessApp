#include "GameContext.h"
#include "GameState.h"
#include "BoardRenderer.h"
#include "PositionAnalyzer.h"
#include "SpecialMoveHandler.h"
#include "GameEndChecker.h"
#include "Piece.h"
#include "Square.h"
#include "PieceFactory.h"
#include<QBrush>

GameContext::GameContext(GameState* state, BoardRenderer* board, PositionAnalyzer* analyzer,
	SpecialMoveHandler* specialMoves, GameEndChecker* gameEndings) :
	whitePlayer(PieceColor::White, "White"), blackPlayer(PieceColor::Black, "Black"),
	state(state), board(board), analyzer(analyzer), specialMoves(specialMoves), gameEndings(gameEndings) {};

Player GameContext::getWhitePlayer() { return whitePlayer; }
Player GameContext::getBlackPlayer() { return blackPlayer; }

Square* GameContext::getSelectedSquare() { return selectedSquare; }

GameState* GameContext::getState() { return state; }
BoardRenderer* GameContext::getBoard() { return board; }

PositionAnalyzer* GameContext::getAnalyzer() { return analyzer; }
SpecialMoveHandler* GameContext::getSpecialMoves() { return specialMoves; }
GameEndChecker* GameContext::getGameEndings() { return gameEndings; }

void GameContext::setFactory(PieceFactory* factory) { this->factory = factory; }

void GameContext::setupStartingPosition() {
	vector<PieceType> startingSetup = { PieceType::Rook, PieceType::Knight, PieceType::Bishop,
		PieceType::Queen, PieceType::King, PieceType::Bishop, PieceType::Knight, PieceType::Rook };
	int file;

	auto allSquares = state->getAllSquares();

	for (int rank = 1; rank <= 8; rank++) {
		file = 0;
		if (rank == 1 || rank == 8) {
			for (PieceType type : startingSetup) {
				PieceColor color = colorsByRank.at(rank);
				QString path = ":/assets/" + colorStrings.at(color) + pieceStrings.at(type) + ".png";
				Square* square = allSquares[(rank - 1) * 8 + file];
				Piece* piece = factory->createPiece(type, color, square, path);
				board->drawPiece(piece);
				square->setPiece(piece);

				if (type == PieceType::King) {
					if (rank == 1) {
						state->setWhiteKingPos(square);
					}
					else {
						state->setBlackKingPos(square);
					}
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
	state->generateFen();
	gameEndings->updatePositionCounts();
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
			board->highlightMove(legalMove);
		}
	}
}

void GameContext::resetSelectedSquare() {
	board->resetColor(selectedSquare);
	selectedSquare = nullptr;
}

void GameContext::handleSquareClick(Square* square) {
	if (!board->getBoardActive()) return;
	if (board->getPromotionMenuActive()) return;

	if (!selectedSquare) {
		if (square->isOccupied() && square->getPiece()->getColor() != state->getTurnColor()) return;
		selectSquare(square);
		return;
	}

	Piece* selectedPiece = selectedSquare->getPiece();

	auto legalMoves = selectedPiece->getLegalMoves();
	if (find(legalMoves.begin(), legalMoves.end(), square) != legalMoves.end()) {
		selectedPiece->moveTo(square);

		resetSelectedSquare();
		board->resetColorOfLegalMoves(legalMoves);

		state->switchTurn();
		state->generateFen();
		gameEndings->updatePositionCounts();
		gameEndings->ifGameIsOver();
		return;
	}
	
	resetSelectedSquare();
	board->resetColorOfLegalMoves(legalMoves);

	if (square->isOccupied()) {
		if (square->getPiece()->getColor() != state->getTurnColor()) return;
		selectSquare(square);
		return;
	}
}

void GameContext::offerDraw(Player player) {

}

void GameContext::acceptDraw() {
	board->showGameOverWindow("Draw by agreement");
}

void GameContext::resign(Player loser) {
	PieceColor winner = (loser.getColor() == PieceColor::White)
		? PieceColor::Black
		: PieceColor::White;

	board->showGameOverWindow(colorStrings.at(winner) + " wins by resignation");
}
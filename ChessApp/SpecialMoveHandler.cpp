#include "SpecialMoveHandler.h"
#include "GameState.h"
#include "BoardRenderer.h"
#include "PositionAnalyzer.h"
#include "GameEndChecker.h"
#include "Square.h"
#include "SquareIndex.h"
#include "Pawn.h"
#include "PieceFactory.h"
#include "GameContext.h"

SpecialMoveHandler::SpecialMoveHandler(GameState* state, BoardRenderer* board, PositionAnalyzer* analyzer) 
	: state(state), board(board), analyzer(analyzer) {}

void SpecialMoveHandler::setContext(GameContext* context) { this->context = context; }
void SpecialMoveHandler::setFactory(PieceFactory* factory) { this->factory = factory; }

void SpecialMoveHandler::checkForPawnsNextTo(Pawn* passingPawn) {
	int rank = passingPawn->getSquare()->getRank();
	int file = passingPawn->getSquare()->getFile();

	for (int i = -1; i <= 1; i += 2) {
		int index = getSquareIndex(rank, file + i);
		if (index == -1) continue;

		Square* nextToPassingPawn = state->getAllSquares()[index];

		if (!nextToPassingPawn->isOccupied()) continue;

		Piece* piece = nextToPassingPawn->getPiece();
		Pawn* pawn = dynamic_cast<Pawn*>(piece);

		if (pawn && pawn->getColor() != passingPawn->getColor()) {
			index = (passingPawn->getColor() == PieceColor::White)
				? getSquareIndex(rank - 1, file)
				: getSquareIndex(rank + 1, file);

			Square* behindPassingPawn = state->getAllSquares()[index];
			pawn->setEnPassantMove(behindPassingPawn);
			state->addEnPassantSquare(behindPassingPawn);
		}
	}
}

void SpecialMoveHandler::checkIfEnPassant(Pawn* movingPawn, Square* destination) {
	if (movingPawn->getEnPassantMove() == destination) {
		executeEnPassant(movingPawn, destination);
	}
}

void SpecialMoveHandler::executeEnPassant(Pawn* movingPawn, Square* destination) {
	int index = (movingPawn->getColor() == PieceColor::White)
		? getSquareIndex(destination->getRank() - 1, destination->getFile())
		: getSquareIndex(destination->getRank() + 1, destination->getFile());

	Square* enPassantPos = state->getAllSquares()[index];
	board->removeFromBoard(enPassantPos->getPiece());
	enPassantPos->setPiece(nullptr);
	movingPawn->setEnPassantMove(nullptr);
}

bool SpecialMoveHandler::isEnPassantLegal(Pawn* movingPawn, Square* destination) {
	int index = (movingPawn->getColor() == PieceColor::White)
		? getSquareIndex(destination->getRank() - 1, destination->getFile())
		: getSquareIndex(destination->getRank() + 1, destination->getFile());

	Square* enPassantPos = state->getAllSquares()[index]; //polje na kojem se nalazi pješak u prolazu
	Piece* onEnPassantPos = enPassantPos->getPiece(); //pješak u prolazu
	enPassantPos->setPiece(nullptr); //micanje pješaka u prolazu s ploèe

	Square* originalSquare = movingPawn->getSquare(); //spremanje originalnog polja ovog pješaka (movingPawn) koji uzima
	originalSquare->setPiece(nullptr); //micanje ovog pješaka s originalnog polja
	movingPawn->setSquare(destination); //polje ovog pješaka postaje njegova destinacija (iza pješaka u prolazu)
	destination->setPiece(movingPawn); //postavljanje ovog pješaka na destinaciju

	bool isKingInCheck = analyzer->isKingInCheck(movingPawn->getColor());

	movingPawn->setSquare(originalSquare); //polje ovog pješaka postaje originalno polje
	enPassantPos->setPiece(onEnPassantPos); //vraæanje pješaka u prolazu na njegovo originalno polje
	destination->setPiece(nullptr); //micanje ovog pješaka s destinacije
	originalSquare->setPiece(movingPawn); //postavljanje ovog pješaka na originalno polje

	return isKingInCheck ? false : true;
}

void SpecialMoveHandler::clearEnPassants() {
	int startOf4thRank = getSquareIndex(4, 0);
	int endOf5thRank = getSquareIndex(5, 7);

	for (int i = startOf4thRank; i <= endOf5thRank; i++) {
		Pawn* pawn = dynamic_cast<Pawn*>(state->getAllSquares()[i]->getPiece());
		if (pawn) {
			pawn->setEnPassantMove(nullptr);
		}
	}
}

bool SpecialMoveHandler::isPromotionPending() { return promotionPending; }
int SpecialMoveHandler::getPendingPromotionFrom() { return pendingPromotionFrom; }
int SpecialMoveHandler::getPendingPromotionTo() { return pendingPromotionTo; }
void SpecialMoveHandler::setPendingPromotionFrom(int fromIndex) { pendingPromotionFrom = fromIndex; }
void SpecialMoveHandler::setPendingPromotionTo(int toIndex) { pendingPromotionTo = toIndex; }
int SpecialMoveHandler::getPromotionPiece() { return promotionPiece; }

bool SpecialMoveHandler::checkIfPromotion(Pawn* promotingPawn, Square* destination) {
	int rank = destination->getRank();

	if (rank != 1 && rank != 8)
		return false;

	if (context->isApplyingNetworkMove()) {
		this->promotingPawn = promotingPawn;
		return true;
	}

	promotionPending = true;
	board->drawPromotionMenu(promotingPawn, destination);
	return true;
}

void SpecialMoveHandler::executePromotion(Pawn* promotingPawn, PieceType type, Square* destination) {
	board->removeFromBoard(promotingPawn);
	state->removePiece(promotingPawn);

	promotionPiece = static_cast<int>(type);
	
	if (!context->isApplyingNetworkMove())
		context->finishPromotionMove();

	QString path = ":/assets/" + colorStrings.at(promotingPawn->getColor()) + pieceStrings.at(type) + ".png";
	Piece* piece = factory->createPiece(type, promotingPawn->getColor(), destination, path);

	board->drawPiece(piece);

	promotingPawn->setPromotedTo(piece);
	destination->setPiece(piece);
}

void SpecialMoveHandler::executePromotionFromNetwork(PieceColor color, PieceType type, Square* destination)
{
	board->removeFromBoard(promotingPawn);
	state->removePiece(promotingPawn);

	QString path = ":/assets/" + colorStrings.at(color) + pieceStrings.at(type) + ".png";
	Piece* piece = factory->createPiece(type, color, destination, path);

	board->drawPiece(piece);
	destination->setPiece(piece);
}

int SpecialMoveHandler::getCastlingRookFrom() { return castlingRookFrom; }
int SpecialMoveHandler::getCastlingRookTo() { return castlingRookTo; }

bool SpecialMoveHandler::canCastle(King* king, CastlingType castlingType) {
	int rank = king->getSquare()->getRank();
	int file = king->getSquare()->getFile();

	int step = (castlingType == CastlingType::Short) ? 1 : -1;
	file += step;
	auto allSquares = state->getAllSquares();
	Square* kingDestination = (castlingType == CastlingType::Short)
		? allSquares[getSquareIndex(rank, 6)]
		: allSquares[getSquareIndex(rank, 2)];

	while (file >= 0 && file <= 7) {
		int index = getSquareIndex(rank, file);
		Rook* rook = dynamic_cast<Rook*>(allSquares[index]->getPiece());

		if (allSquares[index]->isOccupied() && !rook) return false;
		if (!analyzer->isSquareSafe(allSquares[index])) return false;
		if (!allSquares[index]->isOccupied()) {
			file += step;
			continue;
		}
		if (rook->getHasMoved()) return false;
		king->getCastlingMoves().push_back(pair<Square*, Rook*>(kingDestination, rook));
		return true;
	}
	return false;
}

void SpecialMoveHandler::checkIfCastlingMove(King* king, Square* destination) {
	if (context->isApplyingNetworkMove())
		return;
	king->findVisibleSquares();
	auto visibleSquares = king->getVisibleSquares();

	bool isCastlingMove = find(visibleSquares.begin(), visibleSquares.end(), destination)
		== visibleSquares.end();

	if (isCastlingMove) {
		executeCastling(king, destination);
	}
}

void SpecialMoveHandler::executeCastling(King* king, Square* destination) {
	auto allSquares = state->getAllSquares();
	Square* rookDestination = (destination->getFile() == 6)
		? allSquares[getSquareIndex(destination->getRank(), 5)]
		: allSquares[getSquareIndex(destination->getRank(), 3)];
	
	for (auto& castlingMove : king->getCastlingMoves()) {
		if (destination == castlingMove.first) {
			castlingRookFrom = castlingMove.second->getSquare()->getIndex();
			castlingRookTo = rookDestination->getIndex();

			castlingMove.second->moveTo(rookDestination);
			break;
		}
	}

	if (king->getColor() == PieceColor::White) {
		state->removeCastlingRight('K');
		state->removeCastlingRight('Q');
		return;
	}
	state->removeCastlingRight('k');
	state->removeCastlingRight('q');
}

void SpecialMoveHandler::executeAtomicCapture(Square* captureSquare, Piece* capturingPiece) {
	vector<Piece*> toBeCaptured = findAdjacentPieces(captureSquare);

	toBeCaptured.push_back(capturingPiece);
	
	for (Piece* piece : toBeCaptured) {
		Rook* rook = dynamic_cast<Rook*>(piece);
		if (rook) {
			rook->onCapture();
		}

		if (piece->getType() == PieceType::King) {
			context->getGameEndings()->endGame(
				colorStrings.at(capturingPiece->getColor()) + " wins by exploding the king");
		}

		context->capturePiece(piece);
	}
}

vector<Piece*> SpecialMoveHandler::findAdjacentPieces(Square* captureSquare) {
	vector<Piece*> adjacentPieces;

	int captureRank = captureSquare->getRank();
	int captureFile = captureSquare->getFile();

	vector<pair<int, int>> directions = { {1,1}, {1,-1}, {-1,1}, {-1,-1}, {1,0}, {0,1}, {-1,0}, {0,-1} };

	for (auto& dir : directions) {
		int adjacentRank = captureRank + dir.first;
		int adjacentFile = captureFile + dir.second;
		int adjacentIndex = getSquareIndex(adjacentRank, adjacentFile);

		if (adjacentIndex == -1) continue;

		Square* adjacentSquare = state->getAllSquares()[adjacentIndex];
		Piece* adjacentPiece = adjacentSquare->getPiece();

		if (adjacentSquare->isOccupied() && adjacentPiece->getType() != PieceType::Pawn) {
			adjacentPieces.push_back(adjacentPiece);
		}
	}

	return adjacentPieces;
}

void SpecialMoveHandler::clearSpecialMoveData() {
	castlingRookFrom = -1;
	castlingRookTo = -1;
	promotionPiece = -1;
	pendingPromotionFrom = -1;
	pendingPromotionTo = -1;
	promotionPending = false;
	promotingPawn = nullptr;
}
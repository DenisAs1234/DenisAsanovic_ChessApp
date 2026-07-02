#include "SpecialMoveHandler.h"
#include "GameState.h"
#include "GamePageRenderer.h"
#include "PositionAnalyzer.h"
#include "GameEndChecker.h"
#include "Square.h"
#include "SquareIndex.h"
#include "Pawn.h"
#include "PieceFactory.h"
#include "GameContext.h"

SpecialMoveHandler::SpecialMoveHandler(GameState* state, GamePageRenderer* gameRenderer, 
	PositionAnalyzer* analyzer) 
	: state(state), gameRenderer(gameRenderer), analyzer(analyzer) {}

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
	gameRenderer->removeFromBoard(enPassantPos->getPiece());
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
	if (!promotingPawn->getSquare()) 
		return false;

	int rank = destination->getRank();

	if (rank != 1 && rank != 8)
		return false;

	if (context->isApplyingNetworkMove()) {
		this->promotingPawn = promotingPawn;
		return true;
	}

	promotionPending = true;
	gameRenderer->drawPromotionMenu(promotingPawn, destination);
	return true;
}

void SpecialMoveHandler::executePromotion(Pawn* promotingPawn, PieceType type, Square* destination) {
	gameRenderer->removeFromBoard(promotingPawn);
	state->removePiece(promotingPawn);

	promotionPiece = static_cast<int>(type);
	
	if (!context->isApplyingNetworkMove())
		context->finishPromotionMove();

	QString path = ":/assets/" + colorStrings.at(promotingPawn->getColor()) + pieceStrings.at(type) + ".png";
	Piece* piece = factory->createPiece(type, promotingPawn->getColor(), destination, path);

	gameRenderer->drawPiece(piece);

	promotingPawn->setPromotedTo(piece);
	destination->setPiece(piece);
}

void SpecialMoveHandler::executePromotionFromNetwork(PieceColor color, PieceType type, Square* destination)
{
	gameRenderer->removeFromBoard(promotingPawn);
	state->removePiece(promotingPawn);

	QString path = ":/assets/" + colorStrings.at(color) + pieceStrings.at(type) + ".png";
	Piece* piece = factory->createPiece(type, color, destination, path);

	gameRenderer->drawPiece(piece);
	destination->setPiece(piece);
}

void SpecialMoveHandler::setInitialKingFile(int file) { initialKingFile = file; }
void SpecialMoveHandler::setInitialKingsideRookFile(int file) { initialKingsideRookFile = file; }
void SpecialMoveHandler::setInitialQueensideRookFile(int file) { initialQueensideRookFile = file; }
int SpecialMoveHandler::getInitialKingsideRookFile() { return initialKingsideRookFile; }
int SpecialMoveHandler::getInitialQueensideRookFile() { return initialQueensideRookFile; }

int SpecialMoveHandler::getCastlingRookFrom() { return castlingRookFrom; }
int SpecialMoveHandler::getCastlingRookTo() { return castlingRookTo; }

bool SpecialMoveHandler::canCastle(CastlingType castlingType) {
	PieceColor turnColor = state->getTurnColor();
	bool isShort = castlingType == CastlingType::Short;

	char castlingChar = isShort
		? (turnColor == PieceColor::White ? 'K' : 'k')
		: (turnColor == PieceColor::White ? 'Q' : 'q');

	if (state->getCastlingRights().indexOf(castlingChar) == -1) return false;

	auto allSquares = state->getAllSquares();

	int rank = (turnColor == PieceColor::White) ? 1 : 8;
	int file = initialKingFile;

	Square* kingDestination = isShort
		? allSquares[getSquareIndex(rank, 6)]
		: allSquares[getSquareIndex(rank, 2)];
	int kingDestinationFile = kingDestination->getFile();

	if (initialKingFile == kingDestinationFile) {
		return isRookUnobstructed(isShort ? initialKingsideRookFile : initialQueensideRookFile,
			                      isShort ? 5 : 3);
	}

	int step = (initialKingFile < kingDestinationFile) ? 1 : -1;
	file += step;

	while (file >= 0 && file <= 7) {
		int index = getSquareIndex(rank, file);
		Rook* rook = dynamic_cast<Rook*>(allSquares[index]->getPiece());

		if (allSquares[index]->isOccupied() && !rook) 
			return false;

		if (!analyzer->isSquareSafe(allSquares[index])) 
			return false;

		if (file == kingDestinationFile) {
			return isRookUnobstructed(isShort ? initialKingsideRookFile : initialQueensideRookFile,
									  isShort ? 5 : 3);
		}

		file += step;
	}
}

bool SpecialMoveHandler::isRookUnobstructed(int currentFile, int destinationFile) {
	if (currentFile == destinationFile) return true;

	int step = (currentFile < destinationFile) ? 1 : -1;

	int rank = state->getTurnColor() == PieceColor::White ? 1 : 8;
	int file = currentFile;

	auto allSquares = state->getAllSquares();

	do {
		file += step;

		int index = getSquareIndex(rank, file);
		King* king = dynamic_cast<King*>(allSquares[index]->getPiece());

		if (allSquares[index]->isOccupied() && !king)
			return false;

	} while (file != destinationFile);

	return true;
}
/*
Square* SpecialMoveHandler::checkIfCastlingMove(King* king, Square* clickedSquare) {
	if (context->isApplyingNetworkMove())
		return nullptr;
	
	if (!clickedSquare->isOccupied()) 
		return nullptr;

	if (clickedSquare->getPiece()->getColor() != state->getTurnColor())
		return nullptr;

	auto allSquares = state->getAllSquares();

	Square* kingDestination = (initialKingFile < clickedSquare->getFile())
		? allSquares[getSquareIndex(clickedSquare->getRank(), 6)]
		: allSquares[getSquareIndex(clickedSquare->getRank(), 2)];

	Square* rookDestination = (kingDestination->getFile() == 6)
		? allSquares[getSquareIndex(clickedSquare->getRank(), 5)]
		: allSquares[getSquareIndex(clickedSquare->getRank(), 3)];

	castlingRookFrom = clickedSquare->getIndex();
	castlingRookTo = rookDestination->getIndex();

	executeCastling(king, kingDestination);
	return kingDestination;
}

Square* SpecialMoveHandler::checkIfCastlingMove(King* king, Square* clickedSquare) {
	if (context->isApplyingNetworkMove())
		return nullptr;

	if (!clickedSquare->isOccupied())
		return nullptr;

	if (clickedSquare->getPiece()->getColor() != state->getTurnColor())
		return nullptr;

	auto allSquares = state->getAllSquares();

	Square* destination = (initialKingFile < clickedSquare->getFile())
		? allSquares[getSquareIndex(clickedSquare->getRank(), 6)]
		: allSquares[getSquareIndex(clickedSquare->getRank(), 2)];

	executeCastling(king, destination);
	return destination;
}*/

Square* SpecialMoveHandler::checkIfCastlingMove(King* king, Square* clickedSquare) {
	/*if (context->isApplyingNetworkMove())
		return nullptr;*/

	if (!clickedSquare->isOccupied())
		return nullptr;

	if (clickedSquare->getPiece()->getColor() != state->getTurnColor())
		return nullptr;

	auto allSquares = state->getAllSquares();

	Square* kingDestination =
		(initialKingFile < clickedSquare->getFile())
		? allSquares[getSquareIndex(clickedSquare->getRank(), 6)]
		: allSquares[getSquareIndex(clickedSquare->getRank(), 2)];

	Square* rookDestination =
		(kingDestination->getFile() == 6)
		? allSquares[getSquareIndex(clickedSquare->getRank(), 5)]
		: allSquares[getSquareIndex(clickedSquare->getRank(), 3)];

	Square* rookStart =
		(kingDestination->getFile() == 6)
		? allSquares[getSquareIndex(clickedSquare->getRank(), initialKingsideRookFile)]
		: allSquares[getSquareIndex(clickedSquare->getRank(), initialQueensideRookFile)];

	prepareCastling(king, kingDestination, rookStart, rookDestination);

	return kingDestination;
}

void SpecialMoveHandler::prepareCastling(King* king, Square* kingDestination,
	Square* rookStart, Square* rookDestination)
{
	castlingData.king = king;
	castlingData.rook = dynamic_cast<Rook*>(rookStart->getPiece());

	castlingData.kingStart = king->getSquare();
	castlingData.kingDestination = kingDestination;

	castlingData.rookStart = rookStart;
	castlingData.rookDestination = rookDestination;

	castlingRookFrom = rookStart->getIndex();
	castlingRookTo = rookDestination->getIndex();
}

void SpecialMoveHandler::executeCastling() {
	castlingData.kingStart->setPiece(nullptr);
	castlingData.rookStart->setPiece(nullptr);

	castlingData.king->setSquare(castlingData.kingDestination);
	castlingData.rook->setSquare(castlingData.rookDestination);

	castlingData.kingDestination->setPiece(castlingData.king);
	castlingData.rookDestination->setPiece(castlingData.rook);

	castlingData.king->setPos(
		castlingData.kingDestination->getX() + 5,
		castlingData.kingDestination->getY() + 7);

	castlingData.rook->setPos(
		castlingData.rookDestination->getX() + 5,
		castlingData.rookDestination->getY() + 7);

	if (castlingData.king->getColor() == PieceColor::White)
	{
		state->removeCastlingRight('K');
		state->removeCastlingRight('Q');
	}
	else
	{
		state->removeCastlingRight('k');
		state->removeCastlingRight('q');
	}

	castlingData.clear();
}
/*
void SpecialMoveHandler::executeCastling(King* king, Square* destination) {
	auto allSquares = state->getAllSquares();

	Square* rookDestination = (destination->getFile() == 6)
		? allSquares[getSquareIndex(destination->getRank(), 5)]
		: allSquares[getSquareIndex(destination->getRank(), 3)];

	Square* rookPosition = (destination->getFile() == 6)
		? allSquares[getSquareIndex(destination->getRank(), initialKingsideRookFile)]
		: allSquares[getSquareIndex(destination->getRank(), initialQueensideRookFile)];

	Rook* castlingRook = dynamic_cast<Rook*>(rookPosition->getPiece());

	castlingRookFrom = rookPosition->getIndex();
	castlingRookTo = rookDestination->getIndex();

	castlingRook->moveTo(rookDestination);

	if (king->getColor() == PieceColor::White) {
		state->removeCastlingRight('K');
		state->removeCastlingRight('Q');
		return;
	}

	state->removeCastlingRight('k');
	state->removeCastlingRight('q');
}*/
/*
void SpecialMoveHandler::executeCastling(
	King* king,
	Square* kingDestination,
	Square* rookStart,
	Square* rookDestination)
{
	Rook* rook = dynamic_cast<Rook*>(rookStart->getPiece());

	Square* kingStart = king->getSquare();

	// makni obje figure s poèetnih polja
	kingStart->setPiece(nullptr);
	rookStart->setPiece(nullptr);

	// promijeni interne pokazivaèe
	king->setSquare(kingDestination);
	rook->setSquare(rookDestination);

	// postavi figure na nova polja
	kingDestination->setPiece(king);
	rookDestination->setPiece(rook);

	// pomakni spriteove
	king->setPos(
		kingDestination->getX() + 5,
		kingDestination->getY() + 7);

	rook->setPos(
		rookDestination->getX() + 5,
		rookDestination->getY() + 7);

	// ažuriraj prava na rošadu
	if (king->getColor() == PieceColor::White) {
		state->removeCastlingRight('K');
		state->removeCastlingRight('Q');
	}
	else {
		state->removeCastlingRight('k');
		state->removeCastlingRight('q');
	}
}*/
/*
void SpecialMoveHandler::executeCastling(King* king, Square* destination) {
	auto allSquares = state->getAllSquares(); 

	Square* rookDestination = (destination->getFile() == 6)
		? allSquares[getSquareIndex(destination->getRank(), 5)]
		: allSquares[getSquareIndex(destination->getRank(), 3)];

	Square* rookPosition = (destination->getFile() == 6)
		? allSquares[getSquareIndex(destination->getRank(), initialKingsideRookFile)]
		: allSquares[getSquareIndex(destination->getRank(), initialQueensideRookFile)];

	Rook* castlingRook = dynamic_cast<Rook*>(rookPosition->getPiece());

	castlingRookFrom = rookPosition->getIndex();
	castlingRookTo = rookDestination->getIndex();

	castlingRook->moveTo(rookDestination);

	if (king->getColor() == PieceColor::White) {
		state->removeCastlingRight('K');
		state->removeCastlingRight('Q');
		return;
	}

	state->removeCastlingRight('k');
	state->removeCastlingRight('q');
}
*/
/*
void SpecialMoveHandler::executeCastling() {
	auto allSquares = state->getAllSquares();

	Square* rookStart = allSquares[castlingRookFrom];
	Square* rookEnd = allSquares[castlingRookTo];

	if (rookStart != rookEnd) {
		Rook* rook = dynamic_cast<Rook*>(rookStart->getPiece());

		if (rook)
			rook->moveTo(rookEnd);
	}

	if (state->getTurnColor() == PieceColor::White) {
		state->removeCastlingRight('K');
		state->removeCastlingRight('Q');
	}
	else {
		state->removeCastlingRight('k');
		state->removeCastlingRight('q');
	}
}
*/
void SpecialMoveHandler::executeAtomicCapture(Square* captureSquare, Piece* capturingPiece) {
	vector<Piece*> toBeCaptured = findAdjacentPieces(captureSquare);

	toBeCaptured.push_back(capturingPiece);
	
	for (Piece* piece : toBeCaptured) {
		Rook* rook = dynamic_cast<Rook*>(piece);
		if (rook) {
			rook->onCapture();
		}

		if (piece->getType() == PieceType::King) {
			context->getGameEndings()->setIsGameOver(true);

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
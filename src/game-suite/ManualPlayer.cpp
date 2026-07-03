#include "Board.h"
#include "ManualPlayer.h"
#include "globals/Piece.h"
#include "trix-bots/Utils.h"

#include <algorithm>
#include <format>
#include <string>
#include <thread>
#include <WinUser.h>


void ManualPlayer::getMoveToPlay(uint8_t(*board)[13][13], bool isWhite, int endTime)
{
	targetBoard = board;
	isActive = true;
	this->isWhite = isWhite;
	resetMove(false);

	Utils::print(std::format("Ready for {} player input", 
		isWhite ? "white" : "black"), true);

	while (isActive)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void ManualPlayer::keyDown(const PlayerInputKey key)
{
	switch (key)
	{
	case PlayerInputKey::None:
		return;
	// --- Commands ---
	case PlayerInputKey::Undo:
		break;
	case PlayerInputKey::Reset:
		resetMove(true);
		break;
	case PlayerInputKey::Submit:
		endMove();
		return;

	// --- Directions ---
	case PlayerInputKey::Down:
		moveInDirection(Direction::Down);
		break;
	case PlayerInputKey::Up:
		moveInDirection(Direction::Up);
		break;
	case PlayerInputKey::Left:
		moveInDirection(Direction::Left);
		break;
	case PlayerInputKey::Right:
		moveInDirection(Direction::Right);
		break;

	// --- Turn ---
	case PlayerInputKey::Turn:
		turn();
		break;
	
	// --- Numbers ---
	case PlayerInputKey::One:
		changeSplitSelection(1);
		break;
	case PlayerInputKey::Two:
		changeSplitSelection(2);
		break;
	case PlayerInputKey::Three:
		changeSplitSelection(3);
		break;
	case PlayerInputKey::Four:
		changeSplitSelection(4);
		break;
	case PlayerInputKey::Five:
		changeSplitSelection(5);
		break;

	// --- Switch Mode ---
	case PlayerInputKey::SwitchPushMergeMode:
		switchPushMergeMode();
		break;
	}
	updateDisplay();
}

void ManualPlayer::mouseDown(bool isLeft, POINT gridPos)
{
	if (!isActive || gridPos.x == -1 || gridPos.y == -1)
		return;

	changeTowerSelection(gridPos.x, gridPos.y);

	updateDisplay();
}

void ManualPlayer::moveInDirection(ManualPlayer::Direction dir)
{
	int destX = moveInfo.currentX, 
		destY = moveInfo.currentY;
	if (destX == -1 && destY == -1)
	{
		Utils::print("No tower selected.", true);
		return;
	}
	switch (dir)
	{
	case Direction::Up:
		destY--;
		break;
	case Direction::Down:
		destY++;
		break;
	case Direction::Left:
		destX--;
		break;
	case Direction::Right:
		destX++;
		break;
	}
	// check for steps
	if (moveInfo.stepsMade >= maxSteps)
	{
		Utils::print("Step limit reached.", true);
		return;
	}
	// check for bounds
	if (destX < 0 || destX > 12 || destY < 0 || destY > 12)
	{
		Utils::print("Cannot move off the board.", true);
		return;
	}
	// Check for moving back
	if (std::any_of(prevStates.begin(), prevStates.end(), [destX, destY](const std::pair<uint8_t(*)[13], MoveInfo> pair)
		{
			const MoveInfo info = pair.second;
			return info.currentX == destX && info.currentY == destY;
		}))
	{
		Utils::print("Cannot visit the same square twice.", true);
		return;
	}

	uint8_t *const pDestPiece = &(*displayBoard)[destX][destY];
	uint8_t *const pStartPiece = &(*displayBoard)[moveInfo.currentX][moveInfo.currentY];
	const uint8_t destTP = Piece::turnPiece(*pDestPiece);
	const uint8_t startTP = Piece::turnPiece(*pStartPiece);

	const bool isCapturing = (Piece::height(*pDestPiece) > 0) && (Piece::isTower(*pDestPiece) && Piece::isWhite(*pDestPiece) != isWhite);
	const bool isMergeOrPush = (Piece::height(*pDestPiece) > 0) && (Piece::isAddOn(*pDestPiece) || Piece::isWhite(*pDestPiece) == isWhite);

	// check for block
	if ((destX % 2 + destY % 2 == 2) || // Board hole
		(
			destTP && //blocking turn piece on dest
			(((bool)(Piece::setTurnPiece & *pDestPiece))
			^ (dir == Direction::Up || dir == Direction::Down))
		) || (
			startTP && // blocking turn piece on start
			(((bool)(Piece::setTurnPiece & *pStartPiece))
				^ (dir == Direction::Up || dir == Direction::Down))
		))
	{
		Utils::print("Path is blocked.", true);
		return;
	}
	
	// check for illegal captures
	if (isCapturing) {
		if (moveInfo.blueProperty)
		{
			if ((moveInfo.stepsMade + 1 > maxSteps && !moveInfo.isCapturing)	// must have enough moves for first capture
				|| (!moveInfo.isSplitting && Piece::height(moveInfo.currentSelection) != 1)) // must split if not h1
			{
				Utils::print("Cannot capture.", true);
				return;
			}
		}
		else
		{
			if (Piece::height(*pDestPiece) > Piece::height(moveInfo.currentSelection) // capture no bigger towers
				|| moveInfo.stepsMade + 2 > maxSteps //must have enough moves
				|| (!moveInfo.isSplitting && Piece::height(moveInfo.currentSelection) != 1)) // must split if not h1
			{
				Utils::print("Cannot capture.", true);
				return;
			}
		}
	}
	if (moveInfo.hasCaptured)
	{
		Utils::print("Cannot move after finished capture.", true);
		return;
	}
	if (moveInfo.isMerging)
	{
		Utils::print("Cannot move after merge.", true);
		return;
	}
	if (moveInfo.isCapturing && moveInfo.isSplitting)
	{
		Utils::print("Cannot split while capturing.", true);
		return;
	}
		
	// check for illegal merge
	if (isMergeOrPush && isMergeMode 
		&& (moveInfo.split + Piece::height(*pDestPiece) > 5 //result too high
			|| (Piece::hasAddOn(moveInfo.currentSelection) && !moveInfo.isSplitting))) // can't merge using addOn
	{
		Utils::print("Merge is illegal.", true);
		return;
	}
	// check for illegal push
	if (isMergeOrPush && !isMergeMode
		&& (moveInfo.isSplitting
			|| moveInfo.isCapturing
			|| Piece::height(*pDestPiece) > Piece::height(moveInfo.currentSelection))) // too high
		// TODO: Incomplete, must push check for all pushed towers
	{
		Utils::print("Push is illegal.", true);
		return;
	}

	// --- save state ---
	prevStates.push_back(std::pair(*displayBoard, moveInfo));
	MoveInfo newMoveInfo = moveInfo;
	
	// -- update new moveInfo --
	newMoveInfo.stepsMade++;
	newMoveInfo.currentX = destX;
	newMoveInfo.currentY = destY;
	newMoveInfo.isSplitting = false;
	newMoveInfo.isCapturing = isCapturing;
	newMoveInfo.isMerging = isMergeOrPush && isMergeMode;
	if (isMergeOrPush && !isMergeMode)
	{
		newMoveInfo.currentSelection = 0;
		Utils::print("!!! WARNING - Push not implemented yet !!!", true);
	}
	else if (isCapturing)
	{
		newMoveInfo.currentSelection = 
			destTP
			| (isWhite ? Piece::White : Piece::Black)
			| (moveInfo.isSplitting ? 0 : Piece::addOn(moveInfo.currentSelection))
			+ moveInfo.split;
	}
	else
	{
		// this might seem weird, but it handles splits, merges and normal moves in one.
		newMoveInfo.currentSelection =
			(*pDestPiece
			| (isWhite ? Piece::White : Piece::Black)
			| (moveInfo.isSplitting ? 0 : Piece::addOn(moveInfo.currentSelection)))
			+ moveInfo.split;
	}
	newMoveInfo.split = Piece::height(newMoveInfo.currentSelection);

	// update blue for empty dest 
	if (moveInfo.isSplitting && !Piece::isTower(*pDestPiece) && !Piece::isAddOn(*pDestPiece))
		newMoveInfo.blueProperty = false;

	if (moveInfo.isCapturing && !isCapturing)
		newMoveInfo.hasCaptured = true;


	// --- update board ----

	*pDestPiece = newMoveInfo.currentSelection;
	*pStartPiece = Piece::turnPiece(*pStartPiece) 
		| (moveInfo.isSplitting ? Piece::tower(moveInfo.currentSelection) - moveInfo.split : 0);

	// correct left behind addon color
	if(Piece::hasAddOn(moveInfo.currentSelection) && moveInfo.split == Piece::height(moveInfo.currentSelection) -1)
		*pStartPiece &= ~Piece::colourMask;
	

	ui->setHighlight(destX, destY);
	moveInfo = newMoveInfo;
}

void ManualPlayer::resetMove(const bool print)
{
	if (print)
		Utils::print("Resetting position", true);
	std::memcpy(displayBoard, targetBoard, sizeof(*displayBoard));
	
	prevStates.clear();
	moveInfo = MoveInfo();
	startTower = 0;
	maxSteps = 0;
	ui->clearAllHighlights();
	
}

void ManualPlayer::updateDisplay()
{
	InvalidateRect(globalHwnd, NULL, NULL);
}

void ManualPlayer::turn()
{
	uint8_t* pPiece = &(*displayBoard)[moveInfo.currentX][moveInfo.currentY];
	
	if (!Piece::turnPiece(*pPiece))
	{
		Utils::print("Unable to turn without turn piece.", true);
		return;
	}

	// save state
	prevStates.push_back(std::pair(*displayBoard, moveInfo));
	
	*pPiece = (*pPiece ^ Piece::setTurnPiece);
}

void ManualPlayer::changeSplitSelection(const int amountToMoveWith)
{
	const int height = Piece::height(moveInfo.currentSelection);
	if (amountToMoveWith >= height)
	{
		moveInfo.split = height;
		moveInfo.isSplitting = false;
	}
	else {
		moveInfo.split = max(amountToMoveWith, 1);
		moveInfo.isSplitting = true;
	}
	Utils::print("Changing selection to: ", false);
	Utils::print(moveInfo.split, true);
}

void ManualPlayer::changeTowerSelection(const int x, const int y)
{
	resetMove(false);

	uint8_t targetContent = (*displayBoard)[x][y];

	if (!Piece::isTower(targetContent) || !(Piece::isWhite(targetContent) == isWhite))
	{
		Utils::print("Deselecting", true);
		return;
	}
	startTower = targetContent;
	maxSteps = Piece::maxSteps(targetContent);

	moveInfo.currentX = x;
	moveInfo.currentY = y;
	moveInfo.currentSelection = targetContent;
	moveInfo.split = Piece::height(targetContent);
	moveInfo.blueProperty = Piece::isBlue(targetContent);

	ui->setHighlight(x, y);
	Utils::print("Selecting", true);
}
void ManualPlayer::endMove()
{
	if (!isValdMoveToEnd())
	{
		Utils::print("Cannot end move here", true);
		return;
	}

	ui->clearAllHighlights();
	memcpy(targetBoard, displayBoard, sizeof(*targetBoard));
	isActive = false;
}

bool ManualPlayer::isValdMoveToEnd() const
{
	if (moveInfo.isCapturing)
		return false;

	return true;
}

void ManualPlayer::switchPushMergeMode()
{
	isMergeMode = !isMergeMode;

	Utils::print("Switching to ", false);
	Utils::print(isMergeMode ? "Merge Mode." : "Push Mode.", true);
}
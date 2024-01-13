#include "ManualPlayer.h"
#include <string>
#include "BoardState.h"
#include <WinUser.h>
#include <thread>
#include "Trix Bots/Utils.h"
#include <format>
#include "Globals/Piece.h"

void ManualPlayer::getMoveToPlay(uint8_t(*board)[13][13], bool isWhite, int endTime)
{
	targetBoard = board;
	isActive = true;
	isWhite = isWhite;
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
	if (key == PlayerInputKey::None)
		return;

	if (key == PlayerInputKey::Turn) 
	{
		// Testing action
		std::string seagull = "b-10000 b-10012 b-11200 b-11212 rW20006 r-11206 -B10206 -B20204 -B30500 -B30700 -W30512 -W30712 -W10605 -W10607 -W10808 -W20809 -B21104 11110111111111111111";
		BoardState bsService;

		bsService.loadPos(seagull);
		bsService.copyBoard(displayBoard);
		updateDisplay();
	}
	else if (key == PlayerInputKey::Reset)
	{
		resetMove(true);
		isActive = false;
	}
}


void ManualPlayer::moveInDirection(ManualPlayer::Direction dir)
{
	// check for bounds
	if ((dir == Direction::Up && moveInfo.currentY == 0) ||
		(dir == Direction::Down && moveInfo.currentY == 12) ||
		(dir == Direction::Left && moveInfo.currentX == 0) ||
		(dir == Direction::Right && moveInfo.currentX == 12))
	{
		Utils::print("Cannot move off the board", true);
		return;
	}

	MoveInfo newMoveInfo = moveInfo;
	newMoveInfo.stepsMade++;
	
	//apply direction
	switch (dir)
	{
	case Direction::Up:
		newMoveInfo.currentY--;
		break;
	case Direction::Right:
		newMoveInfo.currentX++;
		break;
	case Direction::Down:
		newMoveInfo.currentY++;
		break;
	case Direction::Left:
		newMoveInfo.currentX--;
		break;
	}
}

void  ManualPlayer::resetMove(bool print)
{
	if (print)
		Utils::print("Resetting position", true);
	std::memcpy(displayBoard, targetBoard, sizeof(&displayBoard));

	updateDisplay();
	
	while (!prevStateStack.empty())
	{
		std::pair<uint8_t[13][13], MoveInfo> prevState = prevStateStack.top();
		prevStateStack.pop();

		moveInfo = prevState.second;
		// the board itself was already reset completely
	}

	startTower = 0;
	maxSteps = 0;
	
}

void ManualPlayer::updateDisplay()
{
	InvalidateRect(globalHwnd, NULL, NULL);
}

void ManualPlayer::changeSplitSelection(const uint8_t amountToMoveWith)
{
	moveInfo.splitSelection = max(0, Piece::height(moveInfo.currentSelection) - amountToMoveWith);
}

void ManualPlayer::changeTowerSelection(const uint8_t x, const uint8_t y)
{
	resetMove(true);

	uint8_t targetContent = (*displayBoard)[x][y];

	if (!Piece::isTower(targetContent) || !(Piece::isWhite(targetContent) == isWhite))
	{
		Utils::print("Deselecting", true);
		return;
	}
	startTower = targetContent;
	maxSteps = getMaxSteps(targetContent);
	moveInfo.currentSelection = targetContent;
	moveInfo.currentX = x;
	moveInfo.currentY = y;

	moveInfo.blueProperty = Piece::isBlue(targetContent);
}

bool ManualPlayer::isValdMoveToEnd() const
{
	if (moveInfo.isCapturing)
		return false;

	return true;
}

uint8_t ManualPlayer::getMaxSteps(const uint8_t tower) const
{
	if (!Piece::isTower(tower))
		return 0;
	switch (Piece::height(tower))
	{
	case 1:
		return 4;
	case 2:
	case 3:
		return 3 + Piece::isRed(tower);
	case 4:
	case 5:
		return 2 + Piece::isRed(tower);
	default:
		return 0;
	}
}
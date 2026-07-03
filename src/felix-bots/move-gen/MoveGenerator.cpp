#include "felix-bots/move-gen/MoveGenerator.h"
#include "felix-bots/util/Utility.h"

#include "globals/Piece.h"
using namespace dc;

MoveGenerator MoveGenerator::moveGen;
void MoveGenerator::getMovesStatic(std::vector<dc::Move> *targetList, const GamePosition& state, const bool isWhite, const bool onlyClaimsAndCaptures)
{
	moveGen.getMoves(targetList, state, isWhite, onlyClaimsAndCaptures);
}

void MoveGenerator::getMoves(std::vector<dc::Move> *targetList, const GamePosition& state, const bool isWhite, const bool onlyClaimsAndCaptures)
{
	target = targetList;
	target->reserve(onlyClaimsAndCaptures ? 80 : 550);

	currentSquares = state;
	origSquares = state;
	memset(visited, false, sizeof(visited));

	for (int i = 0; i < 13; i++)
	{
		for (int j = 0; j < 13; j++)
		{
			uint8_t piece = currentSquares[i][j];
			if (Piece::height(piece) == 0 || Piece::isAddOn(piece))
				continue;
			if (Piece::isWhite(piece) == !isWhite)
				continue;

			visited[i][j] = true;
			basicGenerator(i, j, i, j, Piece::maxSteps(piece), false, isWhite, onlyClaimsAndCaptures, false, false);
			visited[i][j] = false;
		}
	}
	target = nullptr;
}

void MoveGenerator::basicGenerator(
	int x, int y,
	const int startX, const int startY,
	int remainingSteps,
	bool turned, bool isWhite, bool onlyClaimsAndCaptures, bool leftBehindAddon, bool leftRed)
{
	// Save move if start Tower has moved
	if (!onlyClaimsAndCaptures && origSquares[startX][startY] != currentSquares[startX][startY])
	{
		auto &move = target->emplace_back();
		move.reserve(5);

		move.emplace_back(startX, startY, (uint8_t)(origSquares[startX][startY] ^ currentSquares[startX][startY]));
		if (startX != x || startY != y)
			move.emplace_back(x, y, (uint8_t)(origSquares[x][y] ^ currentSquares[x][y]));

		for (int i = 0; i < 13; i++)
		{
			for (int j = 0; j < 13; j++)
			{
				if (origSquares[i][j] == currentSquares[i][j] || (startX == i && startY == j) || (x == i && y == j))
					continue;

				move.emplace_back(i, j, (uint8_t)(origSquares[i][j] ^ currentSquares[i][j])); // Creation of xor lists for moves
			}
		}
	}

	// Try to turn if possible and not happened yet
	if (!turned && ((currentSquares[x][y] & hasTurnPiece) != 0))
	{
		currentSquares[x][y] ^= setTurnPiece;
		basicGenerator(x, y, startX, startY, remainingSteps, true, isWhite, onlyClaimsAndCaptures, leftBehindAddon, leftRed);
		currentSquares[x][y] ^= setTurnPiece;
	}

	if (remainingSteps == 0)
		return;

	// Move one square further
	// Loop through the 4 possible directions
	for (int d = 0; d < 4; d++)
	{
		const uint8_t piece = currentSquares[x][y];

		// Check the current turn piece
		if ((piece & hasTurnPiece) != 0)
		{
			if ((piece & setTurnPiece) == (d % 2) * setTurnPiece)
				continue;
		}
		/*   0
		   3 + 1
			 2   */
		const int i = x + ((d & 0b01) * (1 - (d & 0b10)));
		const int j = y + ((1 - (d & 0b01)) * ((d & 0b10) - 1));
		/*int i = x;
		int j = y;
		switch (d)
		{
		case 0:
			j--;
			break;
		case 1:
			i++;
			break;
		case 2:
			j++;
			break;
		case 3:
			i--;
			break;
		}*/
		const uint8_t dest = currentSquares[i][j];

		if (i < 0 || i > 12 || j < 0 || j > 12)
			continue; // Bounds
		if (i % 2 + j % 2 == 2)
			continue; // Holes
		if (visited[i][j])
			continue; // Revisiting check
		// Check the dest turn piece
		if ((dest & hasTurnPiece) != 0 && (((dest & setTurnPiece)) == (d % 2) * setTurnPiece))
			continue;

		const uint8_t prevStartSquare = currentSquares[x][y];
		const uint8_t prevDestSquare = currentSquares[i][j];

		// Most basic case : empty target square
		if ((dest & 0b00111111) == 0)
		{
			for (int splitOff = 1; splitOff <= Piece::height(piece); splitOff++)
			{
				// splittoff is number of moved pieces

				if (splitOff == Piece::height(piece))
				{
					currentSquares[x][y] &= 0b11000000;
				}
				else
				{
					currentSquares[x][y] -= splitOff;
					if (Piece::hasAddOn(piece) && splitOff == Piece::height(piece) - 1)
					{
						currentSquares[x][y] &= 0b11011111; // erase trailing color
					}
				}
				currentSquares[i][j] |= (uint8_t)((piece) & 0b00100000 | splitOff);
				if (splitOff == Piece::height(piece))
				{
					currentSquares[i][j] |= (uint8_t)((piece) & 0b00011000); // addons, only if  we're moving the whole tower
				}

				const bool leavingAddon = leftBehindAddon || Piece::isAddOn(currentSquares[x][y]);
				const bool newLeftRed = leftRed || (leavingAddon && Piece::isRed(currentSquares[x][y]));

				visited[i][j] = true;

				basicGenerator(i, j, startX, startY, remainingSteps - 1, false, isWhite, onlyClaimsAndCaptures, leavingAddon, newLeftRed);

				visited[i][j] = false;
				currentSquares[x][y] = prevStartSquare;
				currentSquares[i][j] = prevDestSquare;
			}
		}
		else
		{
			// Merging case
			if (Piece::height(dest) > 0 && (Piece::isAddOn(dest) ? true : (Piece::colour(dest) == Piece::colour(piece))))
			{
				for (int splitOff = 1; (splitOff <= Piece::height(piece)) && (splitOff <= 5 - Piece::height(dest)); splitOff++)
				{
					if (splitOff == Piece::height(piece) && Piece::hasAddOn(piece))
						continue;

					const uint8_t origPiece = origSquares[startX][startY];

					const bool leavingAddon = leftBehindAddon || (Piece::hasAddOn(piece) && splitOff == Piece::height(piece) - 1);
					const bool newLeftRed = leftRed || (leavingAddon && Piece::isRed(piece));
					// The claim is favorable if we don't lose an AddOn,
					// or if we leave a blue one in favor of a red one
					const bool isNonQuietClaim = Piece::isAddOn(dest) && (!leavingAddon || (Piece::isRed(dest) && !newLeftRed));

					if (splitOff == Piece::height(piece))
					{
						currentSquares[x][y] &= 0b11000000;
					}
					else
					{
						currentSquares[x][y] -= splitOff;
						if (Piece::hasAddOn(piece) && splitOff == Piece::height(piece) - 1)
						{
							currentSquares[x][y] &= 0b11011111; // erase trailing color
						}
					}
					currentSquares[i][j] |= (uint8_t)((piece) & 0b00100000); // color info
					currentSquares[i][j] += splitOff;
					if (splitOff == Piece::height(piece))
					{
						currentSquares[i][j] |= (uint8_t)((piece) & 0b00011000); // addons, only if  we're moving the whole tower
					}

					visited[i][j] = true;

					basicGenerator(i, j, startX, startY, 0, false, isWhite, onlyClaimsAndCaptures && !isNonQuietClaim, leavingAddon, false);

					visited[i][j] = false;
					currentSquares[x][y] = prevStartSquare;
					currentSquares[i][j] = prevDestSquare;
				}
			}

			// Pushing Case
			if (Piece::height(dest) > 0 && Piece::height(dest) <= Piece::height(piece) && (Piece::isAddOn(dest) ? false : (Piece::colour(dest) == Piece::colour(piece))))
			{
				// Checked for (smaller than the pushing piece, not an addon and the same color)
				int offset = 2;
				bool pushValid = false;
				int pushHeight = Piece::height(dest);

				int xDir = 0;
				int yDir = 0;
				switch (d)
				{
				case 0:
					yDir = -1;
					break;
				case 1:
					xDir = 1;
					break;
				case 2:
					yDir = 1;
					break;
				case 3:
					xDir = -1;
					break;
				}

				/*
					Continue checking in the direction of movement until pushing is either posible or it isn't
					Offset is the offset from the original piece
					We loop until we hit a wall or something we can't push (hopefully)
				*/

				while (true)
				{
					int pushX = x + xDir * offset;
					int pushY = y + yDir * offset;

					if (pushX < 0 || pushX > 12 || pushY < 0 || pushY > 12)
						break;

					uint8_t pushPiece = (currentSquares[pushX][pushY]);

					if ((pushPiece & hasTurnPiece) != 0)
					{ // Obey turn pieces
						if (((pushPiece & setTurnPiece)) == (d % 2) * setTurnPiece)
							break; // This feels deeply cursed
					}
					if (Piece::height(pushPiece) == 0)
					{
						pushValid = true;
						break;
					}
					if (Piece::height(pushPiece) <= pushHeight && (Piece::isAddOn(pushPiece) ? false : (Piece::colour(pushPiece) == Piece::colour(piece))))
					{
						offset++;
						pushHeight = Piece::height(pushPiece);
					}
					else
					{
						break;
					}
				}
				if (!pushValid)
					continue;

				GamePosition boardCopy = currentSquares;

				for (int o = offset; o > 0; o--)
				{ // loop backwards from the end of the push chain and copy the pieces over
					int destX = x + xDir * o;
					int destY = y + yDir * o;
					int sourceX = x + xDir * (o - 1);
					int sourceY = y + yDir * (o - 1);

					currentSquares[destX][destY] |= currentSquares[sourceX][sourceY] & 0b00111111;
					currentSquares[sourceX][sourceY] &= 0b11000000;
				}

				visited[i][j] = true;

				basicGenerator(i, j, startX, startY, remainingSteps - 1, false, isWhite, onlyClaimsAndCaptures, false, false);

				visited[i][j] = false;

				// copy changes back
				currentSquares = boardCopy;
			}

			// Capturing case
			if (remainingSteps >= 2 && Piece::isTower(dest) && Piece::colour(dest) != Piece::colour(piece))
			{
				// making sure we have enough moves left to be able to capture
				if (!Piece::isBlue(piece) && Piece::height(dest) > Piece::height(piece))
					continue; // capturing height check

				// Logic: figure out all ways to capture and then generate all splitting options at the end to not redo the work over and over again (this might be something good to cache when trying to optimize)

				currentSquares[i][j] &= 0b11000000; // remove piece we're capturing

				visited[i][j] = true;

				captureGenerator(x, y, i, j, startX, startY, remainingSteps - 1, false, isWhite);

				visited[i][j] = false;
				currentSquares[i][j] = prevDestSquare;
			}
		}
	}
}

void MoveGenerator::captureGenerator(int originX, int originY, int x, int y, const int startX, const int startY, int remainingSteps, bool turned, bool isWhite)
{
	/*
		0
	   3 1
		2
	*/
	const uint8_t origin = currentSquares[originX][originY];

	// Turn in place if we can and haven't yet
	if (!turned && ((currentSquares[x][y] & hasTurnPiece) != 0))
	{

		currentSquares[x][y] ^= setTurnPiece;

		captureGenerator(originX, originY, x, y, startX, startY, remainingSteps, true, isWhite);

		currentSquares[x][y] ^= setTurnPiece;
	}

	// Loop through the 4 possible directions
	for (int d = 0; d < 4; d++)
	{
		const uint8_t piece = currentSquares[x][y];
		// Obey turn pieces
		if ((piece & hasTurnPiece) != 0)
		{
			if (((piece & setTurnPiece)) == (d % 2) * setTurnPiece)
				continue;
		}
		/*   0
		   3 + 1
			 2   */
		const int i = x + ((d & 0b01) * (1 - (d & 0b10)));
		const int j = y + ((1 - (d & 0b01)) * ((d & 0b10) - 1));

		const uint8_t dest = currentSquares[i][j];
		if (i < 0 || i > 12 || j < 0 || j > 12)
			continue;
		if (i % 2 == 1 && j % 2 == 1)
			continue;
		if (visited[i][j])
			continue; // Bounds and revisiting check
		if ((dest & hasTurnPiece) != 0 && (((dest & setTurnPiece)) == (d % 2) * setTurnPiece))
			continue;

		// piece is the position where an enemy piece used to be that we are currently taking, this value has already been wiped
		// dest is the piece we are now looking to move to, either to complete the capture or to continue the chain
		// the original piece doing the capturing still exists at originX, originY in the state array
		// splitting will be done once the end of a capture chain has been found

		// ending the chain on an empty square or a friendly tower or a single addon to merge onto
		if (Piece::height(dest) == 0 || (Piece::isTower(dest) && Piece::colour(dest) == Piece::colour(origin)) || Piece::isAddOn(dest))
		{

			// We have to split less than the full tower, except if the height is 1 and we can't go over the merge limit
			for (int splitOff = 1; (Piece::height(origin) == 1 ? splitOff <= 1 : (splitOff < Piece::height(origin))) && (splitOff <= 5 - Piece::height(dest)); splitOff++)
			{

				if (Piece::height(origin) == 1)
				{
					currentSquares[originX][originY] &= 0b11000000; // wipe the orign only if the height is one
				}
				else
				{
					currentSquares[originX][originY] -= splitOff;
					if (Piece::isAddOn(currentSquares[originX][originY]))
						currentSquares[originX][originY] &= 0b11011111; // if we left a single addon, remove the color
				}

				currentSquares[i][j] += splitOff;
				currentSquares[i][j] |= Piece::colour(origin); // always copy color and add height as that should be allowed based on the only way this can be called

				visited[i][j] = true;

				// do last move
				basicGenerator(i, j, startX, startY, 0 /*steps*/, false, isWhite, false, false, false);

				visited[i][j] = false;
				currentSquares[originX][originY] = origin;
				currentSquares[i][j] = dest;
			}
		}
		else if (remainingSteps + Piece::isBlue(origin) > 1 && Piece::isTower(dest) && Piece::colour(origin) != Piece::colour(dest))
		{
			if (!Piece::isBlue(origin) && Piece::height(dest) > Piece::height(origin))
				continue; // capturing height check

			currentSquares[i][j] &= 0b11000000; // remove piece we're capturing

			visited[i][j] = true;

			captureGenerator(
				originX, originY,
				i, j,
				startX, startY,
				remainingSteps - !Piece::isBlue(origin), // only decrement moves if the capturing piece doesn't have a blue addon
				false, isWhite);

			visited[i][j] = false;
			currentSquares[i][j] = dest;
		}
	}
}
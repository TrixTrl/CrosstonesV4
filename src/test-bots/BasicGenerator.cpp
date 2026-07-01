#include "BasicGenerator.h"
#include "globals/Piece.h"

std::shared_ptr<std::vector<std::vector<BasicGenerator::xMove>>> BasicGenerator::getMoves(const bool isWhite, const uint8_t (*pieces)[13][13]) {
	std::shared_ptr<std::vector<std::vector<xMove>>> moves = std::make_shared<std::vector<std::vector<xMove>>>();
	moves->reserve(700);

	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 13; j++) {
			uint8_t piece = (*pieces)[i][j];
			if (Piece::height(piece) == 0 || Piece::isAddOn(piece)) continue;
			if (Piece::isWhiteTower(piece) == !isWhite) continue;

			uint8_t boardCopy[13][13];
			std::memcpy(&boardCopy, pieces, sizeof((*pieces)));
			bool visited[13][13];
			memset(visited, false, sizeof(visited));
			visited[i][j] = true;
			basicGenerator(moves, &boardCopy, i, j, &visited, Piece::maxSteps(piece), false, isWhite, pieces);
		}
	}

	return moves;
}

void BasicGenerator::basicGenerator(std::shared_ptr<std::vector<std::vector<xMove>>> moves, uint8_t(*state)[13][13], int x, int y, bool(*visited)[13][13], int remainingSteps, bool turned, bool isWhite, const uint8_t(*pieces)[13][13])
{
	std::vector<xMove> move = std::vector<xMove>();
	move.reserve(10);
	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 13; j++) {
			if ((*pieces)[i][j] == (*state)[i][j]) continue;
			move.emplace_back(xMove{ i, j, ((uint8_t)(((*pieces)[i][j]) ^ ((*state)[i][j]))) });		//Creation of xor lists for moves
		}
	}
	if (moves->size() == 0 || move.size() > 0) {
		moves->emplace_back(move);
	}

	if (!turned && (((*state)[x][y] & hasTurnPiece) != 0)) {		//Turn in place if we can and haven't yet
		uint8_t boardCopy[13][13];
		std::memcpy(&boardCopy, state, sizeof(boardCopy));
		boardCopy[x][y] ^= setTurnPiece;
		bool visitedCopy[13][13];
		std::memcpy(&visitedCopy, visited, sizeof(visitedCopy));
		basicGenerator(moves, &boardCopy, x, y, &visitedCopy, remainingSteps, true, isWhite, pieces);
	}

	if (remainingSteps == 0) return;


	/*
		0
	   3 1
		2
	*/
	for (int d = 0; d < 4; d++) {		//Loop through the 4 possible directions
		uint8_t piece = ((*state)[x][y]);
		if ((piece & hasTurnPiece) != 0) {		//Obey turn pieces
			if (((piece & setTurnPiece)) == (d % 2) * setTurnPiece) continue;		//This feels deeply cursed
		}
		int i = x;
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
		}
		uint8_t dest = (*state)[i][j];
		if (i < 0 || i > 12 || j < 0 || j > 12) continue;
		if (i % 2 == 1 && j % 2 == 1) continue;
		if ((*visited)[i][j]) continue;		//Bounds and revisiting check
		if ((dest & hasTurnPiece) != 0 && (((dest & setTurnPiece)) == (d % 2) * setTurnPiece)) continue;

		if ((dest & 0b00111111) == 0) {		//Most basic case : empty target square
			for (int splitOff = 1; splitOff <= Piece::height(piece); splitOff++) {		//number of moved pieces
				uint8_t boardCopy[13][13];
				std::memcpy(&boardCopy, state, sizeof(boardCopy));
				if (splitOff == Piece::height(piece)) {
					boardCopy[x][y] &= 0b11000000;
				}
				else {
					boardCopy[x][y] -= splitOff;
					if (Piece::hasAddOn(piece) && splitOff == Piece::height(piece) - 1) {
						boardCopy[x][y] &= 0b11011111;		//erase trailing color
					}
				}
				boardCopy[i][j] |= (uint8_t)((piece) & 0b00100000 | splitOff);
				if (splitOff == Piece::height(piece)) {
					boardCopy[i][j] |= (uint8_t)((piece) & 0b00011000);		//addons, only if  we're moving the whole tower
				}
				bool visitedCopy[13][13];
				std::memcpy(&visitedCopy, visited, sizeof(visitedCopy));
				visitedCopy[i][j] = true;
				basicGenerator(moves, &boardCopy, i, j, &visitedCopy, remainingSteps - 1, false, isWhite, pieces);
			}
		}
		else if (Piece::height(dest) > 0 && (Piece::isAddOn(dest) ? true : (Piece::colour(dest) == Piece::colour(piece)))) {		//merging
			for (int splitOff = 1; (splitOff <= Piece::height(piece)) && (splitOff <= 5 - Piece::height(dest)); splitOff++) {
				if (splitOff == Piece::height(piece) && Piece::hasAddOn(piece)) continue;
				uint8_t boardCopy[13][13];
				std::memcpy(&boardCopy, state, sizeof(boardCopy));
				if (splitOff == Piece::height(piece)) {
					boardCopy[x][y] &= 0b11000000;
				}
				else {
					boardCopy[x][y] -= splitOff;
					if (Piece::hasAddOn(piece) && splitOff == Piece::height(piece) - 1) {
						boardCopy[x][y] &= 0b11011111;		//erase trailing color
					}
				}
				boardCopy[i][j] |= (uint8_t)((piece) & 0b00100000);		//color info
				boardCopy[i][j] += splitOff;
				if (splitOff == Piece::height(piece)) {
					boardCopy[i][j] |= (uint8_t)((piece) & 0b00011000);		//addons, only if  we're moving the whole tower
				}
				bool visitedCopy[13][13];
				std::memcpy(&visitedCopy, visited, sizeof(visitedCopy));
				visitedCopy[i][j] = true;
				basicGenerator(moves, &boardCopy, i, j, &visitedCopy, 0, false, isWhite, pieces);
			}
		}

		if (Piece::height(dest) > 0 && Piece::height(dest) <= Piece::height(piece) && (Piece::isAddOn(dest) ? false : (Piece::colour(dest) == Piece::colour(piece)))) {		//pushing (smaller than the pushing piece, not an addon and the same color)
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

			while (true) {
				int pushX = x + xDir * offset;
				int pushY = y + yDir * offset;

				if (pushX < 0 || pushX > 12 || pushY < 0 || pushY > 12) break;

				uint8_t pushPiece = ((*state)[pushX][pushY]);

				if ((pushPiece & hasTurnPiece) != 0) {		//Obey turn pieces
					if (((pushPiece & setTurnPiece)) == (d % 2) * setTurnPiece) break;		//This feels deeply cursed
				}
				if (Piece::height(pushPiece) == 0) {
					pushValid = true;
					break;
				}
				if (Piece::height(pushPiece) <= pushHeight && (Piece::isAddOn(pushPiece) ? false : (Piece::colour(pushPiece) == Piece::colour(piece)))) {
					offset++;
					pushHeight = Piece::height(pushPiece);
				}
				else { break; }
			}
			if (!pushValid) continue;
			uint8_t boardCopy[13][13];
			std::memcpy(&boardCopy, state, sizeof(boardCopy));

			for (int o = offset; o > 0; o--) {	//loop backwards from the end of the push chain and copy the pieces over
				int destX = x + xDir * o;
				int destY = y + yDir * o;
				int sourceX = x + xDir * (o - 1);
				int sourceY = y + yDir * (o - 1);

				boardCopy[destX][destY] |= boardCopy[sourceX][sourceY] & 0b00111111;
				boardCopy[sourceX][sourceY] &= 0b11000000;
			}

			bool visitedCopy[13][13];
			std::memcpy(&visitedCopy, visited, sizeof(visitedCopy));
			visitedCopy[i][j] = true;
			basicGenerator(moves, &boardCopy, i, j, &visitedCopy, remainingSteps - 1, false, isWhite, pieces);
		}

		if (remainingSteps >= 2 && Piece::isTower(dest) && Piece::colour(dest) != Piece::colour(piece)) {		//capturing
																												//making sure we have enough moves left to be able to capture
			if (!Piece::isBlue(piece) && Piece::height(dest) > Piece::height(piece)) continue;		//capturing height check

			//Logic: figure out all ways to capture and then generate all splitting options at the end to not redo the work over and over again (this might be something good to cache when trying to optimize)

			uint8_t boardCopy[13][13];
			std::memcpy(&boardCopy, state, sizeof(boardCopy));
			boardCopy[i][j] &= 0b11000000; //remove piece we're capturing
			bool visitedCopy[13][13];
			std::memcpy(&visitedCopy, visited, sizeof(visitedCopy));
			visitedCopy[i][j] = true;
			captureGenerator(moves, &boardCopy, x, y, i, j, &visitedCopy, remainingSteps - 1, false, isWhite, pieces);
		}
	}
}


void BasicGenerator::captureGenerator(std::shared_ptr<std::vector<std::vector<xMove>>> moves, uint8_t(*state)[13][13], int originX, int originY, int x, int y, bool(*visited)[13][13], int remainingSteps, bool turned, bool isWhite, const uint8_t(*pieces)[13][13])
{
	/*
		0
	   3 1
		2
	*/
	uint8_t origin = ((*state)[originX][originY]);

	if (!turned && (((*state)[x][y] & hasTurnPiece) != 0)) {		//Turn in place if we can and haven't yet
		uint8_t boardCopy[13][13];
		std::memcpy(&boardCopy, state, sizeof(boardCopy));
		boardCopy[x][y] ^= setTurnPiece;
		bool visitedCopy[13][13];
		std::memcpy(&visitedCopy, visited, sizeof(visitedCopy));
		captureGenerator(moves, &boardCopy, originX, originY, x, y, &visitedCopy, remainingSteps, true, isWhite, pieces);
	}

	for (int d = 0; d < 4; d++) {		//Loop through the 4 possible directions
		uint8_t piece = ((*state)[x][y]);
		if ((piece & hasTurnPiece) != 0) {		//Obey turn pieces
			if (((piece & setTurnPiece)) == (d % 2) * setTurnPiece) continue;		//This feels deeply cursed
		}
		int i = x;
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
		}
		uint8_t dest = (*state)[i][j];
		if (i < 0 || i > 12 || j < 0 || j > 12) continue;
		if (i % 2 == 1 && j % 2 == 1) continue;
		if ((*visited)[i][j]) continue;		//Bounds and revisiting check
		if ((dest & hasTurnPiece) != 0 && (((dest & setTurnPiece)) == (d % 2) * setTurnPiece)) continue;

		//piece is the position where an enemy piece used to be that we are currently taking, this value has already been wiped
		//dest is the piece we are now looking to move to, either to complete the capture or to continue the chain
		//the original piece doing the capturing still exists at originX, originY in the state array
		//splitting will be done once the end of a capture chain has been found


		//ending the chain on an empty square or a friendly tower or a single addon to merge onto
		if (Piece::height(dest) == 0 || (Piece::isTower(dest) && Piece::colour(dest) == Piece::colour(origin)) || Piece::isAddOn(dest)) {

			//We have to split less than the full tower, except if the height is 1 and we can't go over the merge limit
			for (int splitOff = 1; (Piece::height(origin) == 1 ? splitOff <= 1 : (splitOff < Piece::height(origin))) && (splitOff <= 5 - Piece::height(dest)); splitOff++) {

				uint8_t boardCopy[13][13];
				std::memcpy(&boardCopy, state, sizeof(boardCopy));
				if (Piece::height(origin) == 1) {
					boardCopy[originX][originY] &= 0b11000000;		//wipe the orign only if the height is one
				}
				else {
					boardCopy[originX][originY] -= splitOff;
					if (Piece::isAddOn(boardCopy[originX][originY])) boardCopy[originX][originY] &= 0b11011111;		//if we left a single addon, remove the color
				}

				boardCopy[i][j] += splitOff;
				boardCopy[i][j] |= Piece::colour(origin);		//always copy color and add height as that should be allowed based on the only way this can be called

				bool visitedCopy[13][13];
				std::memcpy(&visitedCopy, visited, sizeof(visitedCopy));
				visitedCopy[i][j] = true;
				basicGenerator(moves, &boardCopy, i, j, &visitedCopy, 0, false, isWhite, pieces); //do last move
			}
		}
		else if (remainingSteps + Piece::isBlue(origin) > 1 && Piece::isTower(dest) && Piece::colour(origin) != Piece::colour(dest)) {
			if (!Piece::isBlue(origin) && Piece::height(dest) > Piece::height(origin)) continue;		//capturing height check
			uint8_t boardCopy[13][13];
			std::memcpy(&boardCopy, state, sizeof(boardCopy));
			boardCopy[i][j] &= 0b11000000; //remove piece we're capturing
			bool visitedCopy[13][13];
			std::memcpy(&visitedCopy, visited, sizeof(visitedCopy));
			visitedCopy[i][j] = true;
			captureGenerator(moves, &boardCopy, originX, originY, i, j, &visitedCopy, remainingSteps - !Piece::isBlue(origin), false, isWhite, pieces);		//only decrement moves if the capturing piece doesn't have a blue addon
		}
	}
}
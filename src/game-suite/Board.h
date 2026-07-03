#pragma once

#include <cstdint>
#include <bitset>
#include <vector>
#include <memory>
#include "GamePosition.h"

class Board
{
public:
	Board();
	void rst(std::bitset<3>& tps);
	void wipe();

	struct xMove {
		int i;
		int j;
		uint8_t delta;
	};
	struct VisitedMap {
		bool tiles[13][13] = {false};

		bool (&operator[](size_t x))[13] {
        	return tiles[x];
		}
		const bool (&operator[](size_t x) const)[13] {
			return tiles[x];
		}
	};

	void copyPositionTo(GamePosition& dest) const;
	void copyPositionTo(uint8_t(*dest)[13][13]) const;
	GamePosition positionCopy() const;
	std::string dumpPosition() const;
	void loadPosition(std::string str);

	std::vector<std::vector<xMove>> getMoves(bool isWhite) const;
	void basicGenerator(
		std::vector<std::vector<xMove>>& moves, 
		const GamePosition& state, 
		int x, int y, 
		VisitedMap& visited, int remainingSteps, bool turned, bool isWhite
	) const;
	void captureGenerator(
		std::vector<std::vector<xMove>>& moves, 
		const GamePosition& state, 
		int originX, int originY, int x, int y, 
		VisitedMap& visited, int remainingSteps, bool turned, bool isWhite
	) const;

	static void unsafeApplyMove(GamePosition& position, const std::vector<xMove>& move);
	void unsafeMakeMove(const std::vector<xMove>& move);
	int makeMove(const std::vector<xMove>& move, bool isWhiteTurn);	//return -1 for invalid, 0 for passing, and 1 for any other valid move
	int makeMove(GamePosition& newState, bool isWhiteTurn);

	enum class winValue {
		none,
		white,
		black,
		draw
	};

	winValue gameOver(bool isWhite);

	static const uint8_t hasTurnPiece = 128;
	static const uint8_t setTurnPiece = 64;
	static constexpr int dx[4] = { 0, 1, 0, -1 };
	static constexpr int dy[4] = {-1, 0, 1,  0 };

	std::vector<std::string> gameRecord;
	void dumpGame();
private:
	GamePosition gamePos;
};

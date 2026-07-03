#pragma once

#include <vector>
#include <string>

#include "../Globals/Piece.h"
#include "Utils.h"

class FastMoveGenerator
{
public:
    enum Direction
    {
        NONE,
        NORTH,
        SOUTH,
        EAST,
        WEST
    };

    struct moveFragment
    {
        Direction dir;
        int splitAmount;
    };

    struct move
    {
        std::pair<int, int> start;
        std::vector<moveFragment> moveFragments;
        bool isMerge = false;
        bool isTurn = false;
    };

    struct moveGenerationState
    {
        std::pair<int, int> position;
        Direction dir;
        int movingAmount;
        int depth;
        int stepsLeft;
        bool capturing = false;
    };

    static std::vector<move> getMoves(uint8_t (*pieces)[13][13], bool isWhite);
    static void generateMoves(uint8_t (*pieces)[13][13], bool isWhite, std::pair<int, int> start, std::vector<move> *moves);
    static std::vector<Direction> getLegalMoveDirections(uint8_t (*pieces)[13][13], moveGenerationState currentState);
    static void applyMove(uint8_t (*pieces)[13][13], const move &move);
    static std::pair<uint8_t, uint8_t> getSplitResult(uint8_t piece, int movingHeight);
    static std::pair<int, int> getPositionInDirection(std::pair<int, int> position, Direction direction);
    static uint8_t mergeTowers(uint8_t staticTower, uint8_t mergingTower);

    static void printMove(move);
};
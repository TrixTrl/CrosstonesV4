#include "FastMoveGenerator.h"

#define FAST_MOVE_DEBUG_PRINTING

std::vector<FastMoveGenerator::move> FastMoveGenerator::getMoves(uint8_t (*pieces)[13][13], bool isWhite)
{
    std::vector<move> moves = std::vector<move>();
    moves.reserve(700);

    for (int x = 0; x < 13; x++)
    {
        for (int y = 0; y < 13; y++)
        {
            uint8_t piece = *pieces[x][y];
            if (!Piece::isTower(piece))
                continue;
            if (Piece::isWhite(piece) != isWhite)
                continue;
        }
    }

    return moves;
}

void FastMoveGenerator::generateMoves(uint8_t (*pieces)[13][13], bool isWhite, std::pair<int, int> start, std::vector<move> &moves)
{
    std::vector<moveGenerationState> stack = std::vector<moveGenerationState>();
    stack.reserve(30);

    moveGenerationState startingState = moveGenerationState();
    startingState.position = start;
    startingState.dir = Direction::NONE;
    startingState.depth = 0;
    startingState.movingAmount = Piece::height(*pieces[start.first][start.second]);

    std::vector<moveFragment> inProgressMoveFragments = std::vector<moveFragment>();
    inProgressMoveFragments.reserve(10);

    stack.emplace_back(startingState);

    while (stack.size() > 0)
    {
        moveGenerationState currentState = stack.at(stack.size() - 1);
        stack.erase(stack.end() - 1);

#ifdef FAST_MOVE_DEBUG_PRINTING
        Utils::print("(" + std::to_string(currentState.position.first) + ", " + std::to_string(currentState.position.second) + ") ", false);
        switch (currentState.dir)
        {
        case Direction::NONE:
            Utils::print("moving nowhere ", false);
            break;
        case Direction::NORTH:
            Utils::print("moving north ", false);
            break;
        case Direction::SOUTH:
            Utils::print("moving south ", false);
            break;
        case Direction::EAST:
            Utils::print("moving east ", false);
            break;
        case Direction::WEST:
            Utils::print("moving west ", false);
            break;
        }
        Utils::print(std::to_string(currentState.movingAmount) + " ", false);
        Utils::print(std::to_string(currentState.depth), true);
#endif

        while (currentState.depth > inProgressMoveFragments.size())
            inProgressMoveFragments.erase(inProgressMoveFragments.end() - 1);

        int prevMovingAmount = Piece::height(*pieces[start.first][start.second]);
        for (const auto &fragment : inProgressMoveFragments)
        {
            prevMovingAmount -= fragment.splitAmount;
        }

        moveFragment currentFragment = moveFragment();
        currentFragment.dir = currentState.dir;
        currentFragment.splitAmount = prevMovingAmount - currentState.movingAmount;

        inProgressMoveFragments.emplace_back(currentFragment);

        uint8_t pieceAtCurrentLocation = *pieces[currentState.position.first][currentState.position.second];
        if (!Piece::isTower(pieceAtCurrentLocation) || Piece::isWhite(pieceAtCurrentLocation) == isWhite) // Check if it is legal to end a move in this position, capture generation making this false
        {
            move currentMove = move();
            currentMove.start = start;
            currentMove.moveFragments = std::vector<moveFragment>();
            for (const auto &fragment : inProgressMoveFragments)
            {
                currentMove.moveFragments.emplace_back(fragment);
            }
            moves.emplace_back(currentMove);
        }

        bool isComplexMove = false;

        std::pair<int, int> moveCheckingPosition = std::pair<int, int>(start.first, start.second);
        for (const auto &fragment : inProgressMoveFragments)
        {
            switch (fragment.dir)
            {
            case Direction::NORTH:
                moveCheckingPosition.second--;
                break;
            case Direction::SOUTH:
                moveCheckingPosition.second++;
                break;
            case Direction::EAST:
                moveCheckingPosition.first++;
                break;
            case Direction::WEST:
                moveCheckingPosition.first--;
                break;
            }
            if ((Piece::towerMask & *pieces[moveCheckingPosition.first][moveCheckingPosition.second]) != 0)
            {
                isComplexMove = true;
                break;
            }
        }

        uint8_t (*currentBoardState)[13][13] = pieces;
        uint8_t complexMoveCopy[13][13];

        if (isComplexMove)
        {
            memcpy(&complexMoveCopy, pieces, sizeof(*pieces));
            move tempComplexMove = move();
            tempComplexMove.start = start;
            tempComplexMove.moveFragments = inProgressMoveFragments;
            applyMove(&complexMoveCopy, tempComplexMove);
            currentBoardState = &complexMoveCopy;
        }

        std::vector<Direction> potentialDirections = getLegalMoveDirections(currentBoardState, currentState);

        for (int i = 1; i <= currentState.movingAmount; i++)
        {
            for (const auto &direction : potentialDirections)
            {
                moveGenerationState nextState = moveGenerationState();
                nextState.position = getPositionInDirection(currentState.position, direction);
                nextState.dir = direction;
                nextState.depth = currentState.depth + 1;
                nextState.movingAmount = i;
            }
        }
    }
}

std::vector<FastMoveGenerator::Direction> FastMoveGenerator::getLegalMoveDirections(uint8_t (*pieces)[13][13], moveGenerationState currentState)
{
    std::vector<Direction> result = std::vector<Direction>();
    if (currentState.dir != Direction::SOUTH && currentState.position.second > 0 && currentState.position.first % 2 == 0)
    {
        uint8_t currentTurnPiece = Piece::turnPieceMask & *pieces[currentState.position.first][currentState.position.second];
        uint8_t destinationTurnPiece = Piece::turnPieceMask & *pieces[currentState.position.first][currentState.position.second - 1];
        if (((currentTurnPiece | destinationTurnPiece) == 0) ||
            (currentTurnPiece != 0 && (currentTurnPiece & Piece::setTurnPiece) != 0) || (destinationTurnPiece != 0 && (destinationTurnPiece & Piece::setTurnPiece) != 0))
        {
            result.emplace_back(Direction::NORTH);
        }
    }
    if (currentState.dir != Direction::NORTH && currentState.position.second < 12 && currentState.position.first % 2 == 0)
    {
        uint8_t currentTurnPiece = Piece::turnPieceMask & *pieces[currentState.position.first][currentState.position.second];
        uint8_t destinationTurnPiece = Piece::turnPieceMask & *pieces[currentState.position.first][currentState.position.second + 1];
        if (((currentTurnPiece | destinationTurnPiece) == 0) ||
            (currentTurnPiece != 0 && (currentTurnPiece & Piece::setTurnPiece) != 0) || (destinationTurnPiece != 0 && (destinationTurnPiece & Piece::setTurnPiece) != 0))
        {
            result.emplace_back(Direction::SOUTH);
        }
    }
    if (currentState.dir != Direction::WEST && currentState.position.first < 12 && currentState.position.second % 2 == 0)
    {
        uint8_t currentTurnPiece = Piece::turnPieceMask & *pieces[currentState.position.first][currentState.position.second];
        uint8_t destinationTurnPiece = Piece::turnPieceMask & *pieces[currentState.position.first + 1][currentState.position.second];
        if (((currentTurnPiece | destinationTurnPiece) == 0) ||
            (currentTurnPiece != 0 && (currentTurnPiece & Piece::setTurnPiece) == 0) || (destinationTurnPiece != 0 && (destinationTurnPiece & Piece::setTurnPiece) == 0))
        {
            result.emplace_back(Direction::EAST);
        }
    }
    if (currentState.dir != Direction::EAST && currentState.position.first > 0 && currentState.position.second % 2 == 0)
    {
        uint8_t currentTurnPiece = Piece::turnPieceMask & *pieces[currentState.position.first][currentState.position.second];
        uint8_t destinationTurnPiece = Piece::turnPieceMask & *pieces[currentState.position.first - 1][currentState.position.second];
        if (((currentTurnPiece | destinationTurnPiece) == 0) ||
            (currentTurnPiece != 0 && (currentTurnPiece & Piece::setTurnPiece) == 0) || (destinationTurnPiece != 0 && (destinationTurnPiece & Piece::setTurnPiece) == 0))
        {
            result.emplace_back(Direction::WEST);
        }
    }
    return result;
}

void FastMoveGenerator::applyMove(uint8_t (*pieces)[13][13], move move)
{
    uint8_t currentPiece = Piece::towerMask & *pieces[move.start.first][move.start.second];
    std::pair<int, int> currentPosition = std::pair<int, int>(move.start.first, move.start.second);

    *pieces[currentPosition.first][currentPosition.second] &= Piece::turnPieceMask;

    for (const auto &fragment : move.moveFragments)
    {
        std::pair<int, int> fragmentDelta = std::pair<int, int>(0, 0);
        switch (fragment.dir)
        {
        case Direction::NORTH:
            fragmentDelta.second--;
            break;
        case Direction::SOUTH:
            fragmentDelta.second++;
            break;
        case Direction::EAST:
            fragmentDelta.first++;
            break;
        case Direction::WEST:
            fragmentDelta.first--;
            break;
        }
        uint8_t targetPiece = *pieces[currentPosition.first + fragmentDelta.first][currentPosition.second + fragmentDelta.second];
        if (Piece::isTower(targetPiece) && (currentPiece & Piece::colourMask == targetPiece & Piece::colourMask))
        {
            // pushing
        }
        std::pair<uint8_t, uint8_t> splitResult = getSplitResult(currentPiece, Piece::height(currentPiece) - fragment.splitAmount);
        currentPosition.first += fragmentDelta.first;
        currentPosition.second += fragmentDelta.second;
        *pieces[currentPosition.first][currentPosition.second] |= splitResult.first;
        currentPiece = splitResult.second;
    }

    *pieces[currentPosition.first][currentPosition.second] |= currentPiece;
}

std::pair<uint8_t, uint8_t> FastMoveGenerator::getSplitResult(uint8_t piece, int movingHeight)
{
    if (movingHeight == Piece::height(piece))
        return std::pair<uint8_t, uint8_t>(0, piece);
    piece -= movingHeight;
    return std::pair<uint8_t, uint8_t>(piece, (piece & Piece::colourMask) + movingHeight);
}

std::pair<int, int> FastMoveGenerator::getPositionInDirection(std::pair<int, int> position, Direction direction)
{
    switch (direction)
    {
    case Direction::NORTH:
        return std::pair<int, int>(position.first, position.second - 1);
        break;
    case Direction::SOUTH:
        return std::pair<int, int>(position.first, position.second + 1);
        break;
    case Direction::EAST:
        return std::pair<int, int>(position.first + 1, position.second);
        break;
    case Direction::WEST:
        return std::pair<int, int>(position.first - 1, position.second);
        break;
    }
}
#include "FastMoveGenerator.h"

// #define FAST_MOVE_DEBUG_PRINTING

std::vector<FastMoveGenerator::move> FastMoveGenerator::getMoves(const uint8_t (*pieces)[13][13], bool isWhite)
{
    std::vector<move> moves = std::vector<move>();
    moves.reserve(400);

    for (int x = 0; x < 13; x++)
    {
        for (int y = 0; y < 13; y++)
        {
            uint8_t piece = (*pieces)[x][y];
            if (!Piece::isTower(piece))
                continue;
            if (Piece::isWhite(piece) != isWhite)
                continue;
            // Utils::print(std::to_string(piece), true);
            // Utils::print(std::to_string(x) + " , " + std::to_string(y), true);
            // Utils::print(std::to_string(Piece::height(piece)), true);
            generateMoves(pieces, isWhite, std::pair<int, int>(x, y), &moves);
        }
    }

    return moves;
}

void FastMoveGenerator::generateMoves(const uint8_t (*pieces)[13][13], bool isWhite, std::pair<int, int> start, std::vector<move> *moves)
{
    std::vector<moveGenerationState> stack = std::vector<moveGenerationState>();
    stack.reserve(30);

    moveGenerationState startingState = moveGenerationState();
    startingState.position = start;
    startingState.dir = Direction::NONE;
    startingState.depth = 0;
    startingState.movingAmount = Piece::height((*pieces)[start.first][start.second]);
    startingState.stepsLeft = Piece::maxSteps((*pieces)[start.first][start.second]);

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
            Utils::print("moving north   ", false);
            break;
        case Direction::SOUTH:
            Utils::print("moving south   ", false);
            break;
        case Direction::EAST:
            Utils::print("moving east    ", false);
            break;
        case Direction::WEST:
            Utils::print("moving west    ", false);
            break;
        }
        Utils::print("moving: " + std::to_string(currentState.movingAmount) + " ", false);
        Utils::print("steps: " + std::to_string(currentState.stepsLeft) + " ", false);
        Utils::print("depth: " + std::to_string(currentState.depth), false);
        Utils::print(currentState.capturing ? " capturing" : "", false);
        Utils::print(currentState.stepsLeft == -2 ? " merging" : "", false);
        Utils::print("", true);
#endif

        while (inProgressMoveFragments.size() > currentState.depth)
            inProgressMoveFragments.erase(inProgressMoveFragments.end() - 1);

        int prevMovingAmount = Piece::height((*pieces)[start.first][start.second]);
        // Utils::print(std::to_string(prevMovingAmount) + " ", false);
        for (const auto &fragment : inProgressMoveFragments)
        {
            prevMovingAmount -= fragment.splitAmount;
        }
        // Utils::print(std::to_string(prevMovingAmount), true);

        moveFragment currentFragment = moveFragment();
        currentFragment.dir = currentState.dir;
        currentFragment.splitAmount = prevMovingAmount - currentState.movingAmount;

        inProgressMoveFragments.emplace_back(currentFragment);

        bool isComplexMove = false;
        bool skippedFirstFragmentComplexCheck = false;

        std::pair<int, int> moveCheckingPosition = std::pair<int, int>(start.first, start.second);
        for (const auto &fragment : inProgressMoveFragments)
        {
            if (!skippedFirstFragmentComplexCheck)
            {
                skippedFirstFragmentComplexCheck = true;
                continue;
            }
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
            if ((Piece::towerMask & (*pieces)[moveCheckingPosition.first][moveCheckingPosition.second]) != 0)
            {
                isComplexMove = true;
                break;
            }
        }

        const uint8_t (*currentBoardState)[13][13] = pieces;
        uint8_t complexMoveCopy[13][13];

        if (isComplexMove)
        {
            memcpy(&complexMoveCopy, pieces, sizeof(*pieces));
            move tempComplexMove = move();
            tempComplexMove.start = std::pair<int, int>(start);
            tempComplexMove.moveFragments = std::vector<moveFragment>();
            for (const auto &fragment : inProgressMoveFragments)
            {
                tempComplexMove.moveFragments.emplace_back(fragment);
            }
            applyMove(&complexMoveCopy, tempComplexMove, true);
            currentBoardState = &complexMoveCopy;
        }

        uint8_t pieceAtCurrentLocation = (*currentBoardState)[currentState.position.first][currentState.position.second];
        if (!Piece::isTower(pieceAtCurrentLocation) || Piece::isWhite(pieceAtCurrentLocation) == isWhite) // Check if it is legal to end a move in this position, capture generation making this false
        {
            move currentMove = move();
            currentMove.start = start;
            currentMove.moveFragments = std::vector<moveFragment>();
            bool skippedFirstFragment = false;
            for (const auto &fragment : inProgressMoveFragments)
            {
                if (!skippedFirstFragment)
                {
                    skippedFirstFragment = true;
                    continue;
                }
                currentMove.moveFragments.emplace_back(fragment);
            }
            if (currentState.stepsLeft == -2)
            {
                currentMove.isMerge = true;
            }
            if (currentMove.moveFragments.size() > 0 || moves->size() == 0)
                moves->emplace_back(currentMove);

            if (Piece::turnPiece(pieceAtCurrentLocation))
            {
                move turnMove = move(currentMove);
                turnMove.isTurn = true;
                moves->emplace_back(turnMove);
            }

            if (currentState.capturing)
                continue;
        }

        if (currentState.stepsLeft == 0 || currentState.stepsLeft == -2)
            continue;

        std::vector<Direction> potentialDirections = getLegalMoveDirections(currentBoardState, currentState);

        uint8_t currentTower = getSplitResult((*pieces)[start.first][start.second], currentState.movingAmount).second;

        for (const auto &direction : potentialDirections)
        {
            std::pair<int, int> nextPosition = getPositionInDirection(currentState.position, direction);
            uint8_t nextPiece = (*currentBoardState)[nextPosition.first][nextPosition.second] & Piece::towerMask;

            // check if movement in that direction is impossible

            bool canMove = true;
            bool canCapture = false;
            bool pushing = false;

            if (Piece::isTower(nextPiece))
            {
                if (Piece::isWhite(nextPiece) == isWhite)
                {
                    if (Piece::height(nextPiece) <= Piece::height(currentTower))
                    {
                        std::pair<int, int> searchLocation = std::pair<int, int>(nextPosition);
                        uint8_t searchingPiece = (*currentBoardState)[searchLocation.first][searchLocation.second];
                        while (Piece::isTower(searchingPiece) && Piece::isWhite(searchingPiece) == isWhite && Piece::height(searchingPiece) <= Piece::height(searchingPiece) &&
                               (Piece::turnPiece(searchingPiece) == 0 || ((direction == Direction::NORTH || direction == Direction::SOUTH) ? Piece::turnPiece(searchingPiece) == Piece::turnPieceMask : Piece::turnPiece(searchingPiece) == Piece::hasTurnPiece)))
                        {
                            searchLocation = getPositionInDirection(searchLocation, direction);
                            if (!(searchLocation.first >= 0 && searchLocation.first <= 12 && searchLocation.second >= 0 && searchLocation.second <= 12))
                            {
                                break;
                            }
                            searchingPiece = (*currentBoardState)[searchLocation.first][searchLocation.second];
                        }
                        if (!(searchLocation.first >= 0 && searchLocation.first <= 12 && searchLocation.second >= 0 && searchLocation.second <= 12 && (searchingPiece & Piece::towerMask) == 0 &&
                              (Piece::turnPiece(searchingPiece) == 0 || ((direction == Direction::NORTH || direction == Direction::SOUTH) ? Piece::turnPiece(searchingPiece) == Piece::turnPieceMask : Piece::turnPiece(searchingPiece) == Piece::hasTurnPiece))))
                        {
                            canMove = false;
                        }
                        else
                        {
                            pushing = true;
                        }
                    }
                    else
                    {
                        canMove = false;
                    }
                }
                else
                {
                    canMove = false;
                    if (Piece::height(nextPiece) > Piece::height(currentTower) && Piece::isBlue(currentTower) == 0)
                    {
                        canCapture = false;
                    }
                    else
                    {
                        canCapture = true;
                    }
                }
            }

            for (int i = currentState.capturing ? currentState.movingAmount : 1; i <= currentState.movingAmount; i++)
            {
                if ((canMove || (canCapture && (i < currentState.movingAmount || currentState.movingAmount == 1 || currentState.capturing) && currentState.stepsLeft > 1)) && (!pushing || i == currentState.movingAmount))
                {
                    moveGenerationState nextState = moveGenerationState();
                    nextState.position = nextPosition;
                    nextState.dir = direction;
                    nextState.depth = currentState.depth + 1;
                    nextState.movingAmount = i;
                    nextState.stepsLeft = currentState.stepsLeft - 1;
                    nextState.capturing = currentState.capturing | canCapture;
                    stack.emplace_back(nextState);
                }
                if (Piece::isTower(nextPiece) && Piece::isWhite(nextPiece) == isWhite && (Piece::hasAddOn(currentTower) == 0 || i < currentState.movingAmount) && Piece::height(nextPiece) + i <= 5)
                {
                    moveGenerationState nextState = moveGenerationState();
                    nextState.position = nextPosition;
                    nextState.dir = direction;
                    nextState.depth = currentState.depth + 1;
                    nextState.movingAmount = i;
                    nextState.stepsLeft = -2;
                    nextState.capturing = currentState.capturing;
                    stack.emplace_back(nextState);
                }
            }
        }
    }
}

std::vector<FastMoveGenerator::Direction> FastMoveGenerator::getLegalMoveDirections(const uint8_t (*pieces)[13][13], moveGenerationState currentState)
{
    std::vector<Direction> result = std::vector<Direction>();
    if (currentState.dir != Direction::SOUTH && currentState.position.second > 0 && currentState.position.first % 2 == 0)
    {
        uint8_t destinationTurnPiece = Piece::turnPieceMask & (*pieces)[currentState.position.first][currentState.position.second - 1];
        if (destinationTurnPiece == 0 || (destinationTurnPiece & Piece::setTurnPiece) != 0)
        {
            result.emplace_back(Direction::NORTH);
        }
    }
    if (currentState.dir != Direction::NORTH && currentState.position.second < 12 && currentState.position.first % 2 == 0)
    {
        uint8_t destinationTurnPiece = Piece::turnPieceMask & (*pieces)[currentState.position.first][currentState.position.second + 1];
        if (destinationTurnPiece == 0 || (destinationTurnPiece & Piece::setTurnPiece) != 0)
        {
            result.emplace_back(Direction::SOUTH);
        }
    }
    if (currentState.dir != Direction::WEST && currentState.position.first < 12 && currentState.position.second % 2 == 0)
    {
        uint8_t destinationTurnPiece = Piece::turnPieceMask & (*pieces)[currentState.position.first + 1][currentState.position.second];
        if (destinationTurnPiece == 0 || (destinationTurnPiece & Piece::setTurnPiece) == 0)
        {
            result.emplace_back(Direction::EAST);
        }
    }
    if (currentState.dir != Direction::EAST && currentState.position.first > 0 && currentState.position.second % 2 == 0)
    {
        uint8_t destinationTurnPiece = Piece::turnPieceMask & (*pieces)[currentState.position.first - 1][currentState.position.second];
        if (destinationTurnPiece == 0 || (destinationTurnPiece & Piece::setTurnPiece) == 0)
        {
            result.emplace_back(Direction::WEST);
        }
    }

    return result;
}

void FastMoveGenerator::applyMove(uint8_t (*pieces)[13][13], const move &move, bool skipLastOverwrite)
{
    uint8_t currentPiece = Piece::towerMask & (*pieces)[move.start.first][move.start.second];
    std::pair<int, int> currentPosition = std::pair<int, int>(move.start);

    if (currentPosition.first < 0 || currentPosition.first > 12 || currentPosition.second < 0 || currentPosition.second > 12)
        throw;
    (*pieces)[currentPosition.first][currentPosition.second] &= Piece::turnPieceMask;

    int fragmentIndex = 0;
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
        if (Piece::turnPiece((*pieces)[currentPosition.first][currentPosition.second]))
        {
            if (fragmentDelta.second != 0)
            {
                (*pieces)[currentPosition.first][currentPosition.second] |= Piece::setTurnPiece;
            }
            else
            {
                (*pieces)[currentPosition.first][currentPosition.second] &= ~Piece::setTurnPiece;
            }
        }
        uint8_t targetPiece = (*pieces)[currentPosition.first + fragmentDelta.first][currentPosition.second + fragmentDelta.second] & Piece::towerMask;
        if (Piece::isTower(targetPiece) && ((currentPiece & Piece::colourMask) == (targetPiece & Piece::colourMask)) && (fragmentIndex < move.moveFragments.size() - 1 || !move.isMerge))
        {
            std::pair<int, int> pushingPosition = getPositionInDirection(currentPosition, fragment.dir);
            uint8_t pushingPiece = (*pieces)[pushingPosition.first][pushingPosition.second] & Piece::towerMask;
            uint8_t pushingDestination = (*pieces)[pushingPosition.first + fragmentDelta.first][pushingPosition.second + fragmentDelta.second] & Piece::towerMask;
            (*pieces)[pushingPosition.first][pushingPosition.second] &= Piece::turnPieceMask;
            do
            {
                pushingPosition = getPositionInDirection(pushingPosition, fragment.dir);
                (*pieces)[pushingPosition.first][pushingPosition.second] &= Piece::turnPieceMask;
                (*pieces)[pushingPosition.first][pushingPosition.second] |= pushingPiece;
                // pushingPosition = std::pair<int, int>(pushingPosition.first + fragmentDelta.first, pushingPosition.second + fragmentDelta.second);
                pushingPiece = pushingDestination;
                pushingDestination = (*pieces)[pushingPosition.first + fragmentDelta.first][pushingPosition.second + fragmentDelta.second] & Piece::towerMask;
            } while (pushingDestination != 0);
            (*pieces)[pushingPosition.first + fragmentDelta.first][pushingPosition.second + fragmentDelta.second] |= pushingPiece;
        }
        else if (fragmentIndex < move.moveFragments.size() - 1)
        {
            (*pieces)[currentPosition.first + fragmentDelta.first][currentPosition.second + fragmentDelta.second] &= Piece::turnPieceMask;
        }
        std::pair<uint8_t, uint8_t> splitResult = getSplitResult(currentPiece, Piece::height(currentPiece) - fragment.splitAmount);
        (*pieces)[currentPosition.first][currentPosition.second] |= splitResult.first;
        currentPiece = splitResult.second;

        currentPosition.first += fragmentDelta.first;
        currentPosition.second += fragmentDelta.second;
        if (currentPosition.first < 0 || currentPosition.first > 12 || currentPosition.second < 0 || currentPosition.second > 12)
            throw;
        fragmentIndex++;
    }
    if (currentPosition.first < 0 || currentPosition.first > 12 || currentPosition.second < 0 || currentPosition.second > 12)
        throw;
    uint8_t finalPiece = mergeTowers((*pieces)[currentPosition.first][currentPosition.second] & Piece::towerMask, currentPiece);
    if (skipLastOverwrite)
        return;
    (*pieces)[currentPosition.first][currentPosition.second] &= Piece::turnPieceMask;
    (*pieces)[currentPosition.first][currentPosition.second] |= finalPiece;
    if (move.isTurn)
    {
        (*pieces)[currentPosition.first][currentPosition.second] ^= Piece::setTurnPiece;
    }
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
    throw;
}

uint8_t FastMoveGenerator::mergeTowers(uint8_t staticTower, uint8_t mergingTower)
{
    if (staticTower == 0)
        return mergingTower;
    return ((staticTower & Piece::addOnMask) | (mergingTower & Piece::colourMask)) + Piece::height(staticTower) + Piece::height(mergingTower);
}

void FastMoveGenerator::printMove(FastMoveGenerator::move move)
{
    Utils::print("(" + std::to_string(move.start.first) + ", " + std::to_string(move.start.second) + "), ");
    std::pair<int, int> position = std::pair<int, int>(move.start);
    for (const auto &fragment : move.moveFragments)
    {
        position = getPositionInDirection(position, fragment.dir);
        Utils::print("(" + std::to_string(position.first) + ", " + std::to_string(position.second) + ")|" + std::to_string(fragment.splitAmount) + ", ");
    }
    Utils::print(move.isMerge ? " merging" : "");
    Utils::print(move.isTurn ? " turning" : "");
    Utils::print("", true);
}
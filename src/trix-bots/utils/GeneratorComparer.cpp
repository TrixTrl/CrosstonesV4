#include "GeneratorComparer.h"

void runPosition(uint8_t (*pieces)[13][13], bool isWhite)
{
    BoardState_T boardState = BoardState_T(pieces);

    Utils::print("Position: " + boardState.dumpPos(), true);

    std::shared_ptr<std::vector<std::vector<BoardState_T::xMove>>> correctMoves = boardState.getMoves(isWhite, false, false);
    std::vector<FastMoveGenerator::move> movesToCompare = FastMoveGenerator::getMoves(pieces, isWhite);

    int n = 0;
    int legalMoves = 0;
    int illegalMoves = 0;
    int duplicateMoves = 0;
    std::vector<int> foundMoves = std::vector<int>(correctMoves->size(), -1);
    for (const auto &move : movesToCompare)
    {
        uint8_t movePieces[13][13];
        memcpy(&movePieces, pieces, sizeof(*pieces));
        FastMoveGenerator::applyMove(&movePieces, move);
        BoardState_T comparisonBoardState = BoardState_T(pieces);
        int moveResult = comparisonBoardState.makeMove(&movePieces, isWhite);
        // Utils::print(moveResult, true);
        // Utils::print(n++, true);
        // Utils::print(move.moveFragments.size(), true);
        if (moveResult == -1)
        {
            illegalMoves++;
            Utils::print("Illegal: ", false);
            FastMoveGenerator::printMove(move);
            Utils::print(BoardState_T(&movePieces).dumpPos(), true);
            memcpy(&movePieces, pieces, sizeof(*pieces));
            FastMoveGenerator::applyMove(&movePieces, move);
        }
        else
        {
            if (foundMoves.at(moveResult) == -1)
            {
                legalMoves++;
                foundMoves.at(moveResult) = n;
                // FastMoveGenerator::printMove(move);
                // Utils::print(stringify(correctMoves->at(moveResult)), true);
            }
            else
            {
                duplicateMoves++;
                Utils::print("Duplicate move: ");
                FastMoveGenerator::printMove(move);
                Utils::print("Duplicate of: ");
                FastMoveGenerator::printMove(movesToCompare.at(foundMoves.at(moveResult)));
            }
        }
        n++;
    }
    Utils::print("\nGenerated " + std::to_string(movesToCompare.size()) + " moves", true);
    Utils::print("Expected " + std::to_string(correctMoves->size()) + " moves\n", true);
    Utils::print("Legal moves: " + std::to_string(legalMoves), true);
    Utils::print("Illegal moves: " + std::to_string(illegalMoves), true);
    Utils::print("Duplicate moves: " + std::to_string(duplicateMoves), true);
    Utils::print("Missing moves: " + std::to_string(correctMoves->size() - legalMoves), true);
    if (legalMoves != correctMoves->size())
    {
        Utils::print("\nCorrect moves:", true);
        for (int i = 0; i < correctMoves->size(); i++)
        {
            if (foundMoves.at(i) >= 0)
            {
                Utils::print(stringify(correctMoves->at(i)), true);
            }
        }
        Utils::print("\nMissing moves:", true);
        for (int i = 0; i < correctMoves->size(); i++)
        {
            if (foundMoves.at(i) < 0)
            {
                Utils::print(stringify(correctMoves->at(i)), true);
            }
        }
    }
}

void compareGenerators()
{
    std::string positions[] = {
        "-W10006 -B11206 11010100001100111111",
        "-W20606 -B30506 11010100001100111111",
        "rW30606 -W10607 -B50605 -B50706 -B50506 11111111111111111111",
        "rW30606 -W10607 -B50605 -B50706 -B50506 00000000000000000000",
        "b-10002 b-10005 -B10102 -W20308 rW30404 -B10500 -B10512 -B10602 -W20604 b-10607 r-10608 -W30610 -B10700 b-10708 -W10804 -W10912 -B11100 -B11110 11111100101101111111",
        "-W50000 11111111111111111111",
        "-B50000 00000000000000000000",
        "rW30404 -W20604 -W10804 11111100101101111111",
        "-W30610 r-10608 11111100101101111111",
        "-B30500 b-11200 bW20204 -W10304 -B30804 -B11204 -B20410 -W11210 bW20212 r-10412 -W10512 -W20712 11110111110111111010",
    };

    for (int i = 0; i < _countof(positions); i++)
    {
        tryPosition(positions[i], i);
    }
}

void executionSpeedTest()
{
    int time = 60;

    std::string positions[] = {
        "-W10006 -B11206 11010100001100111111",
        "-W20606 -B30506 11010100001100111111",
        "rW30606 -W10607 -B50605 -B50706 -B50506 11111111111111111111",
        "rW30606 -W10607 -B50605 -B50706 -B50506 00000000000000000000",
        "b-10002 b-10005 -B10102 -W20308 rW30404 -B10500 -B10512 -B10602 -W20604 b-10607 r-10608 -W30610 -B10700 b-10708 -W10804 -W10912 -B11100 -B11110 11111100101101111111",
        "-W50000 11111111111111111111",
        "-B50000 00000000000000000000",
        "rW30404 -W20604 -W10804 11111100101101111111",
        "-W30610 r-10608 11111100101101111111",
        "-B30500 b-11200 bW20204 -W10304 -B30804 -B11204 -B20410 -W11210 bW20212 r-10412 -W10512 -W20712 11110111110111111010",
    };

    BoardState_T boardState = BoardState_T();
    // boardState.loadPos("b-10002 b-10005 -B10102 -W20308 rW30404 -B10500 -B10512 -B10602 -W20604 b-10607 r-10608 -W30610 -B10700 b-10708 -W10804 -W10912 -B11100 -B11110 11111100101101111111");
    int basicLoops = 0;
    int endTime = Utils::millis() + 1000 * time;
    while (Utils::millis() < endTime)
    {
        boardState.loadPos(positions[basicLoops % _countof(positions)]);
        boardState.getMoves(basicLoops % 2, false, false);
        basicLoops++;
        boardState.getMoves(basicLoops % 2, false, false);
        basicLoops++;
    }
    Utils::print("Basic move generator loops in " + std::to_string(time) + " seconds: " + std::to_string(basicLoops), true);
    int advancedLoops = 0;
    endTime = Utils::millis() + 1000 * time;
    while (Utils::millis() < endTime)
    {
        boardState.loadPos(positions[advancedLoops % _countof(positions)]);
        FastMoveGenerator::getMoves(boardState.getPiecesReference(), advancedLoops % 2);
        advancedLoops++;
        FastMoveGenerator::getMoves(boardState.getPiecesReference(), advancedLoops % 2);
        advancedLoops++;
    }
    Utils::print("Fast move generator loops in " + std::to_string(time) + " seconds: " + std::to_string(advancedLoops), true);
}

void tryPosition(std::string position, int positonIndex)
{
    BoardState_T boardState = BoardState_T();
    boardState.loadPos(position);
    Utils::print("\nPosition #" + std::to_string(positonIndex) + ", playing as White", true);
    runPosition(boardState.getPiecesReference(), true);
    Utils::print("\nPosition #" + std::to_string(positonIndex) + ", playing as Black", true);
    runPosition(boardState.getPiecesReference(), false);
}
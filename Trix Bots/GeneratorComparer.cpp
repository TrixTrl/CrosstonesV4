#include "GeneratorComparer.h"

void runPosition(uint8_t (*pieces)[13][13], bool isWhite)
{
    BoardState_T boardState = BoardState_T(pieces);
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
            memcpy(&movePieces, pieces, sizeof(*pieces));
            FastMoveGenerator::applyMove(&movePieces, move);
        }
        else
        {
            if (foundMoves.at(moveResult) == -1)
            {
                legalMoves++;
                foundMoves.at(moveResult) = n;
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
    BoardState_T boardState = BoardState_T();
    // boardState.loadPos("-W10006 -B11206 11010100001100111111"); //("-W10006 11010100001100111111");
    // runPosition(boardState.getPiecesReference(), true);
    // runPosition(boardState.getPiecesReference(), false);
    boardState.loadPos("-W10006 -W10106 -B21206 11010100001100111111");
    runPosition(boardState.getPiecesReference(), true);
    // runPosition(boardState.getPiecesReference(), false);
}

void executionSpeedTest()
{
    BoardState_T boardState = BoardState_T();
    boardState.loadPos("b-10002 b-10005 -B10102 -W20308 rW30404 -B10500 -B10512 -B10602 -W20604 b-10607 r-10608 -W30610 -B10700 b-10708 -W10804 -W10912 -B11100 -B11110 11111100101101111111");
    int basicLoops = 0;
    int endTime = Utils::millis() + 10000;
    while (Utils::millis() < endTime)
    {
        boardState.getMoves(basicLoops % 2, false, false);
        basicLoops++;
    }
    Utils::print("Basic move generator loops in 10 seconds: " + std::to_string(basicLoops), true);
    int advancedLoops = 0;
    endTime = Utils::millis() + 10000;
    while (Utils::millis() < endTime)
    {
        FastMoveGenerator::getMoves(boardState.getPiecesReference(), advancedLoops % 2);
        advancedLoops++;
    }
    Utils::print("Fast move generator loops in 10 seconds: " + std::to_string(advancedLoops), true);
}
#include "GeneratorComparer.h"

void runPosition(uint8_t (*pieces)[13][13], bool isWhite)
{
    BoardState_T boardState = BoardState_T(pieces);
    std::shared_ptr<std::vector<std::vector<BoardState_T::xMove>>> correctMoves = boardState.getMoves(true, false, false);
    std::vector<FastMoveGenerator::move> movesToCompare = FastMoveGenerator::getMoves(pieces, isWhite);

    for (const auto &move : movesToCompare)
    {
        uint8_t movePieces[13][13];
        memcpy(&movePieces, pieces, sizeof(*pieces));
        FastMoveGenerator::applyMove(&movePieces, move);
        BoardState_T comparisonBoardState = BoardState_T(pieces);
        int moveResult = comparisonBoardState.makeMove(&movePieces, isWhite);
        Utils::print(moveResult, true);
    }
}

void compareGenerators()
{
    BoardState_T boardState = BoardState_T();
    boardState.loadPos("-W10006 11010100001100111111");
    runPosition(boardState.getPiecesReference(), true);
    runPosition(boardState.getPiecesReference(), false);
    boardState.loadPos("-W10006 -B11206 11010100001100111111");
    runPosition(boardState.getPiecesReference(), true);
    runPosition(boardState.getPiecesReference(), false);
}
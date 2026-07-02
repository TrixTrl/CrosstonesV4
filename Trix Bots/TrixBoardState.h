#pragma once

#include <cstdint>
#include <bitset>
#include <vector>
#include <memory>
#include "../Globals/Piece.h"
#include "../fix_win32_compatibility.h"
#include <iostream>
#include <string>
#include <cstdint>
#include <sstream>
#include <iomanip>
#include "Utils.h"

class BoardState_T
{
public:
    BoardState_T();
    BoardState_T(BoardState_T *boardState);
    BoardState_T(uint8_t (*src)[13][13]);
    void rst(std::bitset<3> &tps);
    void wipe();

    void copyBoard(uint8_t (*dest)[13][13]) const;
    void loadBoard(uint8_t (*src)[13][13]);

    struct xMove
    {
        int i;
        int j;
        uint8_t delta;
        xMove(int I, int J, uint8_t D) : i(I), j(J), delta(D) {}
    };

    std::shared_ptr<std::vector<std::vector<xMove>>> getMoves(bool isWhite, bool fullMovesOnly, bool singleTowerExploration) const;
    void basicGenerator(std::shared_ptr<std::vector<std::vector<xMove>>> moves, uint8_t (*state)[13][13], int x, int y, bool (*visited)[13][13], int remainingSteps, bool turned, bool isWhite, bool fullMovesOnly) const;
    void captureGenerator(std::shared_ptr<std::vector<std::vector<xMove>>> moves, uint8_t (*state)[13][13], int originX, int originY, int x, int y, bool (*visited)[13][13], int remainingSteps, bool turned, bool isWhite, bool fullMovesOnly) const;

    void unsafeMakeMove(std::vector<xMove> *move);
    int makeMove(std::vector<xMove> *move, bool isWhiteTurn); // return -1 for invalid, 0 for passing, and 1 for any other valid move
    int makeMove(uint8_t (*newState)[13][13], bool isWhiteTurn);

    enum class winValue
    {
        none,
        white,
        black,
        draw
    };

    winValue gameOver(bool isWhite);

    static const uint8_t hasTurnPiece = 128;
    static const uint8_t setTurnPiece = 64;

    std::string dumpPos();
    void loadPos(std::string str);

    std::vector<std::string> gameRecord;
    void dumpGame();

    std::string getKey();

    uint8_t (*getPiecesReference())[13][13];

private:
    uint8_t pieces[13][13];
};
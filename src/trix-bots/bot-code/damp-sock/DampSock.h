#pragma once

#include "../../utils/Utils.h"
#include "../../../Globals/Player.h"
#include "AlphaBeta.h"
#include "DampSockStructs.h"

class DampSock : public Player
{
public:
    DampSock(int thinkingTime) : thinkingTime(thinkingTime) {};
    void getMoveToPlay(uint8_t (*board)[13][13], bool isWhite, int endTime);
    bool listensToKeyInputs() override
    {
        return false;
    }
    int thinkingTime = 10;
    std::map<std::string, DampNode> table = std::map<std::string, DampNode>();
};
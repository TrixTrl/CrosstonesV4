#include "test-bots/BasicGenerator.h"
#include <iostream>
#include <game-suite/GamePosition.h>

class MoveGenVerifier {
public:
    // This compares the outputs of both generators for a single position
    static bool verifyPosition(const GamePosition& startPosition, bool isWhite) {
        
        std::vector<GamePosition> statesA = generateResultingStatesBaseline(startPosition, isWhite);
        std::vector<GamePosition> statesB = generateResultingStatesB(startPosition, isWhite);

        if (statesA.size() != statesB.size()) {
            std::cout << "Mismatch! Gen A found " << statesA.size() 
                      << " moves, but Gen B found " << statesB.size() << ".\n";
            return false;
        }

        std::sort(statesA.begin(), statesA.end());
        std::sort(statesB.begin(), statesB.end());

        for (size_t i = 0; i < statesA.size(); ++i) {
            if (!(statesA[i] == statesB[i])) {
                std::cout << "Mismatch! The resulting board states do not match at index " << i << ".\n";

                // TODO: print board mismatch

                return false;
            }
        }
        return true; 
    }

private:
    static std::vector<GamePosition> generateResultingStatesBaseline(const GamePosition& start, bool isWhite) {
        std::vector<GamePosition> resultingPositions;
        
        auto moves = BasicGenerator::getMoves(isWhite, &start.pieces);
        
        for (const auto& moveSequence : *moves) {
            GamePosition nextState = start;
            
            for (const auto& step : moveSequence) {
                nextState[step.i][step.j] ^= step.delta;
            }
            
            resultingPositions.push_back(nextState);
        }
        
        return resultingPositions;
    }

    // Helper to run your New Generator and apply its moves
    static std::vector<GamePosition> generateResultingStatesB(const GamePosition& start, bool isWhite) {
        std::vector<GamePosition> resultingPositions;

        // 1. Get moves from your new generator (whatever format they are in)
        // auto newMoves = NewGenerator::getMoves(isWhite, start);
        
        // 2. Apply them to get the resulting board states
        // for (const auto& move : newMoves) {
        //     BoardState nextState = applyNewMove(start, move);
        //     resultingPositions.push_back(nextState);
        // }

        return resultingPositions;
    }
};
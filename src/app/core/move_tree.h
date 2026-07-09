#pragma once
#include <string>
#include <vector>
#include <memory>
#include "game-suite/Board.h"


struct OpeningLabel {
    std::string name; // eg "Korean Opening"
    std::string code; // eg "K3" or empty
    std::vector<int> modes; // applicable game modes
};

struct MoveNode {
    // usually no or one label, except when the same move sequence is named differently in different modes
    std::vector<OpeningLabel> labels;
    // Union of each labels modes
    std::vector<int> modes;
    std::vector<Board::xMove> moveData;
    MoveNode* parent = nullptr;
    std::vector<std::unique_ptr<MoveNode>> children;

    // Returns the label of the Node matching the mode, or nullptr
    const OpeningLabel* labelForMode(int mode) const {
        for (auto& l : labels) {
            if (l.modes.empty()) return &l;
            for (int m : l.modes) if (m == mode) return &l;
        }
        return nullptr;
    }

	MoveNode* addChild(const std::vector<Board::xMove>& move);
	MoveNode* findChild(const std::vector<Board::xMove>& move) const;
    std::vector<MoveNode*> pathToRoot() const;
};

struct EvalProvider {
    virtual ~EvalProvider() = default;
    virtual float evaluate(const Board&, const std::vector<Board::xMove>&) { return 0.0f; }
};

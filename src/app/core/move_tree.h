#pragma once
#include <string>
#include <vector>
#include <memory>
#include "game-suite/Board.h"

struct MoveNode {
    std::string name;
    std::vector<Board::xMove> moveData;
    MoveNode* parent = nullptr;
    std::vector<std::unique_ptr<MoveNode>> children;

	MoveNode* addChild(const std::vector<Board::xMove>& move);
	MoveNode* findChild(const std::vector<Board::xMove>& move) const;
    std::vector<MoveNode*> pathToRoot() const;
};

struct EvalProvider {
    virtual ~EvalProvider() = default;
    virtual float evaluate(const Board&, const std::vector<Board::xMove>&) { return 0.0f; }
};

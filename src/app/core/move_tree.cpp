#include "move_tree.h"
#include <algorithm>

MoveNode* MoveNode::addChild(const std::vector<Board::xMove>& move) {
    auto child = std::make_unique<MoveNode>();
    child->moveData = move;
    child->parent = this;
    auto* ptr = child.get();
    children.push_back(std::move(child));
    return ptr;
}

MoveNode* MoveNode::findChild(const std::vector<Board::xMove>& move) const {
    for (const auto& c : children) {
        if (c->moveData == move) {
            return c.get();
        }
    }
    return nullptr;
}

std::vector<MoveNode*> MoveNode::pathToRoot() const {
    std::vector<MoveNode*> path;
    const MoveNode* n = this;
    while (n) {
        path.push_back(const_cast<MoveNode*>(n));
        n = n->parent;
    }
    std::reverse(path.begin(), path.end());
    return path;
}

#include "layout.h"
#include <algorithm>

namespace ui {

void Slot::resolve(Rectangle bounds, float scale) {
    rect = bounds;
    if (children.empty()) return;

    int n = (int)children.size();
    if (n == 1) {
        children[0].resolve(bounds, scale);
        return;
    }

    float totalFlex = 0;
    float totalFixed = 0;
    for (auto& child : children) {
        if (child.fixed > 0)
            totalFixed += child.fixed * scale;
        else
            totalFlex += child.flex;
    }

    float available = (dir == Dir::Row ? bounds.width : bounds.height) - totalFixed;
    if (available < 0) available = 0;

    float pos = (dir == Dir::Row ? bounds.x : bounds.y);
    float crossPos = (dir == Dir::Row ? bounds.y : bounds.x);
    float crossSize = (dir == Dir::Row ? bounds.height : bounds.width);

    for (auto& child : children) {
        float size = 0;
        if (child.fixed > 0) {
            size = child.fixed * scale;
        } else {
            size = available * (child.flex / totalFlex);
        }

        float minSize = child.minPx * scale;
        if (size < minSize) size = minSize;

        Rectangle childRect;
        if (dir == Dir::Row) {
            childRect = { pos, crossPos, size, crossSize };
        } else {
            childRect = { crossPos, pos, crossSize, size };
        }

        child.resolve(childRect, scale);
        pos += size;
    }
}

Slot* Slot::find(std::string_view id) {
    if (this->id == id) return this;
    for (auto& child : children) {
        if (auto* found = child.find(id))
            return found;
    }
    return nullptr;
}

}

#include "layout.h"
#include <algorithm>

namespace ui {

void Slot::resolve(Rectangle bounds, float scale) {
    rect = bounds;
    if (children.empty()) return;

    Rectangle inner = {
        bounds.x + padding.left * scale,
        bounds.y + padding.top * scale,
        bounds.width - (padding.left + padding.right) * scale,
        bounds.height - (padding.top + padding.bottom) * scale
    };
    if (inner.width < 0) inner.width = 0;
    if (inner.height < 0) inner.height = 0;

    int n = (int)children.size();
    if (n == 1) {
        children[0].resolve(inner, scale);
        return;
    }

    float gapScaled = gap * scale;
    float totalGap = gapScaled * (n - 1);

    float totalFlex = 0;
    float totalFixed = 0;
    for (auto& child : children) {
        if (child.fixed > 0)
            totalFixed += child.fixed * scale;
        else
            totalFlex += child.flex;
    }

    float available = (dir == Dir::Row ? inner.width : inner.height) - totalFixed - totalGap;
    if (available < 0) available = 0;

    float pos = (dir == Dir::Row ? inner.x : inner.y);
    float crossPos = (dir == Dir::Row ? inner.y : inner.x);
    float crossSize = (dir == Dir::Row ? inner.height : inner.width);

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
        pos += size + gapScaled;
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

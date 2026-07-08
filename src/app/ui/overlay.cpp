#include "overlay.h"
#include <algorithm>
#include <vector>

namespace ui {

namespace {

struct OverlayEntry {
    std::string id;
    int zOrder = 0;
    std::function<void(const Theme&)> draw;
};

std::vector<OverlayEntry>& entries() {
    static std::vector<OverlayEntry> list;
    return list;
}

}

void overlayBegin() {
    entries().clear();
}

void overlayPush(std::string_view id, std::function<void(const Theme&)> draw, int zOrder) {
    entries().push_back({std::string(id), zOrder, std::move(draw)});
}

void overlayFlush(const Theme& theme) {
    auto& list = entries();
    std::stable_sort(list.begin(), list.end(), [](const OverlayEntry& a, const OverlayEntry& b) {
        return a.zOrder < b.zOrder;
    });
    for (auto& e : list) e.draw(theme);
    list.clear();
}

}

#pragma once
#include <functional>
#include <vector>

namespace ui {

inline std::vector<std::function<void()>>& popupDraws() {
    static std::vector<std::function<void()>> queue;
    return queue;
}

inline void schedulePopupDraw(std::function<void()> fn) {
    popupDraws().push_back(std::move(fn));
}

inline void flushPopupDraws() {
    for (auto& fn : popupDraws()) fn();
    popupDraws().clear();
}

}

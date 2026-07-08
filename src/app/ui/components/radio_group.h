#pragma once
#include <span>
#include <string_view>
#include "raylib.h"
#include "app/ui/theme.h"

namespace ui {

struct RadioOption {
    Rectangle rect;
    std::string_view label;
};

struct RadioGroup {
    int hoveredIdx = -1;

    void draw(std::span<const RadioOption> options, int selectedIdx, const Theme& t);
    int handleInput(std::span<const RadioOption> options, Vector2 mouse, bool click);
};

}

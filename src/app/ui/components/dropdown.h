#pragma once
#include <span>
#include <string_view>
#include "raylib.h"
#include "app/ui/theme.h"

namespace ui {

struct Dropdown {
    bool open = false;
    int selectedIdx = 0;
    int hoveredIdx = -1;

    void draw(Rectangle rect, std::span<const std::string_view> items, const Theme& t);
    int handleInput(Rectangle rect, Vector2 mouse, bool click, int numItems);
};

}

#pragma once
#include <span>
#include <string_view>
#include "raylib.h"
#include "app/ui/theme.h"

namespace ui {

struct Stepper {
    int selectedIdx = 0;

    void draw(Rectangle rect, std::span<const std::string_view> options, const Theme& t);
    int handleInput(Rectangle rect, Vector2 mouse, bool click, int numOptions);
};

}

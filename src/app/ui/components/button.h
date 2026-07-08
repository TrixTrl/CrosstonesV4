#pragma once
#include <string_view>
#include "raylib.h"
#include "app/ui/theme.h"

namespace ui {

struct Button {
    bool hovered = false;

    void draw(Rectangle rect, std::string_view label, const Theme& t);
    bool handleInput(Rectangle rect, Vector2 mouse, bool click);
};

}

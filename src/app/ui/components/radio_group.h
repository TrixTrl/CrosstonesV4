#pragma once
#include <span>
#include <string_view>
#include "raylib.h"
#include "app/ui/theme.h"

namespace ui {

// Replaces GameApp's ad hoc drawRadio() free function, which computed hover
// state inline from GetMousePosition() at draw time and used hardcoded RGB
// literals instead of theme colors. Options can have different widths/x
// positions (unlike Stepper's two-arrow layout), so callers pass explicit
// rects rather than a single rect to be internally split.
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

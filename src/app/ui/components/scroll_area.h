#pragma once
#include "raylib.h"

namespace ui {

struct ScrollArea {
    Vector2 scroll{0, 0};
    int contentH{0};

    Rectangle begin(Rectangle bounds);
    void end();
};

} // namespace ui

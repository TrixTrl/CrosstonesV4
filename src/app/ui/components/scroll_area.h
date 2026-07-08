#pragma once
#include "raylib.h"

namespace ui {

struct ScrollArea {
    Vector2 scroll{0, 0};
    int contentH{0};

    // Begins a clipped, scrollable region: draws the GuiScrollPanel chrome,
    // begins scissor mode on the returned view rect, and returns it so the
    // caller knows where to draw content. Must be paired with end(). This
    // replaces the old begin(bounds)-only API that callers (OpeningExplorer)
    // were bypassing with their own manual BeginScissorMode/scroll.y reads.
    Rectangle begin(Rectangle bounds, int contentHeight);
    void end();
};

} // namespace ui

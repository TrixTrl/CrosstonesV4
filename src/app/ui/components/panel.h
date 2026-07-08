#pragma once
#include <string_view>
#include "raylib.h"
#include "app/ui/theme.h"

namespace ui {

// Themed rect+border with an optional title, replacing screens' direct
// GuiPanel(...) calls (raygui adds nothing over this for a plain panel).
struct Panel {
    static void draw(Rectangle rect, std::string_view title, const Theme& t);
};

}

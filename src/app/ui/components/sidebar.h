#pragma once
#include <string_view>
#include <vector>
#include "raylib.h"
#include "app/ui/theme.h"

namespace ui {

struct SidebarEntry {
    std::string_view icon;
    std::string_view title;
};

struct Sidebar {
    std::vector<SidebarEntry> entries;
    int activeIdx = 0;
    bool open = true;

    void draw(Rectangle rect, const Theme& t);
    // returns clicked entry index (-1 = none). hamburgerClicked is set if hamburger was pressed.
    int handleClick(Rectangle rect, Vector2 mouse, bool click, bool& hamburgerClicked, const Theme& t);
};

}

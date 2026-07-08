#pragma once
#include <string_view>
#include "raylib.h"
#include "app/ui/theme.h"

namespace ui {

struct Panel {
    static void draw(Rectangle rect, std::string_view title, const Theme& t);
};

}

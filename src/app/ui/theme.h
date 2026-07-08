#pragma once
#include "raylib.h"

namespace ui {

struct Theme {
    Color bg{25, 25, 40, 255};
    Color panel{50, 50, 75, 255};
    Color panelHover{70, 70, 100, 255};
    Color panelActive{45, 45, 70, 255};
    Color border{100, 100, 130, 255};
    Color text{220, 220, 220, 255};
    Color textDim{180, 180, 180, 255};
    Color textAccent{255, 200, 50, 255};
    Color textSection{130, 130, 160, 255};
    Color success{100, 200, 100, 255};
    Color successHover{140, 240, 140, 255};

    int fontSize{14};
    int fontSizeSmall{12};
    int fontSizeTitle{16};
    int spacing{4};
    int itemH{28};
    float scale = 1.0f;
};

void applyTheme(const Theme& t);

} // namespace ui

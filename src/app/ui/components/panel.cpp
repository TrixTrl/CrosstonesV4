#include "panel.h"
#include "app/FontManager.h"

namespace ui {

void Panel::draw(Rectangle rect, std::string_view title, const Theme& t) {
    DrawRectangleRec(rect, t.panel);
    DrawRectangleLinesEx(rect, 1, t.border);
    if (!title.empty()) {
        DrawAppText(title.data(), rect.x + 8 * t.scale, rect.y + 4 * t.scale, (float)t.fontSizeSmall, t.textSection);
    }
}

}

#include "button.h"
#include "app/FontManager.h"

namespace ui {

void Button::draw(Rectangle rect, std::string_view label, const Theme& t) {
    DrawRectangleRec(rect, hovered ? t.panelHover : t.panel);
    DrawRectangleLinesEx(rect, 1, t.border);
    float tw = MeasureAppText(label.data(), (float)t.fontSizeTitle);
    float yOff = (rect.height - t.fontSizeTitle) / 2;
    DrawAppText(label.data(), rect.x + rect.width / 2.0f - tw / 2.0f, rect.y + yOff, (float)t.fontSizeTitle, WHITE);
}

bool Button::handleInput(Rectangle rect, Vector2 mouse, bool click) {
    hovered = CheckCollisionPointRec(mouse, rect);
    return click && hovered;
}

}

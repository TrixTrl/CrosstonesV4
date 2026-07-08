#include "int_field.h"
#include "app/FontManager.h"
#include <algorithm>

namespace ui {

void IntField::draw(Rectangle rect, const Theme& t) {
    Color bg = active ? Color{45, 45, 70, 255} : Color{35, 35, 55, 255};
    Color border = active ? Color{120, 120, 160, 255} : t.border;
    DrawRectangleRec(rect, bg);
    DrawRectangleLinesEx(rect, 1, border);

    std::string display = text.empty() ? "0" : text;
    if (active && (cursorBlink / 15) % 2 == 0) display += "|";
    float yOff = (rect.height - t.fontSizeTitle) / 2;
    DrawAppText(display.c_str(), rect.x + 8 * t.scale, rect.y + yOff, (float)t.fontSizeTitle, WHITE);
}

void IntField::handleInput(Rectangle rect, Vector2 mouse, bool click) {
    if (click) {
        active = CheckCollisionPointRec(mouse, rect);
        if (!active) commit();
        return;
    }

    if (!active) return;

    cursorBlink++;

    int c = GetCharPressed();
    while (c > 0) {
        if (c >= '0' && c <= '9' && text.length() < 3) {
            if (text == "0") text.clear();
            text += (char)c;
        }
        c = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && !text.empty()) {
        text.pop_back();
    }
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) {
        commit();
        active = false;
    }
}

void IntField::commit() {
    if (!text.empty()) value = std::clamp(std::stoi(text), minVal, maxVal);
    else value = 0;
    text = std::to_string(value);
}

}

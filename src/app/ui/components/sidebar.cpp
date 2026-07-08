#include "sidebar.h"
#include "app/FontManager.h"

namespace ui {

void Sidebar::draw(Rectangle rect, const Theme& t) {
    float s = t.scale;
    float sidebarW = open ? 200.0f * s : 40.0f * s;

    // Background & border
    Color sidebarBg = {35, 35, 55, 255};
    DrawRectangleRec(rect, sidebarBg);
    DrawRectangleLinesEx(rect, (int)(1 * s), Color{50, 50, 70, 255});

    // Hamburger icon
    Rectangle hamRect = {rect.x + 6 * s, rect.y + 6 * s, 28 * s, 28 * s};
    int hx = (int)hamRect.x + (int)(4 * s);
    int hy = (int)hamRect.y + (int)(6 * s);
    for (int line = 0; line < 3; line++) {
        DrawRectangle(hx, hy + (int)(line * 6 * s), (int)(20 * s), (int)(2 * s), Color{180, 180, 180, 255});
    }

    // Entries
    float itemH = open ? 32.0f * s : 40.0f * s;
    float stride = open ? 36.0f * s : 44.0f * s;

    for (size_t i = 0; i < entries.size(); i++) {
        Rectangle itemR = {rect.x, rect.y + 40.0f * s + i * stride, sidebarW, itemH};
        bool hover = CheckCollisionPointRec(GetMousePosition(), itemR);
        bool active = (int)i == activeIdx;

        Color itemBg = active ? Color{60, 60, 90, 255} :
                        hover ? Color{50, 50, 75, 255} : Color{0, 0, 0, 0};
        DrawRectangleRec(itemR, itemBg);

        if (open) {
            DrawAppText(entries[i].icon.data(), itemR.x + 10 * s, itemR.y + 6 * s,
                        (float)t.fontSizeTitle,
                        active ? WHITE : Color{180, 180, 180, 255});
            DrawAppText(entries[i].title.data(), itemR.x + 36 * s, itemR.y + 6 * s,
                        (float)t.fontSize,
                        active ? WHITE : Color{160, 160, 160, 255});
        } else {
            float iconW = MeasureAppText(entries[i].icon.data(), (float)t.fontSizeTitle);
            DrawAppText(entries[i].icon.data(),
                        itemR.x + (itemR.width - iconW) / 2,
                        itemR.y + 10 * s,
                        (float)t.fontSizeTitle,
                        active ? WHITE : Color{180, 180, 180, 255});
        }
    }
}

int Sidebar::handleClick(Rectangle rect, Vector2 mouse, bool click, bool& hamburgerClicked, const Theme& t) {
    hamburgerClicked = false;
    if (!click) return -1;

    float s = t.scale;
    Rectangle hamRect = {rect.x + 6 * s, rect.y + 6 * s, 28 * s, 28 * s};
    if (CheckCollisionPointRec(mouse, hamRect)) {
        hamburgerClicked = true;
        return -1;
    }

    float sidebarW = open ? 200.0f * s : 40.0f * s;
    float itemH = open ? 32.0f * s : 40.0f * s;
    float stride = open ? 36.0f * s : 44.0f * s;

    for (size_t i = 0; i < entries.size(); i++) {
        Rectangle itemR = {rect.x, rect.y + 40.0f * s + i * stride, sidebarW, itemH};
        if (CheckCollisionPointRec(mouse, itemR))
            return (int)i;
    }
    return -1;
}

}

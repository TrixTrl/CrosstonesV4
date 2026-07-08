#include "dropdown.h"
#include "app/FontManager.h"
#include "app/ui/popup.h"
#include <vector>
#include <algorithm>

namespace ui {

void Dropdown::draw(Rectangle rect, std::span<const std::string_view> items, const Theme& t) {
    DrawRectangleRec(rect, open ? t.panelHover : t.panel);
    DrawRectangleLinesEx(rect, 1, t.border);
    if (!items.empty()) {
        if (selectedIdx >= (int)items.size()) selectedIdx = 0;
        std::string_view selected = items[selectedIdx];
        float nameW = MeasureAppText(selected.data(), (float)t.fontSizeTitle);
        DrawAppText(selected.data(), rect.x + rect.width / 2.0f - nameW / 2.0f, rect.y + 5 * t.scale, (float)t.fontSizeTitle, WHITE);
    }
    DrawAppText(open ? "^" : "v", rect.x + rect.width - 20 * t.scale, rect.y + 5 * t.scale, (float)t.fontSizeTitle, WHITE);

    if (open && !items.empty()) {
        std::vector<std::string_view> itemsCopy(items.begin(), items.end());
        schedulePopupDraw([this, rect, itemsCopy, t]() {
            for (int p = 0; p < (int)itemsCopy.size(); p++) {
                Rectangle optRect = { rect.x, rect.y + rect.height + p * rect.height, rect.width, rect.height };
                DrawRectangleRec(optRect, hoveredIdx == p ? t.panelHover : t.panelActive);
                DrawRectangleLinesEx(optRect, 1, t.border);
                DrawAppText(itemsCopy[p].data(), optRect.x + 10 * t.scale, optRect.y + 5 * t.scale, (float)t.fontSize, hoveredIdx == p ? WHITE : t.textDim);
            }
        });
    }
}

int Dropdown::handleInput(Rectangle rect, Vector2 mouse, bool click, int numItems) {
    int action = -1;

    hoveredIdx = -1;
    if (open) {
        for (int p = 0; p < numItems; p++) {
            Rectangle optRect = { rect.x, rect.y + rect.height + p * rect.height, rect.width, rect.height };
            if (CheckCollisionPointRec(mouse, optRect)) {
                hoveredIdx = p;
                if (click) {
                    selectedIdx = p;
                    open = false;
                    action = p;
                }
                break;
            }
        }
        if (click && hoveredIdx == -1) {
            open = false;
        }
    } else {
        if (click && CheckCollisionPointRec(mouse, rect)) {
            open = true;
        }
    }

    return action;
}

}

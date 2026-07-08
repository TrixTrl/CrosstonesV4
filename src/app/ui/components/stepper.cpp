#include "stepper.h"
#include "app/FontManager.h"

namespace ui {

void Stepper::draw(Rectangle rect, std::span<const std::string_view> options, const Theme& t) {
    if (options.empty()) return;
    float arrowW = rect.height;
    float gap = 4 * t.scale;
    Rectangle leftRect = { rect.x, rect.y, arrowW, rect.height };
    Rectangle rightRect = { rect.x + rect.width - arrowW, rect.y, arrowW, rect.height };
    Rectangle labelRect = { rect.x + arrowW + gap, rect.y, rect.width - arrowW * 2 - gap * 2, rect.height };

    DrawRectangleRec(leftRect, t.panel);
    DrawRectangleLinesEx(leftRect, 1, t.border);
    float lw = MeasureAppText("<", (float)t.fontSizeTitle);
    DrawAppText("<", leftRect.x + (arrowW - lw) / 2, leftRect.y + 5 * t.scale, (float)t.fontSizeTitle, WHITE);

    if (selectedIdx >= (int)options.size()) selectedIdx = 0;
    std::string_view selected = options[selectedIdx];
    DrawAppText(selected.data(), labelRect.x, labelRect.y + 5 * t.scale, (float)t.fontSizeTitle, WHITE);

    DrawRectangleRec(rightRect, t.panel);
    DrawRectangleLinesEx(rightRect, 1, t.border);
    float rw = MeasureAppText(">", (float)t.fontSizeTitle);
    DrawAppText(">", rightRect.x + (arrowW - rw) / 2, rightRect.y + 5 * t.scale, (float)t.fontSizeTitle, WHITE);
}

int Stepper::handleInput(Rectangle rect, Vector2 mouse, bool click, int numOptions) {
    float arrowW = rect.height;
    Rectangle leftRect = { rect.x, rect.y, arrowW, rect.height };
    Rectangle rightRect = { rect.x + rect.width - arrowW, rect.y, arrowW, rect.height };

    if (click) {
        if (CheckCollisionPointRec(mouse, leftRect)) {
            selectedIdx = (selectedIdx - 1 + numOptions) % numOptions;
            return selectedIdx;
        }
        if (CheckCollisionPointRec(mouse, rightRect)) {
            selectedIdx = (selectedIdx + 1) % numOptions;
            return selectedIdx;
        }
    }
    return -1;
}

}

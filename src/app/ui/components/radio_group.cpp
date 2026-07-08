#include "radio_group.h"
#include "app/FontManager.h"

namespace ui {

void RadioGroup::draw(std::span<const RadioOption> options, int selectedIdx, const Theme& t) {
    for (int i = 0; i < (int)options.size(); i++) {
        const RadioOption& opt = options[i];
        bool selected = (i == selectedIdx);
        bool hovered = (i == hoveredIdx);

        Color dot = selected ? (hovered ? t.successHover : t.success)
                              : (hovered ? t.textDim : Color{128, 128, 128, 255});

        float cx = opt.rect.x + 10 * t.scale;
        float cy = opt.rect.y + opt.rect.height / 2;
        DrawCircleV({cx, cy}, 6 * t.scale, dot);
        if (selected)
            DrawCircleV({cx, cy}, 3 * t.scale, WHITE);
        DrawAppText(opt.label.data(), opt.rect.x + 22 * t.scale, opt.rect.y + 3 * t.scale,
                    (float)t.fontSize, selected ? WHITE : t.textDim);
    }
}

int RadioGroup::handleInput(std::span<const RadioOption> options, Vector2 mouse, bool click) {
    hoveredIdx = -1;
    for (int i = 0; i < (int)options.size(); i++) {
        if (CheckCollisionPointRec(mouse, options[i].rect)) {
            hoveredIdx = i;
            break;
        }
    }
    if (click && hoveredIdx >= 0) return hoveredIdx;
    return -1;
}

}

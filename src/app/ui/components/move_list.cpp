#include "move_list.h"
#include "app/ui/theme.h"
#include "app/FontManager.h"
#include <algorithm>

namespace ui {

void MoveList::draw(int x, int y, int w, const Theme& t) {
    int rowY = y;
    int indent = (int)(8 * t.scale);

    for (size_t i = 0; i < entries.size(); i++) {
        const auto& e = entries[i];
        bool hover = ((int)i == hoveredIdx);

        int h = t.itemH;

        if (hover) {
            DrawRectangle(x, rowY, w, h, t.panelHover);
        }

        char evalBuf[16];
        snprintf(evalBuf, sizeof(evalBuf), "%+.2f", e.eval);
        float evalW = MeasureAppText(evalBuf, t.fontSizeSmall);
        DrawAppText(evalBuf, (float)(x + w - indent - (int)evalW),
                    (float)(rowY + (h - t.fontSizeSmall) / 2),
                    (float)t.fontSizeSmall, t.textDim);

        Color labelColor = e.isBook ? t.textAccent : t.text;
        DrawAppText(e.label.c_str(), (float)(x + indent),
                    (float)(rowY + (h - t.fontSize) / 2),
                    (float)t.fontSize, labelColor);

        if (e.isBook && !e.subtitle.empty()) {
            float labelEnd = indent + MeasureAppText(e.label.c_str(), (float)t.fontSize) + (int)(8 * t.scale);
            DrawAppText(e.subtitle.c_str(),
                        (float)(x + indent + labelEnd),
                        (float)(rowY + (h - t.fontSizeSmall) / 2),
                        (float)t.fontSizeSmall, t.textSection);
        }

        rowY += h;
    }
}

int MoveList::handleInput(Vector2 mouse, bool click) {
    hoveredIdx = -1;
    return -1;
}

int MoveList::totalHeight(const Theme& t) const {
    return (int)entries.size() * t.itemH;
}

} // namespace ui

#include "move_history.h"
#include "app/ui/theme.h"
#include "app/FontManager.h"

namespace ui {

void MoveHistory::draw(int x, int y, int w, const Theme& t) {
    int rowY = y;
    int indent = (int)(12 * t.scale);
    int colW = (w - indent * 2) / 2;

    for (size_t i = 0; i < rows.size(); i++) {
        const auto& r = rows[i];
        bool rowHover = ((int)i == hoveredIdx);
        bool isActive = ((int)i == (int)rows.size() - 1);
        int h = t.itemH;
        int midX = x + indent + colW;

        if (rowHover) {
            int hlX = (hoveredCell == 1) ? midX : x;
            int hlW = (hoveredCell == 1) ? x + w - midX : midX - x;
            DrawRectangle(hlX, rowY, hlW, h, t.panelHover);
        }

        if (isActive) {
            DrawRectangle(x, rowY, (int)(3 * t.scale), h, t.textAccent);
        }

        std::string numStr = std::to_string(r.moveNumber);
        DrawAppText(numStr.c_str(), (float)(x + indent),
                    (float)(rowY + (h - t.fontSize) / 2),
                    (float)t.fontSize, t.textDim);

        float numW = MeasureAppText(numStr.c_str(), (float)t.fontSize);
        Color whiteColor = i == 0 ? t.textSection : t.text;
        DrawAppText(r.white.c_str(), (float)(x + indent + (int)numW + (int)(8 * t.scale)),
                    (float)(rowY + (h - t.fontSize) / 2),
                    (float)t.fontSize, whiteColor);

        if (!r.black.empty()) {
            DrawAppText(r.black.c_str(), (float)(x + indent + colW),
                        (float)(rowY + (h - t.fontSize) / 2),
                        (float)t.fontSize, isActive ? t.textAccent : t.text);
        }

        rowY += h;
    }
}

int MoveHistory::handleInput(Vector2 mouse, bool click) {
    hoveredIdx = -1;
    return -1;
}

int MoveHistory::totalHeight(const Theme& t) const {
    return (int)rows.size() * t.itemH;
}

} // namespace ui

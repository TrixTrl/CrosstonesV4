#include "analysis_stub.h"
#include "raygui.h"
#include "app/FontManager.h"
#include "app/ui/theme.h"

namespace ui {

void AnalysisStub::draw(Rectangle bounds, const Theme& t) {
    GuiPanel(bounds, "Analysis");

    int x = (int)(bounds.x + 12 * t.scale);
    int y = (int)(bounds.y + 30 * t.scale);
    int w = (int)(bounds.width - 24 * t.scale);

    auto drawInfo = [&](const char* label, const char* value) {
        DrawAppText(label, (float)x, (float)y, (float)t.fontSize, t.textDim);
        float valW = MeasureAppText(value, (float)t.fontSize);
        DrawAppText(value, (float)(x + w - (int)valW), (float)y, (float)t.fontSize, t.text);
        y += (int)(t.fontSize * 1.6f);
    };

    DrawAppText("Evaluation", (float)x, (float)y, (float)t.fontSizeSmall, t.textSection);
    y += t.fontSizeSmall + (int)(4 * t.scale);
    drawInfo("CPU:", "--");

    y += (int)(8 * t.scale);
    DrawAppText("Search", (float)x, (float)y, (float)t.fontSizeSmall, t.textSection);
    y += t.fontSizeSmall + (int)(4 * t.scale);
    drawInfo("Depth:", "--");
    drawInfo("Nodes/s:", "--");
    drawInfo("Best Line:", "--");

    y += (int)(8 * t.scale);
    DrawAppText("Statistics", (float)x, (float)y, (float)t.fontSizeSmall, t.textSection);
    y += t.fontSizeSmall + (int)(4 * t.scale);
    drawInfo("Win Rate:", "52%");
    drawInfo("Branches:", "--");
}

} // namespace ui

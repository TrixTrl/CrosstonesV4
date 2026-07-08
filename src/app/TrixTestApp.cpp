#include "TrixTestApp.h"
#include "trix-bots/utils/Utils.h"

void TrixTestApp::onStart() {
    layout = { .dir = ui::Dir::Col, .padding = {20, 20, 20, 20}, .gap = 16, .children = {
        { .id = "title",  .fixed = 30 },
        { .id = "text",   .fixed = 20 },
        { .id = "runBtn", .fixed = 40 },
    }};
}

Rectangle TrixTestApp::runButtonRect() {
    ui::Slot* slot = layout.find("runBtn");
    Rectangle r = slot ? slot->rect : Rectangle{0, 0, 0, 0};
    return {r.x, r.y, 160 * theme.scale, r.height};
}

void TrixTestApp::onTick(float dt, const InputState& input) {
    if (runButton.handleInput(runButtonRect(), input.mouse, input.clicked)) {
        Utils::runCode();
    }
}

void TrixTestApp::onDraw(Rectangle rect) {
    ui::Slot* titleSlot = layout.find("title");
    Rectangle titleRect = titleSlot ? titleSlot->rect : rect;
    drawText("Trix Test App", (int)titleRect.x, (int)titleRect.y, theme.fontSizeTitle, theme.text);
    drawText("Your own debug/benchmark app ;)", (int)titleRect.x, (int)(titleRect.y + 30 * theme.scale), theme.fontSize, theme.textDim);
    runButton.draw(runButtonRect(), "Run", theme);
}

#include "DCTestApp.h"
#include "raylib.h"
#include "game-suite/DCTestSuite.h"
#include <string>

void DCTestApp::onStart() {
    testComplete = false;
    running = true;
    status = "Running DCTestSuite...";

    auto onBoardChanged = [this]() {};

    testThread = std::make_unique<StackThread>([this, onBoardChanged]() {
        DCTestSuite::run(onBoardChanged, &boardView.position().pieces);
        testComplete = true;
        status = "Test complete. Close window to exit.";
    });
}

void DCTestApp::onTick(float dt, const InputState& /*input*/) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        running = false;
    }
}

void DCTestApp::onDraw(Rectangle rect) {
    ui::Slot* boardSlot = layout.find("board");
    boardView.draw(boardSlot ? boardSlot->rect : rect, theme.scale);
}

void DCTestApp::onDrawOverlay(Rectangle rect) {
    ui::Slot* boardSlot = layout.find("board");
    float boardBottom = boardSlot ? boardSlot->rect.y + boardSlot->rect.height : rect.y + rect.height;
    drawText(status.c_str(), (int)(10 * theme.scale), (int)boardBottom + (int)(10 * theme.scale),
             theme.fontSizeTitle, testComplete ? theme.textAccent : theme.text);
}

void DCTestApp::onStop() {
    if (testThread && testThread->joinable())
        testThread->detach();
}

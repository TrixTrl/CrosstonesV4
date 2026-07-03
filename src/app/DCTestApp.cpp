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
        DCTestSuite::run(onBoardChanged, &displayBoard.pieces);
        testComplete = true;
        status = "Test complete. Close window to exit.";
    });
}

void DCTestApp::onFrame() {
    if (IsKeyPressed(KEY_ESCAPE)) {
        running = false;
        stop();
    }

    App::onFrame();
}

void DCTestApp::onDrawOverlay() {
    drawText(status.c_str(), 10, boardPx() + 10, 20, testComplete ? GREEN : WHITE);
}

void DCTestApp::onStop() {
    if (testThread && testThread->joinable())
        testThread->detach();
}

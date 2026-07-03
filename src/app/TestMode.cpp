#include "TestMode.h"
#include "BoardDraw.h"
#include "FontManager.h"
#include "game-suite/DCTestSuite.h"
#include <string>
#include <thread>

void TestMode::run(const AppConfig& config) {
    InitWindow(13 * cellSize, 13 * cellSize + 40, "Crosstones V4 - Test Suite");
    SetTargetFPS(60);
    initAppFont();

    uint8_t displayBoard[13][13] = {0};

    bool running = true;
    bool testComplete = false;
    std::string status = "Running DCTestSuite...";

    auto onBoardChanged = [&]() {
        // flag is checked in the draw loop
    };

    std::thread testThread([&]() {
        DCTestSuite::run(onBoardChanged, &displayBoard);
        testComplete = true;
        status = "Test complete. Close window to exit.";
    });

    while (!WindowShouldClose() && running) {
        if (IsKeyPressed(KEY_ESCAPE)) running = false;

        BeginDrawing();
        drawBoard(&displayBoard);
        DrawAppText(status.c_str(), 10, 13 * cellSize + 10, 20, testComplete ? GREEN : WHITE);
        EndDrawing();
    }

    if (testThread.joinable()) testThread.detach();
    cleanupAppFont();

    CloseWindow();
}

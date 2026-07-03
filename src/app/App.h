#pragma once
#include <atomic>
#include <string_view>
#include <cstring>
#include "raylib.h"
#include "Config.h"
#include "BoardDraw.h"
#include "FontManager.h"
#include "game-suite/GamePosition.h"

class App {
public:
    virtual ~App() = default;

    void run(const AppConfig& cfg) {
        config = &cfg;
        windowWidth = calcWindowWidth();
        windowHeight = calcWindowHeight();

        InitWindow(windowWidth, windowHeight, title().data());
        SetTargetFPS(targetFPS());
        initAppFont();

        onStart();

        while (!WindowShouldClose() && !stopRequested) {
            onFrame();
        }

        onStop();

        cleanupAppFont();
        CloseWindow();
    }

    void stop() { stopRequested = true; }

protected:
    virtual std::string_view title() const = 0;
    virtual int  targetFPS()     const { return 30; }
    virtual int  extraWidth()    const { return 0; }
    virtual int  extraHeight()   const { return 0; }

    virtual void onStart() {}
    virtual void onFrame() {
        BeginDrawing();
        drawBoard(displayBoard, highlights);
        onDrawOverlay();
        EndDrawing();
    }
    virtual void onDrawOverlay() {}
    virtual void onStop() {}

    virtual int calcWindowWidth()  const { return boardPx() + extraWidth(); }
    virtual int calcWindowHeight() const { return boardPx() + extraHeight(); }

    int  boardPx() const { return 13 * 75; }
    int  panelX()  const { return boardPx() + 20; }
    int  panelW()  const { return 300; }

    void drawBoard(const GamePosition& state, const bool highlights[13][13] = nullptr) {
        ::drawBoard(state, highlights);
    }

    void drawText(const char* text, int x, int y, int size, Color color) {
        DrawAppText(text, (float)x, (float)y, size, color);
    }

    float measureText(const char* text, int size) {
        return MeasureAppText(text, size);
    }

    void drawRightPanel() {
        DrawRectangle(panelX(), 0, windowWidth - panelX(), windowHeight, Color{25, 25, 40, 255});
        DrawLine(panelX() - 1, 0, panelX() - 1, windowHeight, Color{100, 100, 130, 255});
    }

    const AppConfig* config = nullptr;
    int windowWidth  = 0;
    int windowHeight = 0;

    GamePosition displayBoard{};
    bool highlights[13][13]{};

private:
    std::atomic<bool> stopRequested{false};
};

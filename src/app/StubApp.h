#pragma once
#include "App.h"

class StubApp : public App {
protected:
    std::string_view title() const override { return "Crosstones V4 - Stub"; }
    int extraWidth()  const override { return 320; }

    void onStart()  override {}
    void onFrame()  override {
        BeginDrawing();
        ClearBackground(Color{25, 25, 40, 255});
        drawText("Stub App — works!", 50, 50, 24, GREEN);
        EndDrawing();
    }
};

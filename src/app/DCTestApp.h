#pragma once
#include "App.h"
#include "StackThread.h"
#include <string>
#include <memory>

class DCTestApp : public App {
protected:
    std::string_view title() const override { return "Crosstones V4 - DC Test Suite"; }
    int extraHeight() const override { return 40; }
    int targetFPS()  const override { return 60; }

    void onStart() override;
    void onFrame() override;
    void onStop() override;
    void onDrawOverlay() override;

private:
    bool testComplete = false;
    bool running = true;
    std::string status = "Running DCTestSuite...";
    std::unique_ptr<StackThread> testThread;
};

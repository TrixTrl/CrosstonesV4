#pragma once
#include "App.h"
#include "ui/components/board.h"
#include "StackThread.h"
#include <string>
#include <memory>

class DCTestApp : public App {
protected:
    void onStart() override;
    void onTick(float) override;
    void onDraw(Rectangle) override;
    void onDrawOverlay(Rectangle) override;
    void onStop() override;

private:
    bool testComplete = false;
    bool running = true;
    std::string status = "Running DCTestSuite...";
    std::unique_ptr<StackThread> testThread;
    ui::Board boardView;
};

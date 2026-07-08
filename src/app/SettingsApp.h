#pragma once
#include "App.h"

class SettingsApp : public App {
protected:
    void onStart() override;
    void onDraw(Rectangle rect) override;

private:
    int selectedIdx = 2; // index into scaleOpts (1.0)
    static constexpr float scaleOpts[10] = {0.5f, 0.75f, 1.0f, 1.25f, 1.5f, 2.0f, 2.5f, 3.0f, 3.5f, 4.0f};
};

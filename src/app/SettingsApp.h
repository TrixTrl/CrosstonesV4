#pragma once
#include "App.h"
#include "ui/scale.h"

class SettingsApp : public App {
protected:
    void injectServices(ui::UiScale* scale) override { scaleCtl = scale; }
    void onStart() override;
    void onTick(float dt, const InputState& input) override;
    void onDraw(Rectangle rect) override;

private:
    ui::UiScale* scaleCtl = nullptr; // non-owning, injected by AppHost before onStart
    int selectedIdx = 2; // index into scaleOpts (1.0)
    bool leftHover = false, rightHover = false, autoHover = false;
    static constexpr float scaleOpts[10] = {0.5f, 0.75f, 1.0f, 1.25f, 1.5f, 2.0f, 2.5f, 3.0f, 3.5f, 4.0f};
};

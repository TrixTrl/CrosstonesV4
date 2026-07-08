#pragma once
#include "App.h"
#include "ui/components/button.h"

class TrixTestApp : public App {
protected:
    void onStart() override;
    void onTick(float dt, const InputState& input) override;
    void onDraw(Rectangle rect) override;

private:
    ui::Button runButton;
    Rectangle runButtonRect();
};

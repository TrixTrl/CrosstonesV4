#pragma once
#include "App.h"
#include "AppRegistry.h"
#include <string>
#include <vector>

class LauncherMenu : public App {
public:
    AppMode selectedMode = AppMode::None;
    AppConfig resultConfig;

protected:
    std::string_view title() const override { return "Crosstones V4 - Launcher"; }
    int targetFPS() const override { return 30; }
    int calcWindowWidth()  const override { return 800; }
    int calcWindowHeight() const override {
        int base = 620;
        const auto& apps = AppRegistry::entries();
        int cols = (800 - 80) / 180;
        if (cols < 1) cols = 1;
        int rows = ((int)apps.size() + cols - 1) / cols;
        if (rows > 1) base += (rows - 1) * 60;
        return base;
    }

    void onStart() override;
    void onFrame() override;
    void onStop() override;

private:
    struct Button {
        Rectangle rect;
        std::string label;
        bool hovered = false;
    };

    Button makeButton(float x, float y, float w, float h, const std::string& label);
    bool buttonClicked(const Button& btn, Vector2 mouse, bool clicked);
    void drawButton(const Button& btn);
    void drawArrowButton(const Button& btn);

    std::vector<std::string> playerTypes;
    int p1TypeIdx = 0;
    int p2TypeIdx = 1;
    int p1Param = 4;
    int p2Param = 60;
    int gameModeIdx = 2;

    std::string p1InputText;
    std::string p2InputText;
    int activeField = 0;
    int cursorBlink = 0;

    bool gmDropdownOpen = false;
    int hoveredGmIdx = -1;
};

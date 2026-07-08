#pragma once
#include <memory>
#include <string>
#include "game-suite/fix_win32_compatibility.h"
#include "raylib.h"
#include "ui/theme.h"
#include "ui/layout.h"
#include "ui/overlay.h"
#include "ui/scale.h"
#include "FontManager.h"
#include "data/LoadedPosition.h"

struct InputState {
    Vector2 mouse{0, 0};
    bool clicked = false;
    float wheel = 0;
};

class App {
public:
    virtual ~App() = default;

    void resolveLayout(Rectangle bounds) { layout.resolve(bounds, theme.scale); }

    void applyThemeScale(float s) {
        theme.fontSize = (int)(14 * s);
        theme.fontSizeSmall = (int)(12 * s);
        theme.fontSizeTitle = (int)(16 * s);
        theme.itemH = (int)(28 * s);
        theme.spacing = (int)(4 * s);
        theme.scale = s;
    }

    LoadedPosition loadedPosition;
    std::string title;

    virtual void onStart() {}
    virtual void onTick(float dt, const InputState& input) {}
    virtual void onDraw(Rectangle rect) = 0;
    virtual void onDrawOverlay(Rectangle rect) {}
    virtual void onStop() {}

    // Hook for AppHost to hand a screen a service it needs beyond the base
    // App interface. Every screen but SettingsApp (the only scale writer)
    // no-ops this.
    virtual void injectServices(ui::UiScale*) {}

protected:
    ui::Theme theme;
    ui::Slot layout;

    void drawText(const char* text, int x, int y, int size, Color color) {
        DrawAppText(text, (float)x, (float)y, size, color);
    }

    float measureText(const char* text, int size) {
        return MeasureAppText(text, size);
    }
};

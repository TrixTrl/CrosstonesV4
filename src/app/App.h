#pragma once
#include <memory>
#include <string>
#include "game-suite/fix_win32_compatibility.h"
#include "raylib.h"
#include "ui/theme.h"
#include "ui/layout.h"
#include "ui/popup.h"
#include "FontManager.h"
#include "data/LoadedPosition.h"

inline float uiScale = 1.0f;
inline bool uiScaleDirty = false;

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
    virtual void onTick(float dt) {}
    virtual void onDraw(Rectangle rect) = 0;
    virtual void onDrawOverlay(Rectangle rect) {}
    virtual void onStop() {}

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

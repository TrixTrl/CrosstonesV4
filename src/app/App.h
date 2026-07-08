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

// Snapshot of per-frame input, built once by AppHost::run() and threaded
// into onTick. All input polling (mouse/keyboard/wheel) must happen only
// in onTick; onDraw/onDrawOverlay are reads of already-computed state.
// Keyboard state is intentionally not snapshotted here: IsKeyPressed/
// IsKeyDown are cheap, global, and stateless per-frame, so screens can still
// call them directly from onTick without correctness issues — only
// mouse/click needs to be threaded, since AppHost already computes it once
// per frame for the sidebar and every screen re-polling it independently is
// the actual bug pattern this fixes.
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

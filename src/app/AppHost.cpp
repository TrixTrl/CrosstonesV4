#include "AppHost.h"
#include "GameApp.h"
#include "GameViewerApp.h"
#include "MoveExplorerApp.h"
#include "DCTestApp.h"
#include "SettingsApp.h"
#include "features/opening_explorer.h"
#include "FontManager.h"
#include "ui/overlay.h"
#include "raylib.h"
#include <memory>

void AppHost::reapplyScaleToAll() {
    applyThemeScale(uiScale.value());
    ui::applyTheme(theme);
    for (auto& child : children)
        child->applyThemeScale(uiScale.value());
}

void AppHost::onStart() {
    sidebar.entries = {
        { "G", "Game"             },
        { "O", "Opening Explorer" },
        { "V", "Game Viewer"      },
        { "M", "Move Explorer"    },
        { "T", "DC Test Suite"    },
        { "S", "Settings"         },
    };

    std::unique_ptr<App> (*factories[])() = {
        []() -> std::unique_ptr<App> { return std::make_unique<GameApp>(); },
        []() -> std::unique_ptr<App> { return std::make_unique<OpeningExplorer>(); },
        []() -> std::unique_ptr<App> { return std::make_unique<GameViewerApp>(); },
        []() -> std::unique_ptr<App> { return std::make_unique<MoveExplorerApp>(); },
        []() -> std::unique_ptr<App> { return std::make_unique<DCTestApp>(); },
        []() -> std::unique_ptr<App> { return std::make_unique<SettingsApp>(); },
    };

    for (auto& factory : factories) {
        auto child = factory();
        child->applyThemeScale(uiScale.value());
        child->injectServices(&uiScale);
        child->onStart();
        children.push_back(std::move(child));
    }
}

void AppHost::onStop() {
    for (auto& child : children)
        child->onStop();
}

void AppHost::run() {
    InitWindow(1280, 720, "Crosstones V4");
    Vector2 dpiScale = GetWindowScaleDPI();
    float detected = (dpiScale.x + dpiScale.y) * 0.5f;
    if (detected < 1.0f) detected = 1.0f;
    uiScale.set(detected);
    uiScale.consumeDirty(); // startup detect isn't a "dynamic rescale"
    SetWindowSize((int)(1280 * uiScale.value()), (int)(720 * uiScale.value()));
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetWindowMinSize((int)(800 * uiScale.value()), (int)(600 * uiScale.value()));
    SetTargetFPS(30);
    initAppFont();

    reapplyScaleToAll();
    theme.bg = {25, 25, 40, 255};

    onStart();

    while (!WindowShouldClose()) {
        int w = GetScreenWidth();
        int h = GetScreenHeight();
        Vector2 mouse = GetMousePosition();
        bool click = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
        InputState input{ mouse, click, GetMouseWheelMove() };

        float s = theme.scale;
        float sidebarW = sidebar.open ? 200.0f * s : 40.0f * s;
        Rectangle sidebarRect = {0, 0, sidebarW, (float)h};
        Rectangle contentRect = {sidebarW, 0, (float)w - sidebarW, (float)h};

        // ── Sidebar input ──
        bool hamburgerClicked = false;
        int clicked = sidebar.handleClick(sidebarRect, mouse, click, hamburgerClicked, theme);
        if (hamburgerClicked) sidebar.open = !sidebar.open;
        if (clicked >= 0) activeApp = clicked;
        sidebar.activeIdx = activeApp;

        // ── Tick active child ──
        ui::overlayBegin();
        if (activeApp >= 0 && activeApp < (int)children.size()) {
            children[activeApp]->resolveLayout(contentRect);
            children[activeApp]->onTick(GetFrameTime(), input);
        }

        // ── Draw ──
        BeginDrawing();
        ClearBackground(theme.bg);

        sidebar.draw(sidebarRect, theme);

        if (activeApp >= 0 && activeApp < (int)children.size()) {
            children[activeApp]->onDraw(contentRect);
            children[activeApp]->onDrawOverlay(contentRect);
        }

        ui::overlayFlush(theme);
        EndDrawing();

        // ── Dynamic re-scale ──
        if (uiScale.consumeDirty()) {
            reapplyScaleToAll();
        }
    }

    onStop();
    cleanupAppFont();
    CloseWindow();
}

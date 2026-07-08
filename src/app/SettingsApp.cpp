#include "SettingsApp.h"
#include "FontManager.h"
#include "raylib.h"
#include <algorithm>
#include <cmath>

void SettingsApp::onStart() {
    float best = 999;
    for (int i = 0; i < 6; i++) {
        float d = std::abs(scaleOpts[i] - uiScale);
        if (d < best) { best = d; selectedIdx = i; }
    }
}

void SettingsApp::onDraw(Rectangle rect) {
    Vector2 mouse = GetMousePosition();
    bool click = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    float contentLeft = rect.x;
    float cx = rect.x + rect.width / 2;

    DrawAppText("Settings", cx - MeasureAppText("Settings", (float)theme.fontSizeTitle * 1.5f) / 2,
                rect.y + 30 * theme.scale, (float)theme.fontSizeTitle * 1.5f, WHITE);

    float labelY = rect.y + 100 * theme.scale;
    DrawAppText("UI Scale", cx - MeasureAppText("UI Scale", (float)theme.fontSize) / 2,
                labelY, (float)theme.fontSize, theme.text);

    float ctrlY = labelY + theme.itemH + theme.spacing;
    float arrowW = theme.itemH;
    float labelW = MeasureAppText("1.00x", (float)theme.fontSizeTitle) + 20 * theme.scale;
    float totalW = arrowW * 2 + 8 * theme.scale + labelW;
    float leftX = cx - totalW / 2;

    Rectangle leftR = {leftX, ctrlY, arrowW, (float)theme.itemH};
    Rectangle valR = {leftX + arrowW + 4 * theme.scale, ctrlY, labelW, (float)theme.itemH};
    Rectangle rightR = {leftX + arrowW + 4 * theme.scale + labelW + 4 * theme.scale, ctrlY, arrowW, (float)theme.itemH};

    // Left arrow
    bool leftH = CheckCollisionPointRec(mouse, leftR);
    DrawRectangleRec(leftR, leftH ? theme.panelHover : theme.panel);
    DrawRectangleLinesEx(leftR, 1, theme.border);
    float lw = MeasureAppText("<", (float)theme.fontSizeTitle);
    DrawAppText("<", leftR.x + (arrowW - lw) / 2, leftR.y + (theme.itemH - (float)theme.fontSizeTitle) / 2,
                (float)theme.fontSizeTitle, WHITE);

    // Value display
    char buf[16];
    snprintf(buf, sizeof(buf), "%.2fx", scaleOpts[selectedIdx]);
    DrawRectangleRec(valR, Color{0, 0, 0, 0});
    float vw = MeasureAppText(buf, (float)theme.fontSizeTitle);
    DrawAppText(buf, valR.x + (valR.width - vw) / 2, valR.y + (theme.itemH - (float)theme.fontSizeTitle) / 2,
                (float)theme.fontSizeTitle, theme.textAccent);

    // Right arrow
    bool rightH = CheckCollisionPointRec(mouse, rightR);
    DrawRectangleRec(rightR, rightH ? theme.panelHover : theme.panel);
    DrawRectangleLinesEx(rightR, 1, theme.border);
    float rw = MeasureAppText(">", (float)theme.fontSizeTitle);
    DrawAppText(">", rightR.x + (arrowW - rw) / 2, rightR.y + (theme.itemH - (float)theme.fontSizeTitle) / 2,
                (float)theme.fontSizeTitle, WHITE);

    if (click) {
        if (leftH && selectedIdx > 0) { 
            selectedIdx--; 
            uiScale = scaleOpts[selectedIdx]; 
            uiScaleDirty = true; 
        }
        if (rightH && selectedIdx < std::size(SettingsApp::scaleOpts)) { 
            selectedIdx++; 
            uiScale = scaleOpts[selectedIdx]; 
            uiScaleDirty = true; 
        }
    }

    // Auto-detect button
    float btnY = ctrlY + theme.itemH + 20 * theme.scale;
    float btnW = 200 * theme.scale;
    float btnH = theme.itemH * 1.2f;
    Rectangle autoR = {cx - btnW / 2, btnY, btnW, btnH};
    bool autoH = CheckCollisionPointRec(mouse, autoR);
    DrawRectangleRec(autoR, autoH ? theme.panelHover : theme.panel);
    DrawRectangleLinesEx(autoR, 1, theme.border);
    float tw = MeasureAppText("Auto-detect", (float)theme.fontSizeTitle);
    DrawAppText("Auto-detect", autoR.x + (btnW - tw) / 2,
                autoR.y + (btnH - (float)theme.fontSizeTitle) / 2,
                (float)theme.fontSizeTitle, WHITE);

    if (click && autoH) {
        Vector2 dpiScale = GetWindowScaleDPI();
        float detected = (dpiScale.x + dpiScale.y) * 0.5f;
        if (detected < 1.0f) detected = 1.0f;
        uiScale = detected;
        float best = 999;
        for (int i = 0; i < 6; i++) {
            float d = std::abs(scaleOpts[i] - detected);
            if (d < best) { best = d; selectedIdx = i; }
        }
        uiScaleDirty = true;
    }
}

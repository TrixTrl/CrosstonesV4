#include "SettingsApp.h"
#include "FontManager.h"
#include "raylib.h"
#include <algorithm>
#include <cmath>

namespace {

struct SettingsRects {
    Rectangle leftR, valR, rightR, autoR;
};

SettingsRects computeRects(Rectangle rect, const ui::Theme& theme) {
    float cx = rect.x + rect.width / 2;
    float labelY = rect.y + 100 * theme.scale;
    float ctrlY = labelY + theme.itemH + theme.spacing;
    float arrowW = theme.itemH;
    float labelW = MeasureAppText("1.00x", (float)theme.fontSizeTitle) + 20 * theme.scale;
    float totalW = arrowW * 2 + 8 * theme.scale + labelW;
    float leftX = cx - totalW / 2;

    Rectangle leftR = {leftX, ctrlY, arrowW, (float)theme.itemH};
    Rectangle valR = {leftX + arrowW + 4 * theme.scale, ctrlY, labelW, (float)theme.itemH};
    Rectangle rightR = {leftX + arrowW + 4 * theme.scale + labelW + 4 * theme.scale, ctrlY, arrowW, (float)theme.itemH};

    float btnY = ctrlY + theme.itemH + 20 * theme.scale;
    float btnW = 200 * theme.scale;
    float btnH = theme.itemH * 1.2f;
    Rectangle autoR = {cx - btnW / 2, btnY, btnW, btnH};

    return { leftR, valR, rightR, autoR };
}

}

void SettingsApp::onStart() {
    float initial = scaleCtl ? scaleCtl->value() : 1.0f;
    float best = 999;
    for (int i = 0; i < 6; i++) {
        float d = std::abs(scaleOpts[i] - initial);
        if (d < best) { best = d; selectedIdx = i; }
    }
}

void SettingsApp::onTick(float dt, const InputState& input) {
    Rectangle rect = layout.rect;
    SettingsRects r = computeRects(rect, theme);

    leftHover = CheckCollisionPointRec(input.mouse, r.leftR);
    rightHover = CheckCollisionPointRec(input.mouse, r.rightR);
    autoHover = CheckCollisionPointRec(input.mouse, r.autoR);

    if (!input.clicked || !scaleCtl) return;

    if (leftHover && selectedIdx > 0) {
        selectedIdx--;
        scaleCtl->set(scaleOpts[selectedIdx]);
    }
    if (rightHover && selectedIdx < (int)std::size(scaleOpts) - 1) {
        selectedIdx++;
        scaleCtl->set(scaleOpts[selectedIdx]);
    }
    if (autoHover) {
        Vector2 dpiScale = GetWindowScaleDPI();
        float detected = (dpiScale.x + dpiScale.y) * 0.5f;
        if (detected < 1.0f) detected = 1.0f;
        scaleCtl->set(detected);
        float best = 999;
        for (int i = 0; i < 6; i++) {
            float d = std::abs(scaleOpts[i] - detected);
            if (d < best) { best = d; selectedIdx = i; }
        }
    }
}

void SettingsApp::onDraw(Rectangle rect) {
    float cx = rect.x + rect.width / 2;

    DrawAppText("Settings", cx - MeasureAppText("Settings", (float)theme.fontSizeTitle * 1.5f) / 2,
                rect.y + 30 * theme.scale, (float)theme.fontSizeTitle * 1.5f, WHITE);

    float labelY = rect.y + 100 * theme.scale;
    DrawAppText("UI Scale", cx - MeasureAppText("UI Scale", (float)theme.fontSize) / 2,
                labelY, (float)theme.fontSize, theme.text);

    SettingsRects r = computeRects(rect, theme);
    float arrowW = theme.itemH;

    // Left arrow
    DrawRectangleRec(r.leftR, leftHover ? theme.panelHover : theme.panel);
    DrawRectangleLinesEx(r.leftR, 1, theme.border);
    float lw = MeasureAppText("<", (float)theme.fontSizeTitle);
    DrawAppText("<", r.leftR.x + (arrowW - lw) / 2, r.leftR.y + (theme.itemH - (float)theme.fontSizeTitle) / 2,
                (float)theme.fontSizeTitle, WHITE);

    // Value display
    char buf[16];
    snprintf(buf, sizeof(buf), "%.2fx", scaleOpts[selectedIdx]);
    float vw = MeasureAppText(buf, (float)theme.fontSizeTitle);
    DrawAppText(buf, r.valR.x + (r.valR.width - vw) / 2, r.valR.y + (theme.itemH - (float)theme.fontSizeTitle) / 2,
                (float)theme.fontSizeTitle, theme.textAccent);

    // Right arrow
    DrawRectangleRec(r.rightR, rightHover ? theme.panelHover : theme.panel);
    DrawRectangleLinesEx(r.rightR, 1, theme.border);
    float rw = MeasureAppText(">", (float)theme.fontSizeTitle);
    DrawAppText(">", r.rightR.x + (arrowW - rw) / 2, r.rightR.y + (theme.itemH - (float)theme.fontSizeTitle) / 2,
                (float)theme.fontSizeTitle, WHITE);

    // Auto-detect button
    DrawRectangleRec(r.autoR, autoHover ? theme.panelHover : theme.panel);
    DrawRectangleLinesEx(r.autoR, 1, theme.border);
    float tw = MeasureAppText("Auto-detect", (float)theme.fontSizeTitle);
    DrawAppText("Auto-detect", r.autoR.x + (r.autoR.width - tw) / 2,
                r.autoR.y + (r.autoR.height - (float)theme.fontSizeTitle) / 2,
                (float)theme.fontSizeTitle, WHITE);
}

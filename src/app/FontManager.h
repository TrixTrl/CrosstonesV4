#pragma once
#include "raylib.h"
#include <vector>

extern Font appFont;

static inline void initAppFont() {
    std::vector<int> codepoints;
    for (int c = 32; c <= 255; c++) codepoints.push_back(c);
    for (int c = 0x2460; c <= 0x2473; c++) codepoints.push_back(c);

    appFont = LoadFontEx("resources/DejaVuSans.ttf", 96, codepoints.data(), (int)codepoints.size());
    if (appFont.texture.id != 0) {
        SetTextureFilter(appFont.texture, TEXTURE_FILTER_BILINEAR);
    }
}

static inline void cleanupAppFont() {
    if (appFont.texture.id != 0)
        UnloadFont(appFont);
}

static inline void DrawAppText(const char* text, float x, float y, float fontSize, Color color) {
    Font f = appFont.texture.id ? appFont : GetFontDefault();
    DrawTextEx(f, text, {x, y}, fontSize, 0, color);
}

static inline float MeasureAppText(const char* text, float fontSize) {
    Font f = appFont.texture.id ? appFont : GetFontDefault();
    return MeasureTextEx(f, text, fontSize, 0).x;
}

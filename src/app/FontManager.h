#pragma once
#include "raylib.h"

extern Font appFont;

static inline void initAppFont() {
    appFont = LoadFontEx("resources/FiraSans-Regular.ttf", 96, 0, 250);
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

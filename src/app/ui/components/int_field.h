#pragma once
#include <string>
#include "raylib.h"
#include "app/ui/theme.h"

namespace ui {

struct IntField {
    int value = 0;
    std::string text;
    bool active = false;
    int cursorBlink = 0;
    int minVal = 0;
    int maxVal = 999;

    void draw(Rectangle rect, const Theme& t);
    void handleInput(Rectangle rect, Vector2 mouse, bool click);
    void commit();
};

}

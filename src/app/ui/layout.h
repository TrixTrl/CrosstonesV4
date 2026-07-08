#pragma once
#include <string>
#include <string_view>
#include <vector>
#include "raylib.h"

namespace ui {

enum class Dir { Row, Col };

struct Padding {
    float top = 0, right = 0, bottom = 0, left = 0;
};

struct Slot {
    Rectangle rect;
    std::string id;
    float fixed = 0;
    float flex = 1;
    float minPx = 0;
    Dir dir = Dir::Row;
    Padding padding{};
    float gap = 0;
    std::vector<Slot> children;

    void resolve(Rectangle bounds, float scale = 1.0f);
    Slot* find(std::string_view id);
};

}

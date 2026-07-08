#pragma once
#include <functional>
#include <string>
#include <string_view>
#include "theme.h"

namespace ui {

void overlayBegin();
void overlayPush(std::string_view id, std::function<void(const Theme&)> draw, int zOrder = 0);
void overlayFlush(const Theme& theme);

}

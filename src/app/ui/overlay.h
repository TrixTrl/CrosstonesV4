#pragma once
#include <functional>
#include <string>
#include <string_view>
#include "theme.h"

namespace ui {

// Replaces ui/popup.h's schedulePopupDraw/flushPopupDraws lambda queue,
// which was a z-ordering workaround for raygui and captured `this` into
// deferred draws — letting a widget's state mutate between "scheduled" and
// "flushed" within the same frame. Callers here must capture a value
// snapshot of everything their draw needs (never `this` or a live
// reference), so the queued draw can never observe a half-mutated widget.
//
// Lifecycle: AppHost::run() calls overlayBegin() once per frame before
// ticking the active screen, and overlayFlush(theme) once, after
// onDraw+onDrawOverlay, before EndDrawing() — mirroring exactly where
// flushPopupDraws() ran before. Nothing persists across frames: an overlay
// must be re-pushed every frame it should stay visible (Dropdown::draw
// already naturally does this every frame while `open == true`).

void overlayBegin();
void overlayPush(std::string_view id, std::function<void(const Theme&)> draw, int zOrder = 0);
void overlayFlush(const Theme& theme);

}

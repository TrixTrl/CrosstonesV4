#include "scroll_area.h"
#include "raygui.h"

namespace ui {

Rectangle ScrollArea::begin(Rectangle bounds) {
    Rectangle content = {0, 0, bounds.width, (float)contentH};
    Rectangle view;
    if (GuiScrollPanel(bounds, NULL, content, &scroll, &view)) {
        return view;
    }
    return {};
}

void ScrollArea::end() {
}

} // namespace ui

#include "scroll_area.h"
#include "raygui.h"

namespace ui {

Rectangle ScrollArea::begin(Rectangle bounds, int contentHeight) {
    contentH = contentHeight;
    // Content width must stay strictly below bounds.width - 2*borderWidth,
    // or GuiScrollPanel's `content.width > bounds.width - 2*border` check
    // spuriously triggers a horizontal scrollbar even though there is
    // nothing to scroll horizontally (content never actually exceeds the
    // panel width here — only its height does).
    float contentWidth = bounds.width - 4;
    Rectangle content = {0, 0, contentWidth, (float)contentH};
    Rectangle view;
    GuiScrollPanel(bounds, NULL, content, &scroll, &view);
    BeginScissorMode((int)view.x, (int)view.y, (int)view.width, (int)view.height);
    return view;
}

void ScrollArea::end() {
    EndScissorMode();
}

} // namespace ui

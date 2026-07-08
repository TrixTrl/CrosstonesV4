#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "theme.h"

namespace ui {

void applyTheme(const Theme& t) {
    // ── DEFAULT (applies to all controls as base) ──
    GuiSetStyle(DEFAULT, BACKGROUND_COLOR,       ColorToInt(t.bg));
    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL,      ColorToInt(t.panel));
    GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED,     ColorToInt(t.panelHover));
    GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED,     ColorToInt(t.panelActive));
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL,      ColorToInt(t.text));
    GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED,     ColorToInt(t.text));
    GuiSetStyle(DEFAULT, TEXT_COLOR_PRESSED,     ColorToInt(t.textAccent));
    GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL,    ColorToInt(t.border));
    GuiSetStyle(DEFAULT, BORDER_COLOR_FOCUSED,   ColorToInt(t.border));
    GuiSetStyle(DEFAULT, BORDER_COLOR_PRESSED,   ColorToInt(t.textAccent));
    GuiSetStyle(DEFAULT, LINE_COLOR,             ColorToInt(t.border));
    GuiSetStyle(DEFAULT, TEXT_SIZE,              t.fontSize);
    GuiSetStyle(DEFAULT, BORDER_WIDTH,           1);

    // ── LABEL ──
    GuiSetStyle(LABEL, TEXT_COLOR_NORMAL,        ColorToInt(t.text));
    GuiSetStyle(LABEL, TEXT_COLOR_FOCUSED,       ColorToInt(t.text));
    GuiSetStyle(LABEL, TEXT_COLOR_PRESSED,       ColorToInt(t.textAccent));

    // ── BUTTON ──
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL,       ColorToInt(t.panel));
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED,      ColorToInt(t.panelHover));
    GuiSetStyle(BUTTON, BASE_COLOR_PRESSED,      ColorToInt(t.panelActive));
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL,       ColorToInt(t.text));
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED,      ColorToInt(t.text));
    GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED,      ColorToInt(t.textAccent));
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL,     ColorToInt(t.border));
    GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED,    ColorToInt(t.border));
    GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED,    ColorToInt(t.textAccent));

    // ── DROPDOWNBOX ──
    GuiSetStyle(DROPDOWNBOX, BASE_COLOR_NORMAL,  ColorToInt(t.panel));
    GuiSetStyle(DROPDOWNBOX, BASE_COLOR_FOCUSED, ColorToInt(t.panelHover));
    GuiSetStyle(DROPDOWNBOX, BASE_COLOR_PRESSED, ColorToInt(t.panelHover));
    GuiSetStyle(DROPDOWNBOX, TEXT_COLOR_NORMAL,  ColorToInt(t.text));
    GuiSetStyle(DROPDOWNBOX, TEXT_COLOR_FOCUSED, ColorToInt(t.text));
    GuiSetStyle(DROPDOWNBOX, TEXT_COLOR_PRESSED, ColorToInt(t.textAccent));
    GuiSetStyle(DROPDOWNBOX, BORDER_COLOR_NORMAL,ColorToInt(t.border));

    // ── TEXTBOX ──
    GuiSetStyle(TEXTBOX, BASE_COLOR_NORMAL,      ColorToInt(t.panel));
    GuiSetStyle(TEXTBOX, BASE_COLOR_FOCUSED,     ColorToInt(t.panelHover));
    GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL,      ColorToInt(t.text));
    GuiSetStyle(TEXTBOX, TEXT_COLOR_FOCUSED,     ColorToInt(t.text));
    GuiSetStyle(TEXTBOX, BORDER_COLOR_NORMAL,    ColorToInt(t.border));
    GuiSetStyle(TEXTBOX, BORDER_COLOR_FOCUSED,   ColorToInt(t.textAccent));

    // ── LISTVIEW ──
    GuiSetStyle(LISTVIEW, BASE_COLOR_NORMAL,     ColorToInt(t.panel));
    GuiSetStyle(LISTVIEW, BASE_COLOR_FOCUSED,    ColorToInt(t.panelHover));
    GuiSetStyle(LISTVIEW, BASE_COLOR_PRESSED,    ColorToInt(t.panelActive));
    GuiSetStyle(LISTVIEW, TEXT_COLOR_NORMAL,     ColorToInt(t.text));
    GuiSetStyle(LISTVIEW, TEXT_COLOR_FOCUSED,    ColorToInt(t.text));
    GuiSetStyle(LISTVIEW, TEXT_COLOR_PRESSED,    ColorToInt(t.textAccent));
    GuiSetStyle(LISTVIEW, BORDER_COLOR_NORMAL,   ColorToInt(t.border));
}

} // namespace ui

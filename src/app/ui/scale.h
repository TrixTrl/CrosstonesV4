#pragma once

namespace ui {

// Owns the current UI scale factor. AppHost holds the single instance;
// SettingsApp is the only screen that ever calls set() (it gets a
// non-owning pointer injected by AppHost before onStart).
class UiScale {
public:
    float value() const { return v; }

    void set(float newVal) {
        v = newVal;
        dirty = true;
    }

    bool consumeDirty() {
        bool d = dirty;
        dirty = false;
        return d;
    }

private:
    float v = 1.0f;
    bool dirty = false;
};

}

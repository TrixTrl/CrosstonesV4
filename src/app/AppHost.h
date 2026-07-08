#pragma once
#include "App.h"
#include "ui/components/sidebar.h"
#include <memory>
#include <vector>
#include <string_view>

class AppHost : public App {
public:
    void run();

protected:
    void onStart() override;
    void onStop() override;
    void onDraw(Rectangle) override {}
    void onDrawOverlay(Rectangle) override {}

private:
    ui::Sidebar sidebar;
    std::vector<std::unique_ptr<App>> children;
    int activeApp = 0;

    void reapplyScaleToAll();
};

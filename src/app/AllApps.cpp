#include "AppRegistry.h"
#include "MoveExplorerApp.h"
#include "GameViewerApp.h"
#include "GameApp.h"
#include "DCTestApp.h"
#include "StubApp.h"
#include "game-suite/GameMaster.h"

void registerAllApps() {
    AppRegistry::registerApp(AppMode::Game, "Play Game",
        "Play against bots or friends",
        []() { return std::make_unique<GameApp>(); });

    AppRegistry::registerApp(AppMode::Explore, "Move Gen Explorer",
        "Browse and explore game positions",
        []() { return std::make_unique<MoveExplorerApp>(); });

    AppRegistry::registerApp(AppMode::Viewer, "Game Viewer",
        "View game replays",
        []() { return std::make_unique<GameViewerApp>(); });

    AppRegistry::registerApp(AppMode::Test, "DC Test Suite",
        "Run test suites",
        []() { return std::make_unique<DCTestApp>(); });

    // AppRegistry::registerApp(AppMode::Stub, "Stub App",
    //     "Test button wrapping",
    //     []() { return std::make_unique<StubApp>(); });
}

#ifndef UNICODE
#define UNICODE
#endif

#include "game-suite/fix_win32_compatibility.h"

#include "app/Config.h"
#include "app/LauncherMenu.h"
#include "app/AppRegistry.h"
#include "app/FontManager.h"
#include "app/StackThread.h"
#include <iostream>

Font appFont;

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
// Also used by StackThread for background-thread stack size
#pragma comment(linker, "/STACK:" STR(CUSTOM_STACK_SIZE))
#pragma comment(linker, "/HEAP:200000000")

int main(int argc, char* argv[]) {
    registerAllApps();

    // Try loading config file first
    AppConfig config = AppConfig::fromFile("crosstones_config.txt");

    // Override with CLI args
    AppConfig cliConfig = AppConfig::fromArgs(argc, argv);
    if (argc > 1) {
        if (cliConfig.mode == AppMode::None) {
            std::cerr << "Usage: crosstones [game|explore|viewer|test] [options...]" << std::endl;
            std::cerr << "  --p1 <type> <param>    Player 1 (Deepchad, AlphaCruncher, Hydra, TheFirst, ManualPlayer)" << std::endl;
            std::cerr << "  --p2 <type> <param>    Player 2" << std::endl;
            std::cerr << "  --mode <bits>          Game mode in binary (e.g. 010 for bases, 111 for all)" << std::endl;
            std::cerr << "  --position <pos_str>   Starting position string" << std::endl;
            std::cerr << "  --gamecode <code>      Game code string for viewer" << std::endl;
            std::cerr << "  --config <path>        Config file path" << std::endl;
            return 1;
        }
        config.mode = cliConfig.mode;
        if (cliConfig.game.player1.type != "Deepchad" || cliConfig.game.player1.param != 4)
            config.game.player1 = cliConfig.game.player1;
        if (cliConfig.game.player2.type != "AlphaCruncher" || cliConfig.game.player2.param != 60)
            config.game.player2 = cliConfig.game.player2;
        if (cliConfig.game.gameModeBits != 2)
            config.game.gameModeBits = cliConfig.game.gameModeBits;
        if (!cliConfig.game.position.empty())
            config.game.position = cliConfig.game.position;
        if (!cliConfig.explore.position.empty())
            config.explore.position = cliConfig.explore.position;
        if (!cliConfig.viewer.gameCode.empty())
            config.viewer.gameCode = cliConfig.viewer.gameCode;
    }

    // No mode selected via CLI → show launcher menu
    if (config.mode == AppMode::None) {
        LauncherMenu launcher;
        launcher.run(config);
        if (launcher.selectedMode != AppMode::None) {
            config = launcher.resultConfig;
        } else {
            return 0;
        }
    }

    // Run selected mode via registry
    auto app = AppRegistry::create(config.mode);
    if (app) {
        app->run(config);
    } else {
        std::cerr << "No mode selected." << std::endl;
        return 1;
    }

    return 0;
}

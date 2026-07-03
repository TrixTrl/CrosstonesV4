#include "LauncherMenu.h"
#include "raylib.h"
#include "FontManager.h"
#include <string>
#include <vector>
#include <algorithm>

static const int screenW = 800;
static const int screenH = 620;

// Dark mode color scheme
static const Color BG           = {25, 25, 40, 255};
static const Color BTN_NORMAL   = {50, 50, 75, 255};
static const Color BTN_HOVER    = {70, 70, 100, 255};
static const Color BTN_BORDER   = {100, 100, 130, 255};
static const Color INPUT_BG     = {35, 35, 55, 255};
static const Color INPUT_BORDER = {120, 120, 160, 255};
static const Color DROPDOWN_BG  = {45, 45, 70, 255};
static const Color DROPDOWN_HVR = {70, 70, 100, 255};

struct Button {
    Rectangle rect;
    std::string label;
    bool hovered = false;
};

static Button makeButton(float x, float y, float w, float h,
                          const std::string& label) {
    return Button{{x, y, w, h}, label};
}

static bool buttonClicked(const Button& btn, Vector2 mouse, bool clicked) {
    return clicked && CheckCollisionPointRec(mouse, btn.rect);
}

static void drawButton(const Button& btn, bool isColored = false) {
    Color c = btn.hovered ? BTN_HOVER : BTN_NORMAL;
    DrawRectangleRec(btn.rect, c);
    DrawRectangleLinesEx(btn.rect, 1, BTN_BORDER);
    DrawAppText(btn.label.c_str(),
                btn.rect.x + btn.rect.width / 2 - MeasureAppText(btn.label.c_str(), 16) / 2,
                btn.rect.y + btn.rect.height / 2 - 8, 16, WHITE);
}

static void drawArrowButton(const Button& btn) {
    Color c = btn.hovered ? BTN_HOVER : BTN_NORMAL;
    DrawRectangleRec(btn.rect, c);
    DrawRectangleLinesEx(btn.rect, 1, BTN_BORDER);
    DrawAppText(btn.label.c_str(),
                btn.rect.x + btn.rect.width / 2 - MeasureAppText(btn.label.c_str(), 16) / 2,
                btn.rect.y + btn.rect.height / 2 - 8, 16, WHITE);
}

AppConfig LauncherMenu::show(const AppConfig& defaults) {
    InitWindow(screenW, screenH, "Crosstones V4 - Launcher");
    SetTargetFPS(30);
    initAppFont();

    AppConfig config;
    config.mode = AppMode::None;
    bool selected = false;

    // Player type options
    std::vector<std::string> playerTypes = {"Deepchad", "AlphaCruncher",
                                            "Hydra", "TheFirst", "ManualPlayer"};
    auto findType = [&](const std::string& name) -> int {
        for (size_t i = 0; i < playerTypes.size(); i++)
            if (playerTypes[i] == name) return (int)i;
        return -1;
    };
    int p1TypeIdx = findType(defaults.game.player1.type);
    if (p1TypeIdx < 0) p1TypeIdx = 0;
    int p2TypeIdx = findType(defaults.game.player2.type);
    if (p2TypeIdx < 0) p2TypeIdx = 1;
    int p1Param = defaults.game.player1.param;
    int p2Param = defaults.game.player2.param;
    int gameModeIdx = defaults.game.gameModeBits;

    // Text input fields for params
    std::string p1InputText = std::to_string(p1Param);
    std::string p2InputText = std::to_string(p2Param);
    int activeField = 0; // 0 = none, 1 = p1, 2 = p2
    int cursorBlink = 0;

    const Rectangle p1InputRect = {90, 180, 100, 30};
    const Rectangle p2InputRect = {470, 180, 100, 30};

    const char* gameModeNames[] = {
        "Phoenix",          // 0b000
        "Monkey",           // 0b001 
        "Lotus",            // 0b010 
        "Kylin",            // 0b011
        "Tortoise",         // 0b100
        "Orchid",           // 0b101
        "Crane",            // 0b110
        "Dragon",           // 0b111
    };
    const int numGameModes = 8;
    bool gmDropdownOpen = false;
    int hoveredGmIdx = -1;
    const Rectangle gmDropdownRect = { 250, 240, 300, 30 };

    while (!WindowShouldClose() && !selected) {
        Vector2 mouse = GetMousePosition();
        bool click = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
        cursorBlink++;

        // Mode selection buttons
        Button gameBtn = makeButton(50, 70, 160, 40, "Play Game");
        Button exploreBtn = makeButton(230, 70, 160, 40, "Explore Moves");
        Button viewerBtn = makeButton(410, 70, 160, 40, "Game Viewer");
        Button testBtn = makeButton(590, 70, 160, 40, "Test Suite");

        // Player type arrow buttons
        Button p1Prev = makeButton(50, 145, 30, 30, "<");
        Button p1Next = makeButton(310, 145, 30, 30, ">");
        Button p2Prev = makeButton(430, 145, 30, 30, "<");
        Button p2Next = makeButton(690, 145, 30, 30, ">");

        if (!gmDropdownOpen && activeField == 0) {
            gameBtn.hovered = CheckCollisionPointRec(mouse, gameBtn.rect);
            exploreBtn.hovered = CheckCollisionPointRec(mouse, exploreBtn.rect);
            viewerBtn.hovered = CheckCollisionPointRec(mouse, viewerBtn.rect);
            testBtn.hovered = CheckCollisionPointRec(mouse, testBtn.rect);
            p1Prev.hovered = CheckCollisionPointRec(mouse, p1Prev.rect);
            p1Next.hovered = CheckCollisionPointRec(mouse, p1Next.rect);
            p2Prev.hovered = CheckCollisionPointRec(mouse, p2Prev.rect);
            p2Next.hovered = CheckCollisionPointRec(mouse, p2Next.rect);
        }

        // --- Handle clicks ---
        if (activeField == 0 && !gmDropdownOpen) {
            if (buttonClicked(gameBtn, mouse, click)) { config.mode = AppMode::Game; selected = true; }
            if (buttonClicked(exploreBtn, mouse, click)) { config.mode = AppMode::Explore; selected = true; }
            if (buttonClicked(viewerBtn, mouse, click)) { config.mode = AppMode::Viewer; selected = true; }
            if (buttonClicked(testBtn, mouse, click)) { config.mode = AppMode::Test; selected = true; }
            if (buttonClicked(p1Prev, mouse, click)) p1TypeIdx = (p1TypeIdx - 1 + (int)playerTypes.size()) % playerTypes.size();
            if (buttonClicked(p1Next, mouse, click)) p1TypeIdx = (p1TypeIdx + 1) % playerTypes.size();
            if (buttonClicked(p2Prev, mouse, click)) p2TypeIdx = (p2TypeIdx - 1 + (int)playerTypes.size()) % playerTypes.size();
            if (buttonClicked(p2Next, mouse, click)) p2TypeIdx = (p2TypeIdx + 1) % playerTypes.size();
        }

        // --- Text input field interaction ---
        bool clickedInput1 = click && CheckCollisionPointRec(mouse, p1InputRect);
        bool clickedInput2 = click && CheckCollisionPointRec(mouse, p2InputRect);

        if (click) {
            if (clickedInput1) {
                activeField = 1;
            } else if (clickedInput2) {
                activeField = 2;
            } else if (activeField != 0 && !CheckCollisionPointRec(mouse, p1InputRect) && !CheckCollisionPointRec(mouse, p2InputRect)) {
                // Commit current field
                if (activeField == 1) {
                    if (!p1InputText.empty()) p1Param = std::clamp(std::stoi(p1InputText), 0, 999);
                    else p1Param = 0;
                    p1InputText = std::to_string(p1Param);
                } else if (activeField == 2) {
                    if (!p2InputText.empty()) p2Param = std::clamp(std::stoi(p2InputText), 0, 999);
                    else p2Param = 0;
                    p2InputText = std::to_string(p2Param);
                }
                activeField = 0;
            }
        }

        // --- Text input keyboard handling ---
        if (activeField == 1) {
            int c = GetCharPressed();
            while (c > 0) {
                if (c >= '0' && c <= '9' && p1InputText.length() < 3) {
                    if (p1InputText == "0") p1InputText.clear();
                    p1InputText += (char)c;
                }
                c = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) && !p1InputText.empty()) {
                p1InputText.pop_back();
            }
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_TAB)) {
                if (!p1InputText.empty()) p1Param = std::clamp(std::stoi(p1InputText), 0, 999);
                else p1Param = 0;
                p1InputText = std::to_string(p1Param);
                if (IsKeyPressed(KEY_TAB)) activeField = 2;
                else activeField = 0;
            }
            if (IsKeyPressed(KEY_ESCAPE)) {
                p1InputText = std::to_string(p1Param);
                activeField = 0;
            }
        } else if (activeField == 2) {
            int c = GetCharPressed();
            while (c > 0) {
                if (c >= '0' && c <= '9' && p2InputText.length() < 3) {
                    if (p2InputText == "0") p2InputText.clear();
                    p2InputText += (char)c;
                }
                c = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) && !p2InputText.empty()) {
                p2InputText.pop_back();
            }
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_TAB)) {
                if (!p2InputText.empty()) p2Param = std::clamp(std::stoi(p2InputText), 0, 999);
                else p2Param = 0;
                p2InputText = std::to_string(p2Param);
                activeField = IsKeyPressed(KEY_TAB) ? 2 : 0;
            }
            if (IsKeyPressed(KEY_ESCAPE)) {
                p2InputText = std::to_string(p2Param);
                activeField = 0;
            }
        }

        // --- Game mode dropdown interaction ---
        hoveredGmIdx = -1;
        if (gmDropdownOpen) {
            for (int p = 0; p < numGameModes; p++) {
                Rectangle optRect = { gmDropdownRect.x, gmDropdownRect.y + gmDropdownRect.height + p * gmDropdownRect.height, gmDropdownRect.width, gmDropdownRect.height };
                if (CheckCollisionPointRec(mouse, optRect)) {
                    hoveredGmIdx = p;
                    if (click) {
                        gameModeIdx = p;
                        gmDropdownOpen = false;
                    }
                    break;
                }
            }
            if (click && hoveredGmIdx == -1 && !CheckCollisionPointRec(mouse, gmDropdownRect)) {
                gmDropdownOpen = false;
            }
        } else {
            if (click && CheckCollisionPointRec(mouse, gmDropdownRect)) {
                gmDropdownOpen = true;
            }
        }

        // TAB to cycle game mode (only when no text field is active)
        if (activeField == 0 && IsKeyPressed(KEY_TAB) && !gmDropdownOpen) {
            gameModeIdx = (gameModeIdx + 1) % numGameModes;
        }

        // Build config from UI state
        config.game.player1.type = playerTypes[p1TypeIdx];
        config.game.player1.param = p1Param;
        config.game.player2.type = playerTypes[p2TypeIdx];
        config.game.player2.param = p2Param;
        config.game.gameModeBits = gameModeIdx;

        // --- Draw ---
        BeginDrawing();
        ClearBackground(BG);

        DrawAppText("Crosstones V4", 50, 20, 24, WHITE);
        DrawAppText("Select a mode:", 50, 50, 16, LIGHTGRAY);
        drawButton(gameBtn);
        drawButton(exploreBtn);
        drawButton(viewerBtn);
        drawButton(testBtn);

        // Player 1
        DrawAppText("Player 1 (White):", 50, 120, 16, WHITE);
        drawArrowButton(p1Prev);
        std::string p1Label = playerTypes[p1TypeIdx];
        DrawAppText(p1Label.c_str(), 90, 152, 16, WHITE);
        drawArrowButton(p1Next);

        // Param input field for P1
        DrawRectangleRec(p1InputRect, activeField == 1 ? DROPDOWN_BG : INPUT_BG);
        DrawRectangleLinesEx(p1InputRect, 1, activeField == 1 ? INPUT_BORDER : BTN_BORDER);
        std::string p1Display = p1InputText.empty() ? "0" : p1InputText;
        if (activeField == 1 && (cursorBlink / 15) % 2 == 0) {
            p1Display += "|";
        }
        DrawAppText(p1Display.c_str(), p1InputRect.x + 8, p1InputRect.y + 7, 16, WHITE);
        DrawAppText("depth/time", 200, 188, 14, GRAY);

        // Player 2
        DrawAppText("Player 2 (Black):", 430, 120, 16, WHITE);
        drawArrowButton(p2Prev);
        std::string p2Label = playerTypes[p2TypeIdx];
        DrawAppText(p2Label.c_str(), 470, 152, 16, WHITE);
        drawArrowButton(p2Next);

        // Param input field for P2
        DrawRectangleRec(p2InputRect, activeField == 2 ? DROPDOWN_BG : INPUT_BG);
        DrawRectangleLinesEx(p2InputRect, 1, activeField == 2 ? INPUT_BORDER : BTN_BORDER);
        std::string p2Display = p2InputText.empty() ? "0" : p2InputText;
        if (activeField == 2 && (cursorBlink / 15) % 2 == 0) {
            p2Display += "|";
        }
        DrawAppText(p2Display.c_str(), p2InputRect.x + 8, p2InputRect.y + 7, 16, WHITE);
        DrawAppText("depth/time", 580, 188, 14, GRAY);

        // Game Rules (dropdown)
        DrawAppText("Game Rules:", 50, 245, 16, WHITE);
        DrawRectangleRec(gmDropdownRect, gmDropdownOpen ? DROPDOWN_BG : DROPDOWN_BG);
        DrawRectangleLinesEx(gmDropdownRect, 1, BTN_BORDER);
        std::string gmName = gameModeNames[gameModeIdx];
        DrawAppText(gmName.c_str(), gmDropdownRect.x + 10, gmDropdownRect.y + 7, 16, WHITE);
        DrawAppText(gmDropdownOpen ? "^" : "v", gmDropdownRect.x + gmDropdownRect.width - 20, gmDropdownRect.y + 7, 16, WHITE);

        if (gmDropdownOpen) {
            for (int p = 0; p < numGameModes; p++) {
                Rectangle optRect = { gmDropdownRect.x, gmDropdownRect.y + gmDropdownRect.height + p * gmDropdownRect.height, gmDropdownRect.width, gmDropdownRect.height };
                DrawRectangleRec(optRect, hoveredGmIdx == p ? DROPDOWN_HVR : DROPDOWN_BG);
                DrawRectangleLinesEx(optRect, 1, BTN_BORDER);
                DrawAppText(gameModeNames[p], optRect.x + 10, optRect.y + 7, 14, hoveredGmIdx == p ? WHITE : LIGHTGRAY);
            }
        }

        DrawAppText("ESC: exit", 50, 500, 14, GRAY);

        EndDrawing();
    }
    // Save settings for next launch
    AppConfig saveCfg = config;
    saveCfg.mode = AppMode::None; // always show launcher next time
    saveCfg.toFile("crosstones_config.txt");
    cleanupAppFont();
    CloseWindow();
    return config;
}